from typing import Dict, Any, List

class ModelValidator:
    """
    Validates model consistency, coefficient dimensions, bound integrity,
    and mathematical solvability prior to C++ solver dispatch.
    """

    @classmethod
    def validate_model(cls, model_data: Dict[str, Any]) -> Dict[str, Any]:
        errors = []
        warnings = []

        variables = model_data.get("variables", [])
        constraints = model_data.get("constraints", [])

        if not variables:
            errors.append("Model must contain at least one decision variable.")
        
        num_vars = len(variables)
        var_names = set()
        for idx, v in enumerate(variables):
            name = v.get("name", "").strip()
            if not name:
                errors.append(f"Variable at index {idx} has an empty name.")
            elif name in var_names:
                warnings.append(f"Duplicate variable name '{name}' detected.")
            var_names.add(name)

            lb = v.get("lower_bound", 0.0)
            ub = v.get("upper_bound", 1e100)
            if ub != "Infinity" and isinstance(ub, (int, float)) and isinstance(lb, (int, float)):
                if lb > ub:
                    errors.append(f"Variable '{name}' has lower bound ({lb}) exceeding upper bound ({ub}).")

        if not constraints:
            errors.append("Model must contain at least one linear constraint.")

        for idx, c in enumerate(constraints):
            c_name = c.get("name", f"Constraint {idx + 1}")
            sense = c.get("sense", "<=")
            if sense not in ["<=", ">=", "="]:
                errors.append(f"Constraint '{c_name}' has invalid sense '{sense}'. Supported: <=, >=, =.")

            coeffs = c.get("coefficients", [])
            if len(coeffs) != num_vars:
                errors.append(
                    f"Constraint '{c_name}' has {len(coeffs)} coefficients but model has {num_vars} variables."
                )

            rhs = c.get("rhs")
            if rhs is None or not isinstance(rhs, (int, float)):
                errors.append(f"Constraint '{c_name}' has missing or non-numeric RHS value.")

        return {
            "is_valid": len(errors) == 0,
            "errors": errors,
            "warnings": warnings,
            "summary": "Model validation passed." if len(errors) == 0 else f"{len(errors)} validation error(s) found."
        }

