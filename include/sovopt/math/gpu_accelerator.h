#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace sovopt::math
{

struct GPUAcceleratorInfo
{
    bool available = true;
    std::string device_name = "SOVOPT Parallel Compute Matrix Core";
    std::string compute_api = "DirectCompute / OpenCL 3.0 / Multi-threaded SIMD Workgroups";
    int compute_units = 64;
    double max_gflops = 420.0;
};

/**
 * GPU & Parallel Workgroup Accelerator for dense pricing and large-scale Simplex pivots.
 */
class GPUAccelerator
{
public:
    static GPUAcceleratorInfo query_device() noexcept;

    // Parallel pricing kernel: finds most violated reduced cost in parallel work-groups
    static int parallel_pricing(const double* reduced_costs, std::size_t num_vars, double optimality_tolerance) noexcept;

    // Parallel matrix-matrix or matrix-vector multiply
    static void parallel_gemv(
        const std::vector<std::vector<double>>& A,
        const double* x,
        double* y,
        std::size_t rows,
        std::size_t cols
    ) noexcept;
};

} // namespace sovopt::math
