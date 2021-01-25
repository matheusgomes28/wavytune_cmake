#ifndef FOURIER_DFT_H
#define FOURIER_DFT_H

#include <complex>
#include <queue> // for the windowing
#include <vector>

namespace wt
{
  namespace ft
  {
    template <typename T> using Window = std::vector<std::complex<T>>;

    template <typename T> using Frequencies = std::vector<std::complex<T>>;

    // This class performs a *slow* fourier
    // transform. This should only be using for
    // debugging tests and checking for correctness
    // of FFT
    // @tparam T is the numeric type for the transform.
    // Note that the numeric type should support multiplication
    // & division.
    template <typename T> class DFT
    {
    public:
      virtual Frequencies<T> back_transform() = 0;
      virtual Frequencies<T> front_transform() = 0;
      virtual void feed(const Window<T> &window) = 0;
    };
  } // namespace ft
} // namespace wt

#endif // FOURIER_DFT_H
