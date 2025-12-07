#include "./mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    m_audioBackend.moveToThread(&m_audioBackendThread);
    connect(&m_audioBackendThread, &QThread::finished, &m_audioBackend, &QObject::deleteLater);
    connect(this, &MainWindow::startBackend, &m_audioBackend, &asiobackend::AsioBackend::run);
    m_audioBackendThread.start();
    emit startBackend();
}


MainWindow::~MainWindow() {
    m_audioBackendThread.quit();
    m_audioBackendThread.wait(); 
    delete ui;
}