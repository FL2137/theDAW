#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "../backend/asiobackend.hpp" 
#include "guiutil.hpp"

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

    asiobackend::AsioBackend m_audioBackend;
    QThread m_audioBackendThread;

    QMutex audioBackendMut;
    QWaitCondition backendWaitCon;
    
    void initAsioBackend();

public slots:
    void backendChooseAudioDriver(const std::vector<std::string> drivers, std::string& selectedDriver);
    void backendReady(const asiobackend::BackendInfo info);

signals:
    void startBackend();

};

#endif // MAINWINDOW_HPP