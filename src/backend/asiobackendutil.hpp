#ifndef ASIOBACKENDUTIL_HPP
#define ASIOBACKENDUTIL_HPP

#include <asiosys.h>
#include <iostream>
#include <vector>
#include <string>
#include "asio.h"
#include "asiodrivers.h"
#include <functional>
#include <random>
#include <chrono>

namespace asiobackend {

    #if NATIVE_INT64
	#define ASIO64toDouble(a)  (a)
    #else
        const double twoRaisedTo32 = 4294967296.;
        #define ASIO64toDouble(a)  ((a).lo + (a).hi * twoRaisedTo32)
    #endif
    
    template<typename T, bool REAL_DIST> // implement REAL_DIST
    void getRandomSampleData(std::vector<T>& outputData, int dataSize) {
       
        std::mt19937 rng;
        std::uniform_int_distribution<uint32_t> seedDist(0, 2137);
        std::random_device rd;
        uint32_t seed = seedDist(rd);
        rng.seed(seed);
        
        if (REAL_DIST) {
            std::uniform_real_distribution<double> sampleDist(0, (std::numeric_limits<uint16_t>::max)());
            if (outputData.size() != dataSize) {
                outputData.resize(dataSize);
            }
    
            for (int sampleIdx = 0; sampleIdx < dataSize; ++sampleIdx) {
                outputData[sampleIdx] = sampleDist(rng);
            }
        }
        else {
            std::uniform_int_distribution<uint16_t> sampleDist(0, (std::numeric_limits<uint16_t>::max)());
    
            if (outputData.size() != dataSize) {
                outputData.resize(dataSize);
            }
    
            for (int sampleIdx = 0; sampleIdx < dataSize; ++sampleIdx) {
                outputData[sampleIdx] = sampleDist(rng);
            }
        }
    }

    template <typename Func, typename ...Args>
    double functionBenchmark(Func f) {
        using std::chrono::high_resolution_clock;
        using std::chrono::duration_cast;
        using std::chrono::duration;
        using std::chrono::milliseconds;

        auto t1 = high_resolution_clock::now();
        f();
        auto t2 = high_resolution_clock::now();

        duration<double, std::milli> ms_double = t2 - t1;

        return ms_double.count();
    }

    constexpr uint8_t MAX_INPUT_CHANNELS = 32;
    constexpr uint8_t MAX_OUTPUT_CHANNELS = 32;

    struct DriverInfo {
        ASIODriverInfo asioDriverInfo;

        long outputChannels;
        long inputChannels;

        long minBufferSize;
        long maxBufferSize;
        long prefferedBufferSize;
        long granularity;

        ASIOSampleRate sampleRate;

        bool postOutput;

        long inputLatency;
        long outputLatency;

        long inputBuffers;
        long outputBuffers;

        ASIOBufferInfo bufferInfos[MAX_INPUT_CHANNELS + MAX_OUTPUT_CHANNELS];

        ASIOChannelInfo channelInfos[MAX_INPUT_CHANNELS + MAX_OUTPUT_CHANNELS];

        double nanoSeconds;
        double samples;
        double timeCodeSamples;

        ASIOTime timeInfo;
        long sysRefTime;

        bool stopped;
    };

    struct BackendInfo {
        int sampleSize;
        int outLatency;
        int inLatency;
        int bufferSize;
    };

    inline static void LOG_ERROR(ASIOError error) {
        std::cout << "[" << __FUNCTION__  << "] ";
        switch (error) {
            case ASE_NotPresent:
                std::cout << "hardware input or output is not present or available" << std::endl;
                break;
            case ASE_HWMalfunction:
                std::cout << "hardware is malfunctioning (can be returned by any ASIO function)" << std::endl;
                break;
            case ASE_InvalidParameter:
                std::cout << "input parameter invalid" << std::endl;
                break;
            case ASE_InvalidMode:
                std::cout << "hardware is in a bad mode or used in a bad mode" << std::endl;
                break;
            case ASE_SPNotAdvancing:
                std::cout << "hardware is not running when sample position is inquired" << std::endl;
                break;
            case ASE_NoClock:
                std::cout << "sample clock or rate cannot be determined or is not present" << std::endl;
                break;
            case ASE_NoMemory:
                std::cout << "not enough memory for completing the request" << std::endl;
                break;
            case ASE_OK:
            default:
                return;
        }
    }

}

#endif // ASIOBACKENDUTIL_HPP