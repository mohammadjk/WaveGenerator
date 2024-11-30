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
#include <string>
#include <vector>
#include <cstring>
#include <fstream>
#include <iostream>

#include "WaveHeader.h"
#include "SineWaveGen.h"

// Helper constants
constexpr uint32_t  k_amplitude {30'000'000};   // The amplitude of the sine wave to generate 

constexpr uint32_t  k_sample_rate {48'000};  // 48kHz sample rate
constexpr uint16_t  k_bits_per_sample {24};  // 24 bits per sample

/**
 * Generates the header for a Wave file. 
 */
auto create_wave_header(double file_length_sec)
{
    wave::WaveHeader hdr{};

    std::memcpy(&hdr.file_type_bloc_id, "RIFF", 4);
    std::memcpy(&hdr.file_format_id, "WAVE", 4);
    std::memcpy(&hdr.format_bloc_id, "fmt ", 4);
    std::memcpy(&hdr.data_bloc_id, "data", 4);

    hdr.bloc_size = wave::k_header_bloc_size;
    hdr.audio_format = wave::k_audio_format_PCM;
    hdr.channel_count = wave::k_channel_count_mono;
    hdr.sample_rate = k_sample_rate;
    hdr.bits_per_sample = k_bits_per_sample;
    hdr.bytes_per_bloc = (hdr.channel_count * hdr.bits_per_sample / 8);
    hdr.bytes_per_sec = (hdr.sample_rate * hdr.bytes_per_bloc);

    uint64_t data_size = static_cast<uint64_t>(file_length_sec * hdr.bytes_per_sec);
    if (data_size > UINT32_MAX) {
        throw std::overflow_error("File generation failed. Data size exceeds the maximum limit.");
    }

    hdr.data_size = static_cast<uint32_t>(data_size);
    hdr.file_size = wave::k_header_size + hdr.data_size - 8;

    std::array<uint8_t, wave::k_header_size> header;
    std::memcpy(header.data(), &hdr, wave::k_header_size);

    return header;
}

/**
 * Generates data for a Wave file.
 */
auto create_wave_data(uint32_t wave_frequency, double file_length_sec)
{    
    std::vector<uint8_t> samples;

    wavegen::sine_wave_generator  audio_source(k_amplitude, wave_frequency, k_sample_rate);
    uint32_t total_sample_count = k_sample_rate * file_length_sec;
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
        throw std::ofstream::failure("File generation failed. Failed to open file " + file_path);
    }

    try {
        if(file.write((const char*)hdr, hdr_size).fail()) {
            throw std::ofstream::failure("File generation failed. Failed to write header data to file.");
        }

        if (file.write((const char*)data, data_size).fail()) {
            throw std::ofstream::failure("File generation failed. Failed to write audio data to file.");
        }
    } catch (...) {
        file.close(); // ensure file is closed on exception
        throw;
    }

    file.close();
}


void create_wave_file(uint32_t wave_frequency, double file_length_sec)
{
    if (file_length_sec <= 0.0) {
        throw std::invalid_argument("Invalid argument. File length should be greater than 0.");
    }

    if (wave_frequency > k_sample_rate/2) {
        throw std::invalid_argument("Invalid argument. Wave frequency should be less than or equal to half of the sample rate.");
    }

    auto header  = create_wave_header(file_length_sec);
    auto samples = create_wave_data(wave_frequency, file_length_sec);

    std::string file_path {"audio.wav"};
    write_to_file(header.data(), header.size(), samples.data(), samples.size(), file_path);
}

void parse_args(int argc, char* argv[], uint32_t& frequency, double& file_length)
{
    if (argc < 3) {
        throw std::invalid_argument("Invalid arguments. Usage: " + std::string(argv[0]) + " <wave_frequency> <file_length_sec>");
    }

    try {
        frequency = std::stoul(argv[1]);
        file_length = std::stod(argv[2]);
    } catch (const std::exception& e) {
        throw std::invalid_argument("Invalid arguments. Enter valid numbers for wave frequency and file length.");
    }
}      

int main(int argc, char* argv[])
{
    try {
        uint32_t frequency{};
        double file_length{};
        parse_args(argc, argv, frequency, file_length);

        std::cout << "Generating a wave file with wave frequency " << frequency 
                  << "Hz and file length " << file_length << " seconds...\n";
        
        create_wave_file(frequency, file_length);

    } catch (const std::exception& e) {
        std::cerr << "Error: \"" << e.what() << "\"\n";
        return -1;
    }

    std::cout << "Finished.\n";

    return 0;
}