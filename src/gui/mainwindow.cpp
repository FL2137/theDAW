#include "./mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::initAsioBackend() {
    m_audioBackend.moveToThread(&m_audioBackendThread);
    connect(&m_audioBackendThread, &QThread::finished, &m_audioBackend, &QObject::deleteLater);
    connect(this, &MainWindow::startBackend, &m_audioBackend, &asiobackend::AsioBackend::run);
    connect(&m_audioBackend,
            &asiobackend::AsioBackend::backendReady,
            this,
            &MainWindow::backendReady);

    m_audioBackendThread.start();
    emit startBackend();
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    initAsioBackend();
}

MainWindow::~MainWindow() {
    m_audioBackendThread.quit();
    m_audioBackendThread.wait(); 
    delete ui;
}


void MainWindow::backendReady(asiobackend::BackendInfo info) {
    ui->bufferSizeCombo->setCurrentText(guiutil::intToQStr(info.bufferSize));
    ui->sampleFreqCombo->setCurrentText(guiutil::intToQStr(info.sampleSize));
    ui->driverCombo->setCurrentText("ASIO");
}