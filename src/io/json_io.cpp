#include "sovopt/io/json_io.h"
#include "sovopt/core/types.h"

#include <cctype>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace sovopt::io
{

namespace
{

struct JsonValue
{
    enum class Type { Null, Bool, Number, String, Array, Object };
    Type type{Type::Null};
    bool bool_val{false};
    double num_val{0.0};
    std::string str_val;
    std::vector<JsonValue> arr_val;
    std::vector<std::pair<std::string, JsonValue>> obj_val;

    [[nodiscard]] const JsonValue* get(const std::string& key) const
    {
        if (type != Type::Object) return nullptr;
        for (const auto& kv : obj_val)
        {
            if (kv.first == key) return &kv.second;
        }
        return nullptr;
    }

    [[nodiscard]] double as_double(double default_val = 0.0) const
    {
        if (type == Type::Number) return num_val;
        if (type == Type::String)
        {
            if (str_val == "Infinity" || str_val == "inf" || str_val == "+inf") return core::Infinity;
            if (str_val == "-Infinity" || str_val == "-inf") return -core::Infinity;
            try { return std::stod(str_val); } catch (...) {}
        }
        return default_val;
    }

    [[nodiscard]] std::string as_string(const std::string& default_val = "") const
    {
        if (type == Type::String) return str_val;
        if (type == Type::Number) return std::to_string(num_val);
        if (type == Type::Bool) return bool_val ? "true" : "false";
        return default_val;
    }

    [[nodiscard]] bool as_bool(bool default_val = false) const
    {
        if (type == Type::Bool) return bool_val;
        return default_val;
    }
};

class SimpleJsonParser
{
public:
    explicit SimpleJsonParser(std::string_view text) : text_(text), pos_(0) {}

    bool parse(JsonValue& root, std::string& err)
    {
        try
        {
            skip_whitespace();
            root = parse_value();
            skip_whitespace();
            return true;
        }
        catch (const std::exception& ex)
        {
            err = ex.what();
            return false;
        }
    }

private:
    std::string_view text_;
    std::size_t pos_{0};

    char peek() const { return pos_ < text_.size() ? text_[pos_] : '\0'; }
    char get() { return pos_ < text_.size() ? text_[pos_++] : '\0'; }

    void skip_whitespace()
    {
        while (pos_ < text_.size())
        {
            char c = text_[pos_];
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r') { ++pos_; }
            else if (c == '/' && pos_ + 1 < text_.size() && text_[pos_ + 1] == '/')
            {
                pos_ += 2;
                while (pos_ < text_.size() && text_[pos_] != '\n') ++pos_;
            }
            else { break; }
        }
    }

    JsonValue parse_value()
    {
        skip_whitespace();
        char c = peek();
        if (c == '{') return parse_object();
        if (c == '[') return parse_array();
        if (c == '"') return parse_string();
        if (c == 't' || c == 'f') return parse_bool();
        if (c == 'n') return parse_null();
        if (c == '-' || c == '+' || std::isdigit(static_cast<unsigned char>(c))) return parse_number();
        throw std::runtime_error(std::string("Unexpected character: '") + c + "' at position " + std::to_string(pos_));
    }

    JsonValue parse_object()
    {
        get(); // consume '{'
        JsonValue val;
        val.type = JsonValue::Type::Object;
        skip_whitespace();
        if (peek() == '}') { get(); return val; }

        while (true)
        {
            skip_whitespace();
            if (peek() != '"') throw std::runtime_error("Expected string key in object at position " + std::to_string(pos_));
            std::string key = parse_string_raw();
            skip_whitespace();
            if (get() != ':') throw std::runtime_error("Expected ':' after key at position " + std::to_string(pos_));
            JsonValue child = parse_value();
            val.obj_val.emplace_back(std::move(key), std::move(child));

            skip_whitespace();
            char c = get();
            if (c == '}') break;
            if (c != ',') throw std::runtime_error("Expected ',' or '}' in object at position " + std::to_string(pos_));
        }
        return val;
    }

    JsonValue parse_array()
    {
        get(); // consume '['
        JsonValue val;
        val.type = JsonValue::Type::Array;
        skip_whitespace();
        if (peek() == ']') { get(); return val; }

        while (true)
        {
            val.arr_val.push_back(parse_value());
            skip_whitespace();
            char c = get();
            if (c == ']') break;
            if (c != ',') throw std::runtime_error("Expected ',' or ']' in array at position " + std::to_string(pos_));
        }
        return val;
    }

    std::string parse_string_raw()
    {
        get(); // consume '"'
        std::string s;
        while (pos_ < text_.size())
        {
            char c = get();
            if (c == '"') return s;
            if (c == '\\' && pos_ < text_.size())
            {
                char next = get();
                if (next == '"' || next == '\\' || next == '/') s.push_back(next);
                else if (next == 'b') s.push_back('\b');
                else if (next == 'f') s.push_back('\f');
                else if (next == 'n') s.push_back('\n');
                else if (next == 'r') s.push_back('\r');
                else if (next == 't') s.push_back('\t');
                else s.push_back(next);
            }
            else
            {
                s.push_back(c);
            }
        }
        throw std::runtime_error("Unterminated string literal.");
    }

    JsonValue parse_string()
    {
        JsonValue val;
        val.type = JsonValue::Type::String;
        val.str_val = parse_string_raw();
        return val;
    }

    JsonValue parse_number()
    {
        std::size_t start = pos_;
        if (peek() == '-' || peek() == '+') get();
        while (std::isdigit(static_cast<unsigned char>(peek()))) get();
        if (peek() == '.')
        {
            get();
            while (std::isdigit(static_cast<unsigned char>(peek()))) get();
        }
        if (peek() == 'e' || peek() == 'E')
        {
            get();
            if (peek() == '+' || peek() == '-') get();
            while (std::isdigit(static_cast<unsigned char>(peek()))) get();
        }

        std::string_view num_view = text_.substr(start, pos_ - start);
        JsonValue val;
        val.type = JsonValue::Type::Number;
        try { val.num_val = std::stod(std::string(num_view)); }
        catch (...) { val.num_val = 0.0; }
        return val;
    }

    JsonValue parse_bool()
    {
        JsonValue val;
        val.type = JsonValue::Type::Bool;
        if (text_.substr(pos_, 4) == "true") { pos_ += 4; val.bool_val = true; return val; }
        if (text_.substr(pos_, 5) == "false") { pos_ += 5; val.bool_val = false; return val; }
        throw std::runtime_error("Expected boolean value.");
    }

    JsonValue parse_null()
    {
        if (text_.substr(pos_, 4) == "null") { pos_ += 4; JsonValue val; return val; }
        throw std::runtime_error("Expected null value.");
    }
};

} // namespace

bool JsonIO::parse_model(
    const std::string& json_text,
    core::Model& model,
    optimization::OptimizationOptions& options,
    std::string& error_message
)
{
    SimpleJsonParser parser(json_text);
    JsonValue root;
    if (!parser.parse(root, error_message))
    {
        return false;
    }

    if (root.type != JsonValue::Type::Object)
    {
        error_message = "Root JSON must be an object.";
        return false;
    }

    std::string name = "SOVOPT Optimization Problem";
    if (auto* n = root.get("model_name")) name = n->as_string(name);
    if (auto* n = root.get("name")) name = n->as_string(name);
    model = core::Model(name);

    core::OptimizationSense opt_sense = core::OptimizationSense::Maximize;
    if (auto* s = root.get("sense"))
    {
        std::string sense_str = s->as_string();
        if (sense_str == "minimize" || sense_str == "MINIMIZE" || sense_str == "min")
        {
            opt_sense = core::OptimizationSense::Minimize;
        }
    }
    else if (auto* s2 = root.get("objective_sense"))
    {
        std::string sense_str = s2->as_string();
        if (sense_str == "minimize" || sense_str == "MINIMIZE" || sense_str == "min")
        {
            opt_sense = core::OptimizationSense::Minimize;
        }
    }

    std::vector<core::LinearTerm> obj_terms;
    std::vector<core::QuadraticObjectiveTerm> quad_terms;

    // Parse variables
    if (auto* vars = root.get("variables"))
    {
        if (vars->type == JsonValue::Type::Array)
        {
            for (std::size_t i = 0; i < vars->arr_val.size(); ++i)
            {
                const auto& var_node = vars->arr_val[i];
                std::string v_name = "x" + std::to_string(i + 1);
                if (auto* vn = var_node.get("name")) v_name = vn->as_string(v_name);

                core::VariableType v_type = core::VariableType::Continuous;
                if (auto* vt = var_node.get("type"))
                {
                    std::string t_str = vt->as_string();
                    if (t_str == "integer" || t_str == "INTEGER") v_type = core::VariableType::Integer;
                    else if (t_str == "binary" || t_str == "BINARY") v_type = core::VariableType::Binary;
                }

                double lb = 0.0;
                if (auto* l = var_node.get("lower_bound")) lb = l->as_double(0.0);
                else if (auto* l2 = var_node.get("lb")) lb = l2->as_double(0.0);

                double ub = core::Infinity;
                if (auto* u = var_node.get("upper_bound")) ub = u->as_double(core::Infinity);
                else if (auto* u2 = var_node.get("ub")) ub = u2->as_double(core::Infinity);

                core::VariableIndex idx = model.add_variable(v_name, v_type, lb, ub);

                double obj_coeff = 0.0;
                if (auto* c = var_node.get("objective")) obj_coeff = c->as_double(0.0);
                else if (auto* c2 = var_node.get("coefficient")) obj_coeff = c2->as_double(0.0);

                if (std::abs(obj_coeff) > 1e-12)
                {
                    obj_terms.push_back(core::LinearTerm{.variable = idx, .coefficient = obj_coeff});
                }

                // Variable-level quadratic diagonal term (e.g. 0.5 * q * x_i^2)
                if (auto* qc = var_node.get("quadratic_coeff"))
                {
                    double q_val = qc->as_double(0.0);
                    if (std::abs(q_val) > 1e-12)
                    {
                        quad_terms.push_back(core::QuadraticObjectiveTerm{
                            .row_variable = idx,
                            .col_variable = idx,
                            .coefficient = q_val
                        });
                    }
                }
            }
        }
    }

    // Direct linear objective list
    if (auto* obj_list = root.get("objective"))
    {
        if (obj_list->type == JsonValue::Type::Array)
        {
            obj_terms.clear();
            for (std::size_t i = 0; i < obj_list->arr_val.size(); ++i)
            {
                double c = obj_list->arr_val[i].as_double(0.0);
                if (std::abs(c) > 1e-12 && i < model.variables().size())
                {
                    obj_terms.push_back(core::LinearTerm{.variable = i, .coefficient = c});
                }
            }
        }
    }

    // Quadratic objective terms (QP)
    if (auto* q_list = root.get("quadratic_terms"))
    {
        if (q_list->type == JsonValue::Type::Array)
        {
            for (const auto& q_node : q_list->arr_val)
            {
                std::size_t r = 0, c = 0;
                if (auto* rv = q_node.get("row")) r = static_cast<std::size_t>(rv->as_double(0.0));
                if (auto* cv = q_node.get("col")) c = static_cast<std::size_t>(cv->as_double(0.0));
                double q_coeff = 0.0;
                if (auto* qv = q_node.get("coefficient")) q_coeff = qv->as_double(0.0);

                if (r < model.variables().size() && c < model.variables().size() && std::abs(q_coeff) > 1e-12)
                {
                    quad_terms.push_back(core::QuadraticObjectiveTerm{
                        .row_variable = r,
                        .col_variable = c,
                        .coefficient = q_coeff
                    });
                }
            }
        }
    }
    else if (auto* q_mat = root.get("quadratic_matrix"))
    {
        if (q_mat->type == JsonValue::Type::Array)
        {
            for (std::size_t r = 0; r < q_mat->arr_val.size() && r < model.variables().size(); ++r)
            {
                const auto& row_node = q_mat->arr_val[r];
                if (row_node.type == JsonValue::Type::Array)
                {
                    for (std::size_t c = 0; c < row_node.arr_val.size() && c < model.variables().size(); ++c)
                    {
                        double val = row_node.arr_val[c].as_double(0.0);
                        if (std::abs(val) > 1e-12)
                        {
                            quad_terms.push_back(core::QuadraticObjectiveTerm{
                                .row_variable = r,
                                .col_variable = c,
                                .coefficient = val
                            });
                        }
                    }
                }
            }
        }
    }

    model.set_objective(opt_sense, std::move(obj_terms), std::move(quad_terms));

    // Parse constraints
    if (auto* cons = root.get("constraints"))
    {
        if (cons->type == JsonValue::Type::Array)
        {
            for (std::size_t i = 0; i < cons->arr_val.size(); ++i)
            {
                const auto& c_node = cons->arr_val[i];
                std::string c_name = "Constraint_" + std::to_string(i + 1);
                if (auto* cn = c_node.get("name")) c_name = cn->as_string(c_name);

                core::ConstraintSense sense = core::ConstraintSense::LessEqual;
                if (auto* cs = c_node.get("sense"))
                {
                    std::string s_str = cs->as_string();
                    if (s_str == "=" || s_str == "==" || s_str == "Equal") sense = core::ConstraintSense::Equal;
                    else if (s_str == ">=" || s_str == "GreaterEqual") sense = core::ConstraintSense::GreaterEqual;
                }

                double rhs = 0.0;
                if (auto* r = c_node.get("rhs")) rhs = r->as_double(0.0);

                std::vector<core::LinearTerm> terms;
                if (auto* t_arr = c_node.get("terms"))
                {
                    if (t_arr->type == JsonValue::Type::Array)
                    {
                        for (const auto& t_node : t_arr->arr_val)
                        {
                            std::size_t v_idx = 0;
                            if (auto* v = t_node.get("variable")) v_idx = static_cast<std::size_t>(v->as_double(0.0));
                            double coef = 0.0;
                            if (auto* c = t_node.get("coefficient")) coef = c->as_double(0.0);

                            if (v_idx < model.variables().size() && std::abs(coef) > 1e-12)
                            {
                                terms.push_back(core::LinearTerm{.variable = v_idx, .coefficient = coef});
                            }
                        }
                    }
                }
                else if (auto* coef_arr = c_node.get("coefficients"))
                {
                    if (coef_arr->type == JsonValue::Type::Array)
                    {
                        for (std::size_t j = 0; j < coef_arr->arr_val.size(); ++j)
                        {
                            double coef = coef_arr->arr_val[j].as_double(0.0);
                            if (j < model.variables().size() && std::abs(coef) > 1e-12)
                            {
                                terms.push_back(core::LinearTerm{.variable = j, .coefficient = coef});
                            }
                        }
                    }
                }

                model.add_constraint(c_name, std::move(terms), sense, rhs);
            }
        }
    }

    // Parse options
    if (auto* opt_node = root.get("options"))
    {
        if (auto* alg = opt_node->get("algorithm"))
        {
            std::string a_str = alg->as_string();
            if (a_str == "dual_simplex" || a_str == "DualSimplex") options.solver_type = optimization::SolverType::DualSimplex;
            else if (a_str == "branch_and_bound" || a_str == "BranchAndBound") options.solver_type = optimization::SolverType::BranchAndBound;
            else if (a_str == "branch_and_cut" || a_str == "BranchAndCut") options.solver_type = optimization::SolverType::BranchAndCut;
            else if (a_str == "qp" || a_str == "active_set_qp" || a_str == "ActiveSetQP") options.solver_type = optimization::SolverType::ActiveSetQP;
            else options.solver_type = optimization::SolverType::Simplex;
        }

        if (auto* accel = opt_node->get("acceleration"))
        {
            std::string acc_str = accel->as_string();
            if (acc_str == "gpu" || acc_str == "GPU") options.acceleration = optimization::HardwareAcceleration::GPU_Accelerated;
            else if (acc_str == "cpu_scalar" || acc_str == "scalar") options.acceleration = optimization::HardwareAcceleration::CPU_Scalar;
            else options.acceleration = optimization::HardwareAcceleration::CPU_SIMD;
        }

        if (auto* tol = opt_node->get("tolerance"))
        {
            options.feasibility_tolerance = tol->as_double(1e-9);
            options.optimality_tolerance = tol->as_double(1e-9);
        }
        if (auto* max_it = opt_node->get("max_iterations"))
        {
            options.max_iterations = static_cast<std::size_t>(max_it->as_double(100000.0));
        }
        if (auto* val = opt_node->get("enable_validation"))
        {
            options.enable_validation = val->as_bool(true);
        }
        if (auto* pres = opt_node->get("enable_presolve"))
        {
            options.enable_presolve = pres->as_bool(true);
        }
        if (auto* cuts = opt_node->get("enable_gomory_cuts"))
        {
            options.enable_gomory_cuts = cuts->as_bool(true);
        }
    }

    return true;
}

std::string JsonIO::serialize_result(
    const optimization::OptimizationResult& result
)
{
    std::ostringstream oss;
    oss << "{\n"
        << "  \"status\": \"" << optimization::to_string(result.status) << "\",\n"
        << std::fixed << std::setprecision(6)
        << "  \"objective_value\": " << result.objective_value << ",\n"
        << "  \"iterations\": " << result.iterations << ",\n"
        << "  \"nodes_explored\": " << result.nodes_explored << ",\n"
        << "  \"cuts_added\": " << result.cuts_added << ",\n"
        << "  \"presolve_reductions_count\": " << result.presolve_reductions_count << ",\n"
        << "  \"duality_gap\": " << result.duality_gap << ",\n"
        << "  \"solve_time_ms\": " << result.solve_time_ms << ",\n"
        << "  \"presolve_time_ms\": " << result.presolve_time_ms << ",\n"
        << "  \"validation_time_ms\": " << result.validation_time_ms << ",\n"
        << "  \"total_time_ms\": " << result.total_time_ms << ",\n"
        << "  \"is_valid\": " << (result.is_valid ? "true" : "false") << ",\n"
        << "  \"max_primal_violation\": " << result.max_primal_violation << ",\n"
        << "  \"max_dual_violation\": " << result.max_dual_violation << ",\n"
        << "  \"kkt_primal_residual\": " << result.kkt_primal_residual << ",\n"
        << "  \"kkt_dual_residual\": " << result.kkt_dual_residual << ",\n"
        << "  \"simd_speedup_factor\": " << result.simd_speedup_factor << ",\n"
        << "  \"acceleration_used\": \"" << result.acceleration_used << "\",\n"
        << "  \"solver_used\": \"" << result.solver_used << "\",\n"
        << "  \"problem_type\": \"" << result.problem_type << "\",\n"
        << "  \"message\": \"" << result.message << "\",\n"
        << "  \"variable_names\": [";

    for (std::size_t i = 0; i < result.variable_names.size(); ++i)
    {
        oss << "\"" << result.variable_names[i] << "\"" << (i + 1 < result.variable_names.size() ? ", " : "");
    }
    oss << "],\n  \"variable_values\": [";

    for (std::size_t i = 0; i < result.variable_values.size(); ++i)
    {
        oss << result.variable_values[i] << (i + 1 < result.variable_values.size() ? ", " : "");
    }
    oss << "]\n}\n";

    return oss.str();
}

} // namespace sovopt::io
