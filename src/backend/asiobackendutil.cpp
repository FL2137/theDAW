#include "asiobackendutil.hpp"

#include <chrono>
#include <random>

namespace asiobackend {

    void enumerateAudioDevices(std::vector<std::string> &driverNames) {
        // Create the AsioDrivers singleton
        static AsioDrivers drivers;
    
        // Get the number of installed ASIO drivers
        long numDrivers = drivers.asioGetNumDev();
    
        // List all driver names
        char driverName[256];
        for (long i = 0; i < numDrivers; ++i) {
            drivers.asioGetDriverName(i, driverName, sizeof(driverName));
            driverNames.push_back(driverName);
        }
    }
} 