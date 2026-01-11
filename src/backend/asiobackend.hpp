#ifndef ASIOBACKEND_HPP
#define ASIOBACKEND_HPP

#include "asiobackendutil.hpp"
#include "spscqueue.hpp"

#include "Effect.hpp"
#include <functional>
#include <vector>
#include <memory>

#include <QThread>
#include <QRunnable>
#include <QWaitCondition>
#include <QMutex>

#include <QSettings>
#include <QObject>


#define SPSC_QUEUE_DEPTH 32

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

    using EffectBase = digitaleffects::EffectBase;
    using EffectID = int;

    AsioBackend(QMutex& mutex, QWaitCondition& waitCondition, SpscQueue<EffectBase*, SPSC_QUEUE_DEPTH>* effectQueue) : mut(mutex), waitCon(waitCondition)
    {
    }

    ~AsioBackend();
    
    static DriverInfo driverInfo;
    AsioDrivers asioDrivers{};

    inline static float pregain = 1.0f;

private:

    bool manualDriverSelection();

    void enumerateAudioDevices(std::vector<std::string>& driverNames);

    void initialize();

    ASIOCallbacks m_callbacks;

    static const size_t INPUT_CHANNEL = 1;
    static const size_t OUTPUT_CHANNEL_R = 2;
    static const size_t OUTPUT_CHANNEL_L = 3;

    static constexpr size_t SAMPLE_RATES_COUNT = 6;
    static constexpr size_t BUFFER_SIZES_COUNT = 11;
    
    const std::array<uint32_t, SAMPLE_RATES_COUNT> SAMPLE_RATES = {
        44100, 48000, 88200, 96000, 176400, 192000
    };
    
    const std::array<size_t, BUFFER_SIZES_COUNT> BUFFER_SIZES = {
        16, 32, 48, 64, 128, 160, 192, 256, 512, 1024
    };

    QSettings settings;
    QMutex& mut;
    QWaitCondition& waitCon;

    void processingFunction(int32_t* buffer, int size);

    std::vector<EffectBase*> processingList;

    void setupEffects();

    SpscQueue<EffectBase*, SPSC_QUEUE_DEPTH>* m_effectQueue{nullptr};

public slots:
    void run();
    
signals:
    void backendReady(asiobackend::BackendInfo info);
    void backendChooseAudioDriver(const std::vector<std::string>, std::string&);
};


}

#endif // ASIOBACKEND_HPP