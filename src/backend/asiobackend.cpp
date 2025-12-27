#include "asiobackendutil.hpp"
#include "asiobackend.hpp"

#include "distortion.hpp"
#include "../math/mathutil.hpp"
#include <QSettings>
#include <QEventLoop>

namespace asiobackend {

void bufferSwitch(long index, ASIOBool processNow);
ASIOTime* bufferSwitchTimeInfo(ASIOTime* timeInfo, long index, ASIOBool processNow);
long asioMessage(long selector, long value, void* message, double* opt);

void sampleRateDidChange(ASIOSampleRate sampleRate) {
    // dummy basically
}

DriverInfo AsioBackend::driverInfo = DriverInfo();

AsioBackend::~AsioBackend() {
    if (driverInfo.bufferInfos != nullptr) {
        ASIODisposeBuffers();
        std::cout << "ASIO buffers disposed of" << std::endl;
    }
    ASIOError result;

    result = ASIOExit();
    if (result == ASE_OK){
        std::cout << "ASIO driver exited correctly" << std::endl;
    }
    else {
        LOG_ERROR(result);
    }
}

void AsioBackend::run() {
    initialize();
    setupEffects();
}

void AsioBackend::setupEffects()
{
    processingList.push_back(new digitaleffects::Distortion());
}

void AsioBackend::initialize() {

    ASIOError result;

    if (!manualDriverSelection()) {
        std::cerr << "ASIO driver failed to load\n";
        exit(-1);
    }

    result = ASIOInit(&driverInfo.asioDriverInfo);
    if (result != ASE_OK) {
        std::cerr << "ASIOInit error\n";
        LOG_ERROR(result);

        exit(-2);
    }

    result = ASIOGetChannels(&driverInfo.inputChannels, &driverInfo.outputChannels);
    if (result != ASE_OK) {
        std::cerr << "ASIOGetchannels error\n";
        LOG_ERROR(result);
        exit(-2);
    }

    result = ASIOGetBufferSize(&driverInfo.minBufferSize,
                            &driverInfo.maxBufferSize,
                            &driverInfo.prefferedBufferSize,
                            &driverInfo.granularity);

    if (result != ASE_OK) {
        std::cerr << "ASIOGetBufferSize error\n";
        exit(-2);
    }

    result = ASIOGetSampleRate(&driverInfo.sampleRate);
    if (result != ASE_OK) {
        std::cerr << "ASIOGetBufferSize error\n";
        exit(-2);
    }

    if (ASIOOutputReady() == ASE_OK) {
        driverInfo.postOutput = true;
    }
    else {
        driverInfo.postOutput = false;
    }

    //setup callbacks

    m_callbacks.asioMessage = &asioMessage;
    m_callbacks.bufferSwitch = &bufferSwitch;
    m_callbacks.bufferSwitchTimeInfo = &bufferSwitchTimeInfo;
    m_callbacks.sampleRateDidChange = &sampleRateDidChange;

    ASIOBufferInfo* bufferInfo = driverInfo.bufferInfos;

    driverInfo.inputBuffers = driverInfo.inputChannels;
    for (int i = 0; i < driverInfo.inputBuffers; i++, bufferInfo++) {
        bufferInfo->isInput = ASIOTrue;
        bufferInfo->channelNum = i;
        bufferInfo->buffers[0] = 0;
        bufferInfo->buffers[1] = 0;
    }


    driverInfo.outputBuffers = driverInfo.outputChannels;
    for (int i = 0; i < driverInfo.outputBuffers; i++, bufferInfo++) {
        bufferInfo->isInput = ASIOFalse;
        bufferInfo->channelNum = i;
        bufferInfo->buffers[0] = 0;
        bufferInfo->buffers[1] = 0;
    }


    result = ASIOCreateBuffers(driverInfo.bufferInfos,
                driverInfo.inputBuffers + driverInfo.outputBuffers,
                driverInfo.prefferedBufferSize,
                &m_callbacks);


    if (result == ASE_OK) {
        for (int i = 0; i < driverInfo.inputBuffers + driverInfo.outputBuffers; i++) {
            driverInfo.channelInfos[i].channel = driverInfo.bufferInfos[i].channelNum;
            driverInfo.channelInfos[i].isInput = driverInfo.bufferInfos[i].isInput;
            result = ASIOGetChannelInfo(&driverInfo.channelInfos[i]);
            if (result != ASE_OK)
                break;
        }

        if (result == ASE_OK) {
            result = ASIOGetLatencies(&driverInfo.inputLatency, &driverInfo.outputLatency);

            if (result == ASE_OK) {
                std::cout << "Latencies: \n";
                std::cout << "In: " << driverInfo.inputLatency << std::endl;
                std::cout << "Out: " << driverInfo.outputLatency << std::endl;
            }
        }
    }

    result = ASIOStart();

    if (result == ASE_OK) {
        BackendInfo info{};
        info.bufferSize = driverInfo.maxBufferSize;
        info.inLatency = driverInfo.inputLatency;
        info.outLatency = driverInfo.outputLatency;
        info.sampleSize = driverInfo.sampleRate;
    }

    while (!driverInfo.stopped) {
        Sleep(100);
    }
    
    if ((result = ASIOStop() == ASE_OK)) {
        ASIODisposeBuffers();
    }
}

void AsioBackend::enumerateAudioDevices(std::vector<std::string>& driverNames) {
    long numDrivers = asioDrivers.asioGetNumDev();
    // List all driver names
    char driverName[256];
    for (long i = 0; i < numDrivers; ++i) {
        asioDrivers.asioGetDriverName(i, driverName, sizeof(driverName));
        driverNames.push_back(driverName);
    }
}

bool AsioBackend::manualDriverSelection() {
    std::vector<std::string> driverNames{};
    enumerateAudioDevices(driverNames);
    AsioDrivers drv;

    std::string driverName = "";
    settings.setValue("backend/drivername", "Focusrite USB ASIO");
    
    settings.sync();
    
    if (!settings.value("backend/drivername").isNull()) {
        driverName = settings.value("backend/drivername").toString().toStdString();
    }
    else {
        emit backendChooseAudioDriver(driverNames, driverName);
        settings.setValue("backend/drivername", QString::fromStdString(driverName));
    }
    drv.loadDriver(driverName.data());
    bool ok = AsioBackend::asioDrivers.loadDriver(driverName.data());
    return ok;
}

void bufferSwitch(long index, ASIOBool processNow) {
    ASIOTime time;
    memset(&time, 0, sizeof(time));

    if(ASIOGetSamplePosition(&time.timeInfo.samplePosition, &time.timeInfo.systemTime) == ASE_OK) {
        time.timeInfo.flags = 1 | 2; // ???
    }

    bufferSwitchTimeInfo(&time, index, processNow);
}

void setTimeInfo(ASIOTime* timeInfo) {
    AsioBackend::driverInfo.timeInfo = *timeInfo;
}

void processingFunction(int32_t* buffer, int size) {

}

ASIOTime* bufferSwitchTimeInfo(ASIOTime* timeInfo, long index, ASIOBool processNow) {

    AsioBackend::driverInfo.timeInfo = *timeInfo;

    if (timeInfo->timeInfo.flags & 1) { // 1 - system time valid
        AsioBackend::driverInfo.nanoSeconds = ASIO64toDouble(timeInfo->timeInfo.systemTime);
    }
    else {
        AsioBackend::driverInfo.nanoSeconds = 0;
    }

    if (timeInfo->timeInfo.flags & 2) { // 2 - sample position valid
        AsioBackend::driverInfo.samples = ASIO64toDouble(timeInfo->timeInfo.samplePosition);
    }
    else {
        AsioBackend::driverInfo.samples = 0;
    }

    if (timeInfo->timeCode.flags & 1) {
        AsioBackend::driverInfo.timeCodeSamples = ASIO64toDouble(timeInfo->timeCode.timeCodeSamples);
    }
    else {
        AsioBackend::driverInfo.timeCodeSamples = 0;
    }

    AsioBackend::driverInfo.sysRefTime = timeGetTime();

    long buffSize = AsioBackend::driverInfo.prefferedBufferSize;

    int32_t* inBuf = static_cast<int32_t*>(AsioBackend::driverInfo.bufferInfos[1].buffers[index]);
    int32_t* outBufL = static_cast<int32_t*>(AsioBackend::driverInfo.bufferInfos[3].buffers[index]);
    int32_t* outBufR = static_cast<int32_t*>(AsioBackend::driverInfo.bufferInfos[2].buffers[index]);

    static constexpr int32_t MAX_INT = std::numeric_limits<int32_t>::max();
    
    float lowPassYk = 0;
    float lowPassAlpha = 0.0001f;
    
    // oversampling
    //gain
    for (long idx = 0; idx < buffSize; ++idx)
    {
        float sample = inBuf[idx] / 2147483648.0f;

        sample *= AsioBackend::pregain;

        // // sample = tanhf(sample);

        // if (sample > 0)
        // {
        //     sample = 1.f - exp(-sample);
        // }
        // else if(sample < 0)
        // {
        //     sample = -1.f + exp(sample);
        // }

        int32_t processed = static_cast<int32_t>(sample * 2147483648.0f);

        outBufL[idx] = processed;
        outBufR[idx] = processed;
    }


    // for (long idx = 0; idx < buffSize; ++idx)
    // {
    //     float sample = inBuf[idx] / 2147483648.0f;

    //     // low pass filter start
    //     lowPassYk += lowPassAlpha * (sample - lowPassYk);
    //     // low pass filter end

    //     int32_t processed = static_cast<int32_t>(lowPassYk * 2147483648.0f);

    //     outBufL[idx] = processed;
    //     outBufR[idx] = processed;
    // }


    if (AsioBackend::driverInfo.postOutput)
		ASIOOutputReady();

	return 0L;
}

long asioMessage(long selector, long value, void* message, double* opt) {
    return 0L;
}

void AsioBackend::addEffect(EffectDfx* effect) {
    processingList.push_back(effect);
}

void AsioBackend::removeEffect(AsioBackend::EffectID id) {
    processingList.erase(std::find_if(processingList.begin(), processingList.end(), [id](const EffectDfx* rhs){ return (id == rhs->id);}));
}

} // asiobackend