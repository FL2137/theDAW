#ifndef DISTORTION_HPP
#define DISTORTION_HPP

#include <array>
#include <limits>
#include "Effect.hpp"

namespace digitaleffects {

class Distortion : public EffectBase {
public:

    template<typename T, size_t N>
    void process(std::array<T, N>& sample) {
        
        const float ONETHIRD = 1.f/3.f;
        const float TWOTHIRDS = 2.f/3.f;

        for (uint32_t idx = 0u; idx < N; ++idx) {
            if (sample[idx] < ONETHIRD) {
                sample[idx] = sample[idx] * 2; 
            }
            else if(sample[idx] >= ONETHIRD && sample[idx] <= TWOTHIRDS) {
                sample[idx] = (3 - ((2 - (3*sample[idx])) * (2 - (3*sample[idx])))) / 3;
            }
            else if (sample[idx] > TWOTHIRDS) {
                sample[idx] = 1;
            }
        }
    }

    template<typename T, size_t N>
    void process(T* sample) {

        const float ONETHIRD = 1.f/3.f;
        const float TWOTHIRDS = 2.f/3.f;

        for (uint32_t idx = 0u; idx < N; ++idx) {
            if (sample[idx] < ONETHIRD) {
                sample[idx] = sample[idx] * 2; 
            }
            else if(sample[idx] >= ONETHIRD && sample[idx] <= TWOTHIRDS) {
                sample[idx] = (3 - ((2 - (3*sample[idx])) * (2 - (3*sample[idx])))) / 3;
            }
            else if (sample[idx] > TWOTHIRDS) {
                sample[idx] = 1;
            }
        }
    }
};
}

#endif // DISTORTION_HPP