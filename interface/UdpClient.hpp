#ifndef UDPCLIENT_HPP
#define UDPCLIENT_HPP

#include <QObject>
#include <QUdpSocket>

class UdpClient : public QObject {
    Q_OBJECT
   public:
    explicit UdpClient(QObject *parent = nullptr);

   signals:
};

#endif  // UDPCLIENT_HPP
