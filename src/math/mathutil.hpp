#ifndef MATHUTIL_HPP
#define MATHUTIL_HPP

#include <array>
#include <math.h>
#include <QtQuick/QSGNode>

namespace core {
namespace math {

#define PI 3.14159265358979323846

template<typename T, size_t N>
void normalizeSampleData(const std::array<T, N>& data, std::array<float, N>& out, int32_t& outmax, int32_t& outmin) {

    int32_t max = INT32_MIN;
    int32_t min = INT32_MAX;

    for (uint32_t i = 0; i < N; ++i) {
        if (data[i] > max) {
            max = data[i];
        }

        if (data[i] < min) {
            min = data[i];
        }
    }

    int32_t divisor = max - min;

    outmax = max;
    outmin = min;

    for (int i = 0; i < N; ++i) {
        out[i] = static_cast<float>((data[i] - min)) / divisor;
    }
}

template<typename T, size_t N>
void normalizeSampleData(T data[N], float out[N], int32_t& outmax, int32_t& outmin) {
    int32_t max = INT32_MIN;
    int32_t min = INT32_MAX;

    for (uint32_t i = 0; i < N; ++i) {
        if (data[i] > max) {
            max = data[i];
        }

        if (data[i] < min) {
            min = data[i];
        }
    }

    int32_t divisor = max - min;

    outmax = max;
    outmin = min;

    for (int i = 0; i < N; ++i) {
        out[i] = static_cast<float>((data[i] - min)) / divisor;
    }
}

template<typename T, size_t N>
void denormalizeSample(std::array<T, N>& data, int32_t max, int32_t min) {
    int32_t mult = max - min;
    for (uint32_t i = 0; i < N; ++i) {
        data[i] = data[i] * mult + min;
    }
}
template<typename T, size_t N>
void denormalizeSample(float data[N], T in[N], int32_t max, int32_t min) {
    int32_t mult = max - min;
    for (uint32_t i = 0; i < N; ++i) {
        in[i] = std::floor(data[i] * mult + min);
    }
}

inline void generateSinWave(double* data,  uint32_t cycles, uint32_t resolution = 48000) {
    
    double waveLength = PI * cycles * 2;
    double step = waveLength / static_cast<double>(resolution);

    double x = 0.0f;
    
    for (uint32_t i = 0; i < resolution; i++) {
        data[i] = std::sin(x);
        if (i < 5) {
            std::cout << "x:" << x << std::endl;
        }
        x += step;
    }
}

inline void generateSineWavePoints(QPointF* points, uint32_t cycles, int32_t startOffset = 0, uint32_t resolution = 48000) {
    double waveLength = PI * cycles * 2;
    double step = waveLength / static_cast<double>(resolution);

    double x = startOffset;

    for (uint32_t i = 0; i
         < resolution; i++) {
        points[i] = QPointF(x, std::sinf(x));
        x += step;
    }
    
}


}
}

#endif // MATHUTIL_HPP