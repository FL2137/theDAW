#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QThread>
#include "../backend/asiobackend.hpp"

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

signals:
    void startBackend();

};

#endif // MAINWINDOW_HPP