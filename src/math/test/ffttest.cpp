#include <gtest/gtest.h>

#include <vector>
#include <complex>
#include <cstdint>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <chrono>

#include "../fft.hpp"

class FFTTests : public testing::Test {
protected:
    FFTTests() {}

    void SetUp() override {}
    void TearDown() override {}

    const std::string DATA_FOLDER_PATH = std::string("C:\\Users\\frane\\Desktop\\wasapiwrap\\src\\math\\test\\") + std::string("\\data");
};

double benchmark(std::function<void()> foo) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto t1 = high_resolution_clock::now();
    foo();
    auto t2 = high_resolution_clock::now();

    duration<double, std::milli> ms_double = t2 - t1;

    return ms_double.count();
}


TEST_F(FFTTests, radix2CooleyTukey128) {

    std::vector<std::complex<double>> data = {};
    std::vector<std::complex<double>> fftedData = {};

    std::fstream dataFile(DATA_FOLDER_PATH + "\\fft_test_128.txt");

    std::string line;
    int i = 0;
    while(std::getline(dataFile, line)) {
        if (i < 128) {
            std::complex<double> cc{std::stod(line), 0};
            data.push_back(cc);
        }
        else if (i > 128) {
            std::complex<double> cc;
            
            size_t minusIndex = line.find_last_of('-');
            if (minusIndex > 2 && minusIndex < 100) {
                cc.real(std::stod(line.substr(0, minusIndex)));
                cc.imag(std::stod(line.substr(minusIndex, line.size() - minusIndex)));
            }
            else {
                size_t plusIndex = line.find_last_of('+');
                cc.real(std::stod(line.substr(0, plusIndex)));
                cc.imag(std::stod(line.substr(plusIndex, line.size() - plusIndex)));
            }

            fftedData.push_back(cc);
        }
        i++;
    }
    dataFile.close();

    core::math::FFT<double, 128> fft;

    double time = benchmark([&](){
        fft.process(data);
    });

    std::cout << "[128FFT(std::vector)] processing time: " << time << std::endl;

    for (uint32_t idx = 0u; idx < data.size(); ++idx) {
        EXPECT_NEAR(std::abs(data[idx]), std::abs(fftedData[idx]), 0.01);
    }
}


TEST_F(FFTTests, radix2CooleyTukey128Array) {
    std::vector<std::complex<double>> data = {};
    std::vector<std::complex<double>> fftedData = {};

    std::fstream dataFile(DATA_FOLDER_PATH + "\\fft_test_128.txt");

    std::string line;
    int i = 0;
    while(std::getline(dataFile, line)) {
        if (i < 128) {
            std::complex<double> cc{std::stod(line), 0};
            data.push_back(cc);
        }
        else if (i > 128) {
            std::complex<double> cc;
            
            size_t minusIndex = line.find_last_of('-');
            if (minusIndex > 2 && minusIndex < 100) {
                cc.real(std::stod(line.substr(0, minusIndex)));
                cc.imag(std::stod(line.substr(minusIndex, line.size() - minusIndex)));
            }
            else {
                size_t plusIndex = line.find_last_of('+');
                cc.real(std::stod(line.substr(0, plusIndex)));
                cc.imag(std::stod(line.substr(plusIndex, line.size() - plusIndex)));
            }

            fftedData.push_back(cc);
        }
        i++;
    }
    dataFile.close();

    core::math::FFT<double, 128> fft;

    double time = benchmark([&](){
        fft.process(data);
    });

    std::cout << "[128FFT(std::array)] processing time: " << time << std::endl;

    for (uint32_t idx = 0u; idx < data.size(); ++idx) {
        EXPECT_NEAR(data[idx].imag(), fftedData[idx].imag(), 0.01);
        EXPECT_NEAR(data[idx].real(), fftedData[idx].real(), 0.01);
    }
}

TEST_F(FFTTests, radix2CooleyTukey48000PaddedArray) {
    std::array<std::complex<double>, 32767 * 2> data = {};
    std::array<std::complex<double>, 32767 * 2> fftedData = {};
    std::cout << "LOG1\n";
    std::fstream dataFile(DATA_FOLDER_PATH + "\\fft_test_48000.txt");

    std::string line;
    int i = 0;
    int dataIdx = 0;
    int ffdtedDataIdx = 0;
    while(std::getline(dataFile, line)) {
        if (i < 48001) {
            std::complex<double> cc{std::stod(line), 0};
            data[dataIdx++] = cc;
        }
        else if (i > 48001) {
            std::complex<double> cc;
            
            size_t minusIndex = line.find_last_of('-');
            if (minusIndex > 2 && minusIndex < 100) {
                cc.real(std::stod(line.substr(0, minusIndex)));
                cc.imag(std::stod(line.substr(minusIndex, line.size() - minusIndex)));
            }
            else {
                size_t plusIndex = line.find_last_of('+');
                cc.real(std::stod(line.substr(0, plusIndex)));
                cc.imag(std::stod(line.substr(plusIndex, line.size() - plusIndex)));
            }
            fftedData[ffdtedDataIdx++] = cc;
        }
        i++;
    }
    dataFile.close();

}