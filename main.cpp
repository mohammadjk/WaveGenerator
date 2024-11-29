/**
 * Author:  Mohammad Khodsetan
 * Date:    28-11-2024
 * 
 * Task:
 * Write a function in c++ which creates an audio wave file with pure sinusoidal sound.
 * - Both C or C++ code acceptable.
 * - Both platform-specific code (Windows, Linux, MacOS) or cross-platform code acceptable.
 * 
 * Input: 	
 * - sound frequency in Hertzs and a length of the file in seconds
 * 
 * Output:
 * - audio.wav file 24 bit, mono, 48kHz in current directory.
 * - an exception with an explanation if there was an error anywhere.
 */
#include <array>
#include <vector>
#include <cstring>
#include <fstream>
#include <iostream>

#include "WaveHeader.h"
#include "SineWaveGen.h"

// Helper constants
constexpr uint32_t  k_amplitude {300'000};   // The amplitude of the sine wave to generate 
constexpr uint16_t  k_frequency {1'000};     // Frequency of the sine wave to generate (Unit: Hz)

/**
 * Generates the header for a Wave file. 
 */
auto create_wave_header(uint32_t sample_rate, uint32_t file_length_sec, uint16_t bits_per_sample) noexcept
{
    wave::WaveHeader hdr{};

    std::memcpy(&hdr.file_type_bloc_id, "RIFF", 4);
    std::memcpy(&hdr.file_format_id, "WAVE", 4);
    std::memcpy(&hdr.format_bloc_id, "fmt ", 4);
    std::memcpy(&hdr.data_bloc_id, "data", 4);

    hdr.bloc_size = wave::k_header_bloc_size;
    hdr.audio_format = wave::k_audio_format_PCM;
    hdr.channel_count = wave::k_channel_count_mono;
    hdr.sample_rate = sample_rate;
    hdr.bits_per_sample = bits_per_sample;
    hdr.bytes_per_bloc = (hdr.channel_count * hdr.bits_per_sample / 8);
    hdr.bytes_per_sec = (hdr.sample_rate * hdr.bytes_per_bloc);
    hdr.data_size = file_length_sec * hdr.bytes_per_sec;
    hdr.file_size = wave::k_header_size + hdr.data_size - 8;

    std::array<uint8_t, wave::k_header_size> header;
    std::memcpy(header.data(), &hdr, wave::k_header_size);

    return header;
}

/**
 * Generates data for a Wave file.
 */
auto create_wave_data(uint32_t sample_rate, uint32_t file_length_sec)
{    
    std::vector<uint8_t> samples;

    wavegen::sine_wave_generator  audio_source(k_amplitude, k_frequency, sample_rate);
    uint32_t total_sample_count = sample_rate * file_length_sec;
    for (uint32_t sample_index{}; sample_index < total_sample_count; ++sample_index) 
    {        
        auto sample = audio_source.get_sample(sample_index);

        samples.push_back(sample & 0xFF);
        samples.push_back((sample >> 8) & 0xFF);
        samples.push_back((sample >> 16) & 0xFF);
    }

    return samples;
}

/**
 * Writes header and audio data to a given file
 */
void write_to_file(const uint8_t* hdr, uint32_t hdr_size, const uint8_t* data, uint32_t data_size, const std::string& file_path)
{
    std::ofstream file(file_path, std::fstream::binary);

    if (!file.is_open()) {
        throw std::ofstream::failure("Failed to open file " + file_path);
    }

    try {
        if(file.write((const char*)hdr, hdr_size).fail()) {
            throw std::ofstream::failure("Failed to write header data to file.");
        }

        if (file.write((const char*)data, data_size).fail()) {
            throw std::ios_base::failure("Failed to write audio data to file.");
        }
    } catch (...) {
        file.close(); // ensure file is closed on exception
        throw;
    }

    file.close();
}


void CreateWaveFile(uint32_t sample_rate, uint32_t file_length_sec, uint16_t bits_per_sample, const std::string& file_path)
{
    try {

        auto header  = create_wave_header(sample_rate, file_length_sec, bits_per_sample);
        auto samples = create_wave_data(sample_rate, file_length_sec);

        write_to_file(header.data(), header.size(), samples.data(), samples.size(), file_path);

    } catch (const std::exception& e) {
        std::cout << "File generation failed. Error: \"" << e.what() << "\"\n";
    }
}

int main()
{
    static_assert(noexcept(write_to_file), "foo should be noexcept!");

    uint32_t sample_rate {48'000}; // 48kHz
    uint32_t file_length_sec {5};  // 5sec
    uint16_t bits_per_sample {24}; // 24bit
    std::string file_path {"audio.wav"};

    std::cout << "Generating a wave file...\n";
       
    CreateWaveFile(sample_rate, file_length_sec, bits_per_sample, file_path);

    std::cout << "Finished.\n";

    return 0;
}