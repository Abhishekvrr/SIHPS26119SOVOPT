#pragma once

#include <cstddef>
#include <vector>
#include <string>

namespace sovopt::math
{

/**
 * SIMD acceleration kernels for vector & matrix computations.
 * Supports AVX2 / SSE4.1 / NEON / portable loop unrolling with OpenMP/multi-threading.
 */
class SIMDVector
{
public:
    // Vector dot product with SIMD vectorization: sum(a[i] * b[i])
    static double dot_product(const double* a, const double* b, std::size_t size) noexcept;

    // Vectorized AXPY operation: y[i] = y[i] + alpha * x[i]
    static void axpy(double alpha, const double* x, double* y, std::size_t size) noexcept;

    // Vectorized scale: x[i] = x[i] * alpha
    static void scale(double* x, double alpha, std::size_t size) noexcept;

    // Vectorized Tableau Row Elimination (Simplex pivot step):
    // target_row[j] -= factor * pivot_row[j]
    static void pivot_row_elimination(double* target_row, const double* pivot_row, double factor, std::size_t columns) noexcept;

    // SIMD-accelerated Min Ratio Test for Simplex / Dual Simplex
    // Finds row with minimum positive ratio rhs[i] / column[i]
    static int min_ratio_test(const double* rhs, const double* col, const bool* eligible_rows, std::size_t rows, double tolerance) noexcept;

    // Dense Matrix-Vector Multiplication: y = A * x
    static void mat_vec_mult(const std::vector<std::vector<double>>& A, const double* x, double* y, std::size_t rows, std::size_t cols) noexcept;

    // Returns the active SIMD instruction set name (e.g. "AVX2 (256-bit FMA)", "SSE4.1 (128-bit)", or "Optimized 4x Unrolled")
    static std::string get_instruction_set() noexcept;

    // Measure benchmark speedup factor vs un-vectorized baseline
    static double benchmark_simd_speedup(std::size_t vector_size = 100000) noexcept;
};

} // namespace sovopt::math
