#include "asiobackendutil.hpp"
#include "asiobackend.hpp"

#include "distortion.hpp"
#include "../math/mathutil.hpp"


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
}

void AsioBackend::initialize() {


    std::string driverName = "";
    manualDriverSelection(driverName);

    ASIOError result;

    if (!m_drivers.loadDriver(driverName.data())) {
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
        //emit backendReady(info);
    }

    while (!driverInfo.stopped) {
        Sleep(100);
    }
    
    if ((result = ASIOStop() == ASE_OK)) {
        ASIODisposeBuffers();
    }
}

void AsioBackend::manualDriverSelection(std::string& driverName) {
    std::vector<std::string> driverNames{};
    asiobackend::enumerateAudioDevices(driverNames);

    int idx = 1;
    for (auto _driverName : driverNames) {
        std::cout << idx++ << ". " << _driverName << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Choose your dirver: ";
    std::cin >> idx;
    driverName = driverNames[idx-1];
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

ASIOTime* bufferSwitchTimeInfo(ASIOTime* timeInfo, long index, ASIOBool processNow) {
    static long processedSamples = 0;
    AsioBackend::driverInfo.timeInfo = *timeInfo;
    static bool printed = false;

    // Debug: print buffer layout once
    if (!printed) {
        std::cout << "Input Channels: " << AsioBackend::driverInfo.inputChannels << std::endl;
        std::cout << "Output Channels: " << AsioBackend::driverInfo.outputChannels << std::endl;
        std::cout << "Input Buffers: " << AsioBackend::driverInfo.inputBuffers << std::endl;
        std::cout << "Output Buffers: " << AsioBackend::driverInfo.outputBuffers << std::endl;
        
        for (int i = 0; i < AsioBackend::driverInfo.inputBuffers + AsioBackend::driverInfo.outputBuffers; i++) {
            std::cout << "Buffer " << i << ": isInput=" 
                      << (AsioBackend::driverInfo.bufferInfos[i].isInput ? "YES" : "NO") 
                      << " channel=" << AsioBackend::driverInfo.bufferInfos[i].channelNum << std::endl;
        }
        printed = true;
    }


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
    constexpr float OUTGAIN = 9.0f;
    constexpr float INGAIN = 3.0f;
    for (int i = 0 ; i < buffSize; ++i) {

        float sample = inBuf[i] / 2147483648.0f;

        sample *= INGAIN;

        float threshold = 0.6f * INGAIN;
        if (sample > threshold) sample = threshold;
        else if (sample < -threshold) sample = -threshold;

        int32_t processed = static_cast<int32_t>(sample * 2147483648.0f);
        outBufL[i] = processed * OUTGAIN;
        outBufR[i] = processed * OUTGAIN;
    }

    if (AsioBackend::driverInfo.postOutput)
		ASIOOutputReady();

    processedSamples += buffSize;

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

}