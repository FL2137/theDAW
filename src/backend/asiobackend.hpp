#ifndef ASIOBACKEND_HPP
#define ASIOBACKEND_HPP

#include "asiobackendutil.hpp"
#include "Effect.hpp"
#include <functional>
#include <vector>

#include <QThread>
#include <QRunnable>
#include <QObject>

namespace asiobackend {


template<typename T, size_t N>
class Processing : public QRunnable {
public:

    using EffectList = std::vector<digitaleffects::EffectBase*>;

    Processing(T* inBuffers, T* outBufferL, T* outBufferR, int bufferSize, EffectList processingList) {
        inbuff = inBuffers;
        outbuffL = outBufferL;
        outbuffR = outBufferR;
        this->bufferSize = bufferSize;
        m_processingList = processingList;
    }

    void run() {
        for (int sampleIdx = 0; sampleIdx < bufferSize; ++sampleIdx) {
            for (digitaleffects::EffectBase* effect : m_processingList) {
                effect->process();
            }
        }
    }

    ~Processing() override {}

private:
    std::vector<digitaleffects::EffectBase*> m_processingList;
    T* inbuff = nullptr;
    T* outbuffL = nullptr;
    T* outbuffR = nullptr;
    int bufferSize = 0;
};



class AsioBackend : public QObject {

    Q_OBJECT

public:

    using EffectDfx = digitaleffects::EffectBase;
    using EffectID = int;

    AsioBackend() = default;

    
    ~AsioBackend();
    
    void addEffect(EffectDfx* effect);
    void removeEffect(EffectID id);
    
    static DriverInfo driverInfo;

private:

    void manualDriverSelection(std::string &driverName);
    void initialize();

    AsioDrivers m_drivers;
    ASIOCallbacks m_callbacks;

    std::vector<digitaleffects::EffectBase*> processingList;

    static constexpr size_t SAMPLE_RATES_COUNT = 6;
    static constexpr size_t BUFFER_SIZES_COUNT = 11;
    
    const std::array<uint32_t, SAMPLE_RATES_COUNT> SAMPLE_RATES = {
        44100, 48000, 88200, 96000, 176400, 192000
    };

    
    const std::array<uint32_t, BUFFER_SIZES_COUNT> BUFFER_SIZES = {
        16,32, 48, 64, 128, 160, 192, 256, 512, 1024
    };

public slots:
    void run();
    
signals:
    void runProcessing();
    void backendReady(asiobackend::BackendInfo info);

};
}


#endif // ASIOBACKEND_HPP