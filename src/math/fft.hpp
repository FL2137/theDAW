#ifndef FFT_HPP
#define FFT_HPP

#include <array>
#include <cstdint>
#include <complex.h>

namespace core {
    namespace math {
        
        const double PI = 3.14159265358979323846;

        template <typename T, size_t N>
        class FFT {
        public:

            FFT() = default;
            ~FFT() = default;

            void process(std::vector<std::complex<T>>& grain) {
                radix2CooleyTukey(grain, grain.size());
            }
            
            void process(std::array<std::complex<T>, N>& grain) {
                radix2CooleyTukey<N>(grain);
            }

        private:
            void radix2CooleyTukey(std::vector<std::complex<T>>& grain, int n) {
                if (n == 1) {
                    return;
                }

                std::vector<std::complex<T>> split1 = {};
                split1.resize(n / 2);

                std::vector<std::complex<T>> split2 = {};
                split2.resize(n / 2);
                
                for (int idx = 0; idx < n; ++idx) {
                    if (idx % 2 == 0) {
                        split1[idx / 2] = grain[idx];
                    }
                    else {
                        split2[idx / 2] = grain[idx];
                    }
                }

                radix2CooleyTukey(split1, n / 2);
                radix2CooleyTukey(split2, n / 2);

                T ang = (-2 * PI) / n;
                std::complex<T> w{1};
                std::complex<T> wn{cos(ang), sin(ang)};

                for (int i = 0; i < n / 2; i++) {
                    grain[i] = split1[i] + w *split2[i];
                    grain[i + n/2] = split1[i] - w * split2[i];
                    w *= wn;
                } 
            }


            template<size_t M>
            void radix2CooleyTukey(std::array<T, M>& grain) {
                if (M == 1) {
                    return;
                }

                std::vector<std::complex<T>> split1 = {};
                split1.resize(M / 2);

                std::vector<std::complex<T>> split2 = {};
                split2.resize(M / 2);
                
                for (int idx = 0; idx < M; ++idx) {
                    if (idx % 2 == 0) {
                        split1[idx / 2] = grain[idx];
                    }
                    else {
                        split2[idx / 2] = grain[idx];
                    }
                }

                radix2CooleyTukey<M / 2>(split1);
                radix2CooleyTukey<M / 2>(split2);

                T ang = (-2 * PI) / M;
                std::complex<T> w{1};
                std::complex<T> wn{cos(ang), sin(ang)};

                for (int i = 0; i < M / 2; i++) {
                    grain[i] = split1[i] + w *split2[i];
                    grain[i + M/2] = split1[i] - w * split2[i];
                    w *= wn;
                } 
            } 

            void radix2CooleyTukey(std::complex<T>* grain, int n) {
                if (n == 1) {
                    return;
                }

                std::complex<T>* split1 = new std::complex<T>[n / 2];
                std::complex<T>* split2 = new std::complex<T>[n / 2];
                for (int idx = 0; idx < n; ++idx) {
                    if (idx % 2 == 0) {
                        split1.push_back(grain[idx]);
                    }
                    else {
                        split2.push_back(grain[idx]);
                    }
                }
                radix2CooleyTukey(split1, n / 2);
                radix2CooleyTukey(split2, n / 2);

                T ang = -2 * PI / n;
                std::complex<T> w{1};
                std::complex<T> wn{cos(ang), sin(ang)};

                for (int i = 0; n > i*2; i++) {
                    auto twiddle = w * split2[i];
                    grain[i] = split1[i] + twiddle;
                    grain[i + n/2] = split1[i] - twiddle;
                    w *= wn;
                }

                delete[] split1;
                delete[] split2;
            }

            void radix3CooleyTukey(std::vector<std::complex<T>>& grain, int n) {
                
                if (n == 1) {
                    return;
                }

                std::vector<std::complex<T>> split1;
                std::vector<std::complex<T>> split2;
                std::vector<std::complex<T>> split3;

                for (int idx = 0; idx < n; ++idx) {
                    if (idx % 3 == 0) {
                        split1.push_back(grain[idx]);
                    }
                    else if (idx % 3 == 1) {
                        split1.push_back(grain[idx]);
                    }
                    else if (idx % 3 == 2) {
                        split1.push_back(grain[idx]);
                    }
                }

                radix3CooleyTukey(split1, n / 3);
                radix3CooleyTukey(split2, n / 3);
                radix3CooleyTukey(split3, n / 3);


                const T eulerIdentity = (2 * PI) / n;
                std::complex<T> w{1};
                const std::complex<T> wn{cos(eulerIdentity), sin(eulerIdentity)};

                for (int i = 0; (i * 3) < n; ++i) {
                    grain[i] = split1[i] + w *split2[i];
                    grain[i + n/3] = split1[i] - w * split2[i];
                    w *= wn;
                }
            }

        private:

        };



        template<size_t N>
        struct DftLookupTable {
            
            DftLookupTable() {

            }
            int values[N][N];
        };

        template<typename T, size_t N>
        class DFT {
        public:

            using DataGrain = std::array<std::complex<T>, N>;

            DFT() : x(2 * PI / N), wn(cos(x), sin(x)) {
            }

            void process(const DataGrain& dataIn, DataGrain& dftOut) {
                int k = 0;

                for (k = 0; k < N; k++) {
                    dftOut[k] = 0;
                    for (int n = 0; n < N; ++n) {
                        dftOut[k] += dataIn[n] * pow(wn, (-1 * k * n));
                    }
                    std::cout << std::endl;
                }
            }

        private:
        
            constexpr T** prepareOmegaLookup() {
                const std::complex<T> Wn = {cos(2 * core::math::PI / N), sin(2 * core::math::PI / N)};
                for (int i = 0; i < N; ++i) {
                    for (int j = 0; j < N; ++j) {
                        
                    }
                }
            }


        private:
            const std::complex<T> wn;
            const T x;
        };

    }
} 

#endif // FFT_HPP