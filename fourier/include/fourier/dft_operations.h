#ifndef FOURIER_SLOW_FFT_H
#define FOURIER_SLOW_FFT_H

// Includes from this project
#include <matrix/matrix.h>

// Includes from the std
#include <cmath>
#include <functional>
#include <vector>


namespace wt
{
  namespace ft
  {
    #define MATH_PI 3.14159265358979323846


    enum class FFT_PARTITION : char
    {
      ODD,
      EVEN
    };

    using namespace wt;
    using signal = std::vector<std::complex<double>>;
    Matrix<std::complex<double>> slow_fft(const signal& window);
    std::vector<std::size_t> partition_indices(const signal& in, std::vector<FFT_PARTITION>& order);
    Matrix<std::complex<double>> fast_fft(const signal& input);

    // TODO : This should be in the matrix  project
    template <typename T, typename U>
    Matrix<T> apply(std::function<T(const U&)> f, const Matrix<U>& m)
    {
      Matrix<T> result{m.n_rows(), m.n_cols()};
      for (std::size_t i = 0; i < m.n_rows(); ++i)
      {
        auto m_row = m[i];
        auto result_row = result[i];
        for (std::size_t j = 0; j < m.n_cols(); ++j)
        {
          result_row[j] = f(m_row[j]);
        }
      }
      return result;
    }
  } // namespace ft
} // namespace wt
#endif // FOURIER_SLOW_FFT_H

