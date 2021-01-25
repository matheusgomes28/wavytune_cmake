#ifndef FOURIER_SFT_H
#define FOURIER_SFT_H

// Includes from the std
#include <cstdlib>
#include <complex>
#include <deque>

// Includes from this project
#include "dft.h"


namespace wt
{
  namespace ft
  {
    /// Class for the slow fourier
    /// transform, inheriting from the
    /// DFT class
    /// @tparam T is the numeric type of the signal
    template <typename T>
    class SFT : public DFT<T>
    {
    public:

      Frequencies<T> back_transform() final
      {
        // TODO : Take care of empty queues
        Window<T> back = windows_.back();
        windows_.pop_back();
        return back;
      }

      Frequencies<T> front_transform() final
      {
        // Pop from the front
        Window<T> front = windows_.front();
        windows_.pop_front();
        return front; // TODO : proper transform this
      }

      void feed(const Window<T>& window) final
      {
        windows_.push_back(window);
      }

      void set_base_samples(std::size_t n)
      {
        base_samples_ = n;
      }

    private:
      std::size_t base_samples_;
      std::deque<Window<T>> windows_;
    };
  } // namespace ft
} // namespace wt

#endif // FOURIER_SFT_H
