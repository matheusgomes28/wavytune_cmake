#ifndef WT_ANALYSIS_HANN_WINDOW_H
#define WT_ANALYSIS_HANN_WINDOW_H

#include <array>
#include <cmath>

namespace wt::analysis {

    template <std::size_t Size>
    std::array<float, Size> make_hann_coefficients() {
        static_assert(Size > 0, "hann coefficient array must have more than 0 items!");

        constexpr float TWO_PI = 6.28318530718;

        std::array<float, Size> ret{};
        
        for (std::size_t i = 0; i < Size; ++i) {
            ret[i] = 0.5f * (1 - std::cos((i * TWO_PI) / static_cast<float>(Size - 1)));
        }

        return ret;
    }
}

#endif // WT_ANALYSIS_HANN_WINDOW_H