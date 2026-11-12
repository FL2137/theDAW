#include "./mainwindow.hpp"
#include "ui_mainwindow.h"

#include <vector>
#include <string>
#include <QSettings>
#include <QInputDialog>
#include <QString>
#include <QMessageBox>

void MainWindow::initAsioBackend() {
    
    asiobackend::AsioBackend::impl = std::make_unique<asiobackend::AsioBackend>(&m_effectQueue);

    asiobackend::AsioBackend::impl->moveToThread(&m_audioBackendThread);
    
    auto audioBackendImpl = asiobackend::AsioBackend::impl.get();

    connect(&m_audioBackendThread, &QThread::finished, audioBackendImpl, &QObject::deleteLater);
    connect(this, &MainWindow::startBackend, audioBackendImpl, &asiobackend::AsioBackend::run);
    connect(ui->asioButton, &QPushButton::clicked, audioBackendImpl, &asiobackend::AsioBackend::run); 
    // connect(&m_audioBackend,
    //         &asiobackend::AsioBackend::backendReady,
    //         this,
    //         &MainWindow::backendReady);
    connect(audioBackendImpl,
            &asiobackend::AsioBackend::backendChooseAudioDriver,
            this,
            &MainWindow::backendChooseAudioDriver,
            Qt::ConnectionType::BlockingQueuedConnection);

    m_audioBackendThread.start();
}

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->graphicsView->setScene(&m_coordinateSystem);
    m_coordinateSystem.setup();
    // setupVulkanWindow();
    initAsioBackend();

    connect(ui->addSineWaveButton, &QPushButton::pressed, this, &MainWindow::onAddSineWaveClicked);
}

MainWindow::~MainWindow() {
    if (m_audioBackendThread.isRunning()) {
        m_audioBackendThread.quit();
        m_audioBackendThread.wait(); 
    }
    delete ui;
}

void MainWindow::setupVulkanWindow() {

    m_vulkanRenderer = reinterpret_cast<WaveVulkanRenderer*>(m_vulkanWidget.createRenderer());

    QWidget* vulkanWindowWrapper = QWidget::createWindowContainer(&m_vulkanWidget);

    ui->vulkanLayout->addWidget(vulkanWindowWrapper);

}

void MainWindow::backendReady(const asiobackend::BackendInfo info) {
  
}

void MainWindow::backendChooseAudioDriver(const std::vector<std::string> drivers, std::string& selectedDriver) {
    QStringList items{};
    for (const std::string driverName : drivers) {
        items << QString::fromStdString(driverName);
    }
    bool ok{};
    QString item = QInputDialog::getItem(this, tr("Select audio driver"),
                                            tr("Audio Driver:"), items, 0, false, &ok);
    if (ok && !item.isEmpty()) {
        selectedDriver = item.toStdString();
    }
    return;
}

void MainWindow::onNewEffectCreated(digitaleffects::EffectBase* effect)
{
}


void MainWindow::onAddSineWaveClicked() {
    auto points = new QPointF[48000 * 5]{};
    core::math::generateSineWavePoints(points, 5, -19);
    m_coordinateSystem.addSignal(new SignalCurve(points, 48000 * 5));
}