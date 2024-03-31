#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QHostAddress>
#include <QMainWindow>
#include <QStyle>

#include "TcpClient.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

   private slots:
    void on_InIPAddress_textChanged(const QString &arg1);

    void on_btnConnect_clicked();

    void device_connected();
    void device_disconnected();
    void device_stateChanged(QAbstractSocket::SocketState);
    void device_errorOccurred(QAbstractSocket::SocketError);
    void device_dataReady(QByteArray data);

    void on_btnSendCmd_clicked();

   private:
    Ui::MainWindow *ui;
    TcpClient _client;

    void setup_TcpClient();
};
#endif  // MAINWINDOW_HPP
