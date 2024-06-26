#ifndef TCPCLIENT_HPP
#define TCPCLIENT_HPP

#include <QObject>
#include <QTcpSocket>
#include <iostream>

using std::cout, std::endl;

class TcpClient : public QObject {
    Q_OBJECT

   public:
    explicit TcpClient(QObject *parent = nullptr);
    void connect_to_server(QString ip, int port);
    void close_connection();
    void send(QString msg);
    bool is_connected();
    QAbstractSocket::SocketState state();

   signals:
    void connected();
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState);
    void errorOccurred(QAbstractSocket::SocketError);
    void data_ready(QByteArray);

   private slots:
    void socket_stateChanged(QAbstractSocket::SocketState state);
    void socket_readyRead();

   private:
    QTcpSocket _socket;
    QString _ip;
    int _port;
};

#endif  // TCPCLIENT_HPP
