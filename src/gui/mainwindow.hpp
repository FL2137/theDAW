#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "../backend/spscqueue.hpp"
#include <QMainWindow>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "../backend/asiobackend.hpp" 
#include "guiutil.hpp"
#include "../backend/distortion.hpp"
#include "graphics/waveview.hpp"
#include "signalcurve.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;

    QThread m_audioBackendThread;

    SpscQueue<digitaleffects::EffectBase*, SPSC_QUEUE_DEPTH>* m_effectQueue{nullptr};
    
    void initAsioBackend();

    VulkanWidget m_vulkanWidget;
    WaveVulkanRenderer* m_vulkanRenderer{nullptr};

    CoordinateSystem m_coordinateSystem;

    void setupVulkanWindow();

public slots:
    void backendChooseAudioDriver(const std::vector<std::string> drivers, std::string& selectedDriver);
    void backendReady(const asiobackend::BackendInfo info);
    void onNewEffectCreated(digitaleffects::EffectBase* effect);
    void onAddSineWaveClicked();
    
signals:
    void startBackend();
    void newEffectAdded(digitaleffects::EffectBase* effect);

};

#endif // MAINWINDOW_HPP