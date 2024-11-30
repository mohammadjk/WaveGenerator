/**
 * Author:  Mohammad Khodsetan
 * Date:    29-11-2024
 * Desc.:   A sample sinusoidal sound generator.
 */
#ifndef SINE_WAVE_GEN_H_
#define SINE_WAVE_GEN_H_

#include <cstdint>
#include <cmath>

namespace wavegen
{

constexpr double    k_PI {3.141592653589793};

/** 
 * sinusoidal sound generator 
 **/
class sine_wave_generator
{
public:
    sine_wave_generator(uint32_t wave_amplitude, uint32_t wave_frequency, uint32_t sample_rate) 
        : m_amplitude(wave_amplitude)
        , m_frequency(wave_frequency)
        , m_sample_rate(sample_rate) 
    {
    }

    int32_t get_sample(uint32_t sample_index) {
        double time_increment = 1.0/m_sample_rate;
        auto sample_time = sample_index * time_increment;
        return static_cast<int32_t>(m_amplitude * std::sin(2.0 * k_PI  * m_frequency * sample_time));
    }

private:
    uint32_t m_amplitude;
    uint32_t m_frequency;
    uint32_t m_sample_rate;
};

}
#endif // SINE_WAVE_GEN_H_