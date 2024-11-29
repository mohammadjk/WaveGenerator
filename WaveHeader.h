/**
 * Author:  Mohammad Khdosetan
 * Date:    29-11-2024
 * Desc.:   Provides types and definitons for a WAVE audio header.
 */

#ifndef WAVE_HEADER_H_
#define WAVE_HEADER_H_

#include <cstdint>

namespace wave
{

// WAV file header (44 bytes).
struct WaveHeader
{
    // RIFF header
    uint8_t  file_type_bloc_id[4];  // Identifier "RIFF"
    uint32_t file_size;             // Overall file size minus 8 bytes
    uint8_t  file_format_id[4];     // Format "WAVE"

    // Chunk describing the data format
    uint8_t format_bloc_id[4];      // Identifier "fmt". Marks the start of the "format" chunk
    uint32_t bloc_size;             // Size of the format chunk (16 bytes)
    uint16_t audio_format;          // Audio format (1: PCM integer, 3: IEEE 754 float)
    uint16_t channel_count;         // Number of channels (1: mono, 2: stereo)
    uint32_t sample_rate;           // Sample Rate (Samples per second in hertz)
    uint32_t bytes_per_sec;         // Number of bytes to read per second (sample_rate * bytes_per_bloc)
    uint16_t bytes_per_bloc;        // Number of bytes per block (channel_count * bits_per_sample / 8)
    uint16_t bits_per_sample;       // Number of bits per sample

    // Chunk containing the sampled data
    uint8_t data_bloc_id[4];        // Identifier "data". Marks the start of the audio data chunk
    uint32_t data_size;             // Size of the audio data in bytes
};

constexpr uint16_t k_header_size = sizeof(WaveHeader)/sizeof(char);

constexpr uint32_t k_header_bloc_size = 0x10; // 16 bytes

constexpr uint16_t k_channel_count_mono = 0x01; // Mono
constexpr uint16_t k_channel_count_stereo = 0x02; // Stereo

constexpr uint16_t k_audio_format_PCM = 0x01; // PCM
constexpr uint16_t k_audio_format_IEEE_754 = 0x03; // IEEE 754

}// namespace wave

#endif // WAVE_HEADER_H_