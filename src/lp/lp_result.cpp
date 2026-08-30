#include "sovopt/lp/lp_result.h"

namespace sovopt::lp {

const char* to_string(LPStatus status) noexcept
{
    switch (status) {
    case LPStatus::Optimal:
        return "OPTIMAL";

    case LPStatus::Infeasible:
        return "INFEASIBLE";

    case LPStatus::Unbounded:
        return "UNBOUNDED";

    case LPStatus::IterationLimit:
        return "ITERATION_LIMIT";

    case LPStatus::NumericalError:
        return "NUMERICAL_ERROR";

    case LPStatus::InvalidModel:
        return "INVALID_MODEL";
    }

    return "UNKNOWN";
}

} // namespace sovopt::lp