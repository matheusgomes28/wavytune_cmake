#ifndef FOURIER_MATRIX_EXCEPTION_H
#define FOURIER_MATRIX_EXCEPTION_H

#include <stdexcept>

namespace wt
{
	class MatrixException : public std::runtime_error
	{
	public:
		MatrixException(const std::string& message)
			: std::runtime_error(message)
		{
		}
	};

} // namespace wt

#endif // FOURIER_MATRIX_EXCEPTION_H
