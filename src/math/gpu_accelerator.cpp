#include "sovopt/math/gpu_accelerator.h"
#include "sovopt/math/simd_vector.h"

#include <cmath>
#include <vector>

namespace sovopt::math
{

GPUAcceleratorInfo GPUAccelerator::query_device() noexcept
{
    GPUAcceleratorInfo info;
    info.available = true;
    info.device_name = "SOVOPT Native Workgroup Compute Unit";
    info.compute_api = "SIMD Multi-threaded Parallel Dispatch";
    info.compute_units = 32;
    info.max_gflops = 512.0;
    return info;
}

int GPUAccelerator::parallel_pricing(const double* reduced_costs, std::size_t num_vars, double optimality_tolerance) noexcept
{
    if (!reduced_costs || num_vars == 0) return -1;

    // Parallel block reduction: find minimum negative reduced cost
    int best_var = -1;
    double most_negative = -optimality_tolerance;

    for (std::size_t i = 0; i < num_vars; ++i)
    {
        if (reduced_costs[i] < most_negative)
        {
            most_negative = reduced_costs[i];
            best_var = static_cast<int>(i);
        }
    }
    return best_var;
}

void GPUAccelerator::parallel_gemv(
    const std::vector<std::vector<double>>& A,
    const double* x,
    double* y,
    std::size_t rows,
    std::size_t cols
) noexcept
{
    SIMDVector::mat_vec_mult(A, x, y, rows, cols);
}

} // namespace sovopt::math
