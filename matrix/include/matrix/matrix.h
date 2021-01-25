#ifndef FOURIER_MATRIX_H
#define FOURIER_MATRIX_H

#include <matrix/matrix_exception.h>
#include <matrix/matrix_row.h>
#include <matrix/matrix_traits.h>

#include <cstdlib>
#include <type_traits>
#include <vector>

namespace wt
{
  template <typename T, typename _Matrix_Traits> class Matrix_T
  {
    using non_const_T = typename std::remove_const<T>::type;

  public:
    Matrix_T(std::size_t n_rows, std::size_t n_cols)
        : n_rows_{n_rows}, n_cols_{n_cols}, data_ptr_{new T[n_rows * n_cols]}
    {
      for (std::size_t row = 0; row < n_rows_; ++row)
      {
        for (std::size_t col = 0; col < n_cols_; ++col)
        {
          data_ptr_[row * n_cols_ + col] = _Matrix_Traits::default_val;
        }
      }
    }

    Matrix_T(const Matrix_T &cp)
        : data_ptr_{nullptr}, n_cols_{cp.n_cols_}, n_rows_{cp.n_rows_}
    {
      std::size_t data_size = cp.n_rows_ * cp.n_cols_;
      data_ptr_ = new T[data_size];
      std::copy(cp.data_ptr_, cp.data_ptr_ + data_size, data_ptr_);
    }

    Matrix_T(Matrix_T &&mv) noexcept
        : data_ptr_{mv.data_ptr_}, n_rows_{mv.n_rows_}, n_cols_{mv.n_cols_}
    {
      mv.n_cols_ = 0;
      mv.n_rows_ = 0;
      mv.data_ptr_ = nullptr;
    }

    ~Matrix_T()
    {
      if (data_ptr_)
      {
        delete[] data_ptr_;
      }
    }

    std::size_t n_rows() const { return n_rows_; }

    std::size_t n_cols() const { return n_cols_; }

    MatrixRow<T, _Matrix_Traits> operator[](std::size_t row)
    {
      if (row < n_rows_)
      {
        return MatrixRow<T, _Matrix_Traits>(data_ptr_ + n_cols_ * row, n_cols_);
      }
      throw MatrixException("the row given is out of bounds");
    }

    MatrixRow<const T, _Matrix_Traits> operator[](std::size_t row) const
    {
      if (row < n_rows_)
      {
        return MatrixRow<const T, _Matrix_Traits>(data_ptr_ + n_cols_ * row,
                                                  n_cols_);
      }
      throw MatrixException("the row given is out of bounds");
    }

    // Scalar operations
    friend Matrix_T<non_const_T, _Matrix_Traits>
    operator*(T lhs, const Matrix_T<T, _Matrix_Traits> &rhs)
    {
      Matrix_T<non_const_T, _Matrix_Traits> result{rhs.n_rows_, rhs.n_cols_};
      for (std::size_t row = 0; row < rhs.n_rows_; ++row)
      {
        MatrixRow<const T, _Matrix_Traits> curr_row = rhs[row];
        MatrixRow<non_const_T, _Matrix_Traits> result_row = result[row];

        for (std::size_t col = 0; col < rhs.n_cols_; ++col)
        {
          result_row[col] = curr_row[col] * lhs;
        }
      }
      return result;
    }

    friend Matrix_T<non_const_T, _Matrix_Traits>
    operator*(const Matrix_T<T, _Matrix_Traits> &lhs, T rhs)
    {
      return rhs * lhs;
    }

    // Matrix operations
    Matrix_T<non_const_T, _Matrix_Traits>
    operator+(const Matrix_T<T, _Matrix_Traits> &B) const
    {
      if ((n_cols_ == B.n_cols_) && (n_rows_ == B.n_rows_))
      {
        Matrix_T<non_const_T, _Matrix_Traits> result{n_rows_, B.n_cols_};

        for (std::size_t row = 0; row < n_rows_; ++row)
        {
          MatrixRow<const T, _Matrix_Traits> A_row = (*this)[row];
          MatrixRow<const T, _Matrix_Traits> B_row = B[row];
          MatrixRow<non_const_T, _Matrix_Traits> result_row = result[row];

          for (std::size_t col = 0; col < n_cols_; ++col)
          {
            result_row[col] = A_row[col] + B_row[col];
          }
        }

        return result;
      }
      throw MatrixException("cannot add matrices with different dimensions");
    }

    Matrix_T<non_const_T, _Matrix_Traits>
    operator-(const Matrix_T<T, _Matrix_Traits> &B) const
    {
      if ((n_cols_ == B.n_cols_) && (n_rows_ == B.n_rows_))
      {
        Matrix_T<non_const_T, _Matrix_Traits> result{n_rows_, B.n_cols_};
        for (std::size_t row = 0; row < n_rows_; ++row)
        {
          MatrixRow<const T, _Matrix_Traits> A_row = (*this)[row];
          MatrixRow<const T, _Matrix_Traits> B_row = B[row];
          MatrixRow<non_const_T, _Matrix_Traits> result_row = result[row];

          for (std::size_t col = 0; col < n_cols_; ++col)
          {
            result_row[col] = A_row[col] - B_row[col];
          }
        }
        return result;
      }
      throw MatrixException(
          "cannot  subtract matrices with different dimensions");
    }

    Matrix_T<non_const_T, _Matrix_Traits>
    operator*(const Matrix_T<T, _Matrix_Traits> &B) const
    {
      if (n_cols_ == B.n_rows_)
      {
        Matrix_T<non_const_T, _Matrix_Traits> result{n_rows_, B.n_cols_};

        for (std::size_t row = 0; row < n_rows_; ++row)
        {
          MatrixRow<const T, _Matrix_Traits> A_row = (*this)[row];
          MatrixRow<non_const_T, _Matrix_Traits> result_row = result[row];
          for (std::size_t col = 0; col < B.n_cols_; ++col)
          {
            for (std::size_t i = 0; i < n_rows_; ++i)
            {
              result_row[col] += A_row[i] * B.data_ptr_[i * B.n_cols_ + col];
            }
          }
        }
        return result;
      }
      throw MatrixException(
          "cannot multiply matrices with different inner dimensions");
    }

    Matrix_T<non_const_T, _Matrix_Traits>
    operator*(const std::vector<T> &col_vec)
    {
      if (n_cols_ == col_vec.size())
      {
        Matrix_T<non_const_T, _Matrix_Traits> result{n_rows_, 1};
        for (std::size_t row = 0; row < n_rows_; ++row)
        {
          MatrixRow<non_const_T, _Matrix_Traits> this_row = (*this)[row];
          MatrixRow<non_const_T, _Matrix_Traits> result_row = result[row];
          for (std::size_t i = 0; i < col_vec.size(); ++i)
          {
            result_row[0] += this_row[i] * col_vec[i];
          }
        }

        return result;
      }
      throw MatrixException(
          "cannot multiply matrix by vector of different size");
    }

    Matrix_T<T, _Matrix_Traits> &
    operator=(const std::initializer_list<T> &values)
    {
      if (values.size() == n_rows_ * n_cols_)
      {
        for (std::size_t row = 0; row < n_rows_; ++row)
        {
          MatrixRow<T, _Matrix_Traits> curr_row = (*this)[row];
          for (std::size_t col = 0; col < n_cols_; ++col)
          {
            curr_row[col] = *(values.begin() + row * n_cols_ + col);
          }
        }
        return *this;
      }
      throw MatrixException("values must have the same size as the matrix");
    }

    Matrix_T &operator=(const Matrix_T &cp)
    {
      std::size_t data_size = cp.n_rows_ * cp.n_cols_;
      data_ptr_ = new T[data_size];
      std::copy(cp.data_ptr_, cp.data_ptr_ + data_size, data_ptr_);
      n_cols_ = cp.n_cols_;
      n_rows_ = cp.n_rows_;
      return *this;
    }

    Matrix_T &operator=(Matrix_T &&mv) noexcept
    {
      data_ptr_ = mv.data_ptr_;
      mv.data_ptr_ = nullptr;

      n_cols_ = mv.n_cols_;
      mv.n_cols_ = 0;

      n_rows_ = mv.n_rows_;
      mv.n_rows_ = 0;
      return *this;
    }

  private:
    T *data_ptr_;
    std::size_t n_rows_;
    std::size_t n_cols_;
  };

  template <typename T> using Matrix = Matrix_T<T, MatrixTraits<T>>;
} // namespace wt

#endif // FOURIER_MATRIX_H
