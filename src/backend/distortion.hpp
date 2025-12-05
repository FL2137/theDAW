#ifndef DISTORTION_HPP
#define DISTORTION_HPP

#include <array>
#include <limits>
#include "Effect.hpp"

namespace digitaleffects {

template<typename T, size_t N>
class Distortion : public EffectBase {
public:

    using DataGrain = std::array<T, N>;

    void process(DataGrain& sample) override {
        
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

    void process(T* sample) override {

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

    void processNoNormalization(T* sample) {
        const float ONETHIRD = (std::numeric_limits<T>::max)() / 3.0f;
        const float TWOTHIRDS = (2.0f * (std::numeric_limits<T>::max)()) / 3.0f; 

        for (uint32_t idx = 0u; idx < N; ++idx) {
            if (sample[idx] < 0) {
                if (sample[idx] <= (-TWOTHIRDS)) {
                    sample[idx] = -(std::numeric_limits<T>::max)();
                }
                else if (sample[idx] >= (-TWOTHIRDS) && sample[idx] < (-ONETHIRD)) {
                    sample[idx] = (3 - ((2 - (3*sample[idx])) * (2 - (3*sample[idx])))) / 3;
                    if (sample[idx] > 0) {
                        sample[idx] = -sample[idx];
                    }
                }
                else if (sample[idx] >= (-ONETHIRD)) {
                    sample[idx] = 2 * sample[idx];
                }
            }
            else {
                if (sample[idx] < ONETHIRD) {
                    sample[idx] = sample[idx] * 2; 
                }
                else if(sample[idx] >= ONETHIRD && sample[idx] <= TWOTHIRDS) {
                    sample[idx] = (3 - ((2 - (3*sample[idx])) * (2 - (3*sample[idx])))) / 3;
                }
                else if (sample[idx] > TWOTHIRDS) {
                    sample[idx] = (std::numeric_limits<T>::max)();
                }
            }
        }
    }

    void swap() {

    }

private:


};
}

#endif // DISTORTION_HPP