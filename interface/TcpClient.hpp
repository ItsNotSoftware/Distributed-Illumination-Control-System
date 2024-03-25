#ifndef TCPCLIENT_HPP
#define TCPCLIENT_HPP

#include <QObject>
#include <QTcpSocket>

class TcpClient : public QObject {
    Q_OBJECT
   public:
    explicit TcpClient(QObject *parent = nullptr);
    void connect_to_server(QString ip, int port);

   signals:

   private:
    QTcpSocket _socket;
    QString _ip;
    int _port;
};

#endif  // TCPCLIENT_HPP
