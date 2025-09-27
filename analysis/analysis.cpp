#include <algorithm>
#include <kiss_fftr.h>

#include <analysis/analysis.hpp>


namespace wt::analysis {


    FftAnalyzer::FftAnalyzer() : _kiss_cfg{kiss_fftr_alloc(WINDOW_SIZE, 0, nullptr, nullptr)} {}

    FftAnalyzer::~FftAnalyzer() {
        free(static_cast<kiss_fftr_cfg>(_kiss_cfg));
    }

    auto FftAnalyzer::set_preprocessor(processor_func<float> func) -> void {
        _pre_processor = func;
    }

    auto FftAnalyzer::set_postprocessor(processor_func<std::complex<float>, (WINDOW_SIZE / 2) + 1> func) -> void {
        _post_processor = func;
    }

    auto FftAnalyzer::analyze(std::array<float, WINDOW_SIZE> const& input)
        -> std::array<std::complex<float>, (WINDOW_SIZE / 2) + 1> {

        std::array<float, WINDOW_SIZE> buffer = input;

        if (_pre_processor) {
            (*_pre_processor)(buffer);
        }

        // Run the actual fft
        constexpr auto OUTPUT_SIZE = (WINDOW_SIZE / 2) + 1;
        std::array<kiss_fft_cpx, OUTPUT_SIZE> kiss_output{};
        kiss_fftr(static_cast<kiss_fftr_cfg>(_kiss_cfg), buffer.data(), kiss_output.data());

        // Convert and return
        std::array<std::complex<float>, OUTPUT_SIZE> ret{};
        std::transform(begin(kiss_output), end(kiss_output), begin(ret),
            [](kiss_fft_cpx in) { return std::complex<float>{in.r, in.i}; });

        if (_post_processor) {
            (*_post_processor)(ret);
        }

        return ret;
    }
} // namespace wt::analysis
