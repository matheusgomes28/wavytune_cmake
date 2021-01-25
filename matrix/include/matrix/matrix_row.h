#ifndef FOURIER_MATRIX_ROW_H
#define FOURIER_MATRIX_ROW_H

#include <matrix/matrix_exception.h>

#include <cstdlib>
#include <type_traits>

namespace wt
{
  template <typename T, typename _Matrix_Traits> class Matrix_T;

  template <typename T, typename _Matrix_Traits> class MatrixRow
  {
    using T_non_cost = typename std::remove_const<T>::type;

  public:
    T &operator[](std::size_t i)
    {
      if (i < data_size_)
      {
        return data_ptr_[i];
      }
      throw MatrixException("row given is out of bounds");
    }

    // Maths operators
    MatrixRow<T, _Matrix_Traits> &operator*(T val)
    {
      for (std::size_t i = 0; i < data_size_; ++i)
      {
        data_ptr_[i] *= val;
      }
      return *this;
    }

    MatrixRow<T, _Matrix_Traits> &
    operator+(const MatrixRow<T, _Matrix_Traits> &B)
    {
      if (B.data_size_ != data_size_)
      {
        throw MatrixException("cannot add row with different sizes");
      }

      for (std::size_t i = 0; i < data_size_; ++i)
      {
        data_ptr_[i] += B.data_ptr_[i];
      }
      return *this;
    }

    MatrixRow<T, _Matrix_Traits> &
    operator-(const MatrixRow<T, _Matrix_Traits> &B)
    {
      if (B.data_size_ != data_size_)
      {
        throw MatrixException("cannod subtract rows with different sizes");
      }

      for (std::size_t i = 0; i < data_size_; ++i)
      {
        data_ptr_[i] -= B.data_ptr_[i];
      }
      return *this;
    }

    MatrixRow<T, _Matrix_Traits> &
    operator=(const std::initializer_list<T> &values)
    {
      if (values.size() == data_size_)
      {
        for (std::size_t i = 0; i < data_size_; ++i)
        {
          data_ptr_[i] = *(values.begin() + i);
        }
        return *this;
      }
      throw MatrixException("value sizes do not match row size");
    }

  private:
    MatrixRow(T *data_ptr, std::size_t data_size)
        : data_ptr_{data_ptr}, data_size_{data_size}
    {
    }

    T *data_ptr_;
    std::size_t data_size_;

    friend class Matrix_T<T, _Matrix_Traits>;
    friend class Matrix_T<T_non_cost, _Matrix_Traits>;
  };

} // namespace wt

#endif // FOURIER_MATRIX_ROW_H
