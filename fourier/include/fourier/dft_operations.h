#ifndef FOURIER_SLOW_FFT_H
#define FOURIER_SLOW_FFT_H

// Includes from this project
#include <matrix/matrix.h>

// Includes from the std
#include <cmath>
#include <vector>

#define MATH_PI 3.14159265358979323846
namespace wt::ft
{
    using namespace wt::matrix;

    enum class FFT_PARTITION : char
    {
      ODD,
      EVEN
    };

    using namespace wt;
    using signal = std::vector<std::complex<double>>;

    Matrix<std::complex<double>> slow_fft(const signal &window);

    std::vector<std::size_t>
    partition_indices(const signal &in, std::vector<FFT_PARTITION> &order);

    Matrix<std::complex<double>> fast_fft(const signal &input);
} // namespace wt::ft
#endif // FOURIER_SLOW_FFT_H
