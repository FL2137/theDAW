#ifndef ASIOBACKEND_HPP
#define ASIOBACKEND_HPP

#include "asiobackendutil.hpp"
#include "Effect.hpp"
#include <functional>
#include <vector>

#include <QObject>

namespace asiobackend {

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

    void runProcessing();
    std::vector<digitaleffects::EffectBase*> processingList;


public slots:
    void run();

signals:
    void backendReady(asiobackend::BackendInfo info);

};
}


#endif // ASIOBACKEND_HPP