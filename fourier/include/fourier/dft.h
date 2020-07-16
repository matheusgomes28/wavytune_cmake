#ifndef FOURIER_DFT_H
#define FOURIER_DFT_H

#include <queue> // for the windowing
#include <vector>
#include <complex>

// This class performs a *slow* fourier
// transform. This should only be using for 
// debugging tests and checking for correctness
// of FFT
// @tparam T is the numeric type for the transform.
// Note that the numeric type should support multiplication
// & division.
template <typename T>
class DFT
{
public:
	using Window = std::vector<std::complex<T>>;
	using Frequencies = std::vector<std::complex<T>>;

	virtual Frequencies back_transform() = 0;
	virtual Frequencies front_transform() = 0;
	virtual void feed(const Window& window) = 0;
};
#endif // FOURIER_DFT_H
