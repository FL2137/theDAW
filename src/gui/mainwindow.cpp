#include "./mainwindow.hpp"
#include "ui_mainwindow.h"

#include <vector>
#include <string>
#include <QSettings>
#include <QInputDialog>

void MainWindow::initAsioBackend() {
    m_audioBackend.moveToThread(&m_audioBackendThread);
    connect(&m_audioBackendThread, &QThread::finished, &m_audioBackend, &QObject::deleteLater);
    connect(this, &MainWindow::startBackend, &m_audioBackend, &asiobackend::AsioBackend::run);
    // connect(&m_audioBackend,
    //         &asiobackend::AsioBackend::backendReady,
    //         this,
    //         &MainWindow::backendReady);
    connect(&m_audioBackend,
            &asiobackend::AsioBackend::backendChooseAudioDriver,
            this,
            &MainWindow::backendChooseAudioDriver,
            Qt::ConnectionType::BlockingQueuedConnection);

    m_audioBackendThread.start();
    emit startBackend();
}

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_audioBackend(audioBackendMut, backendWaitCon)
{
    ui->setupUi(this);
    initAsioBackend();
}

MainWindow::~MainWindow() {
    m_audioBackendThread.quit();
    m_audioBackendThread.wait(); 
    delete ui;
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
