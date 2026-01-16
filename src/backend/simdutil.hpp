#ifndef SIMDUTIL_HPP
#define SIMDUTIL_HPP

#include <immintrin.h>
#include <iostream>


namespace simdutil
{

static inline void loadSIMDBatch(float* data, size_t size)
{
    __m256 a = _mm256_set_ps(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    __m256 b = _mm256_set_ps(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);

    __m256 c = _mm256_add_ps(a, b);



}

void printSIMD(float* data, size_t size)
{
}





}

#endif // SIMDUTIL_HPP