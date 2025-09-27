#ifndef WT_ANALYSIS_H
#define WT_ANALYSIS_H

#include <array>
#include <complex>
#include <functional>
#include <memory>
#include <optional>

namespace wt::analysis
{
    // Assume that our window size in frames is
    // 1024 per channel, 2 channels
    constexpr int WINDOW_SIZE  = 2048;

    /**
     * @brief TODO
     */
    template <typename T, std::size_t N = WINDOW_SIZE>
    using processor_func = std::function<void(std::array<T, N>&)>;
    
    class FftAnalyzer {
    public:

        FftAnalyzer();
        ~FftAnalyzer();

        /**
         * @brief Will actually analyse the data. If post & pre
         * processing functions are set, we will also involve tose
         * in the pipeline.
         * @param input the input buffer with the window data.
         * @return the buffer of complex coefficients for the analysed
         * data.
         */
        std::array<std::complex<float>, (WINDOW_SIZE / 2) + 1> analyze(std::array<float, WINDOW_SIZE> const& input);

        /**
         * @brief sets the function for pre-processing the data
         * before analysis. This function should receive a buffer
         * for the window data and able to modify it in place.
         * @param func the processor function, see `processor_func`.
         */
        void set_preprocessor(processor_func<float> func);

        /**
         * @brief sets the function for post-processting the data
         * after analysis. This function should receive a buffer
         * of the analysed data, i.e. complex coefficients, and
         * should modify the data inplace.
         * @param fun the processor function, see `processor_func`.
         */
        void set_postprocessor(processor_func<std::complex<float>, (WINDOW_SIZE / 2) + 1> function);
    
    private:
        std::optional<processor_func<std::complex<float>, (WINDOW_SIZE / 2) + 1>> _post_processor;
        std::optional<processor_func<float>> _pre_processor;
        
        void* _kiss_cfg;
    };
} // namespace

#endif // WT_ANALYSIS_H
