#include "TcpClient.hpp"

TcpClient::TcpClient(QObject *parent) : QObject{parent} {
    // Connect signals of QTcpSocket to this class
    connect(&_socket, &QTcpSocket::connected, this, &TcpClient::connected);
    connect(&_socket, &QTcpSocket::disconnected, this,
            &TcpClient::disconnected);
    connect(&_socket, &QTcpSocket::stateChanged, this,
            &TcpClient::socket_stateChanged);
    connect(&_socket, &QTcpSocket::errorOccurred, this,
            &TcpClient::errorOccurred);
    connect(&_socket, &QTcpSocket::readyRead, this,
            &TcpClient::socket_readyRead);
}

void TcpClient::connect_to_server(QString ip, int port) {
    if (_socket.isOpen()) {
        // IP and port didnt change
        if (ip == _ip && port == _port) return;

        _socket.close();  // close previous socket
    }

    _ip = ip;
    _port = port;
    _socket.connectToHost(ip, port);
}

void TcpClient::close_connection() {
    cout << "test" << endl;
    _socket.close();
}

void TcpClient::send(QString msg) { _socket.write(msg.toUtf8()); }

bool TcpClient::is_connected() {
    return _socket.state() == QAbstractSocket::ConnectedState;
}

void TcpClient::socket_stateChanged(QAbstractSocket::SocketState state) {
    if (state == QAbstractSocket::UnconnectedState) {
        _socket.close();
    }

    emit stateChanged(state);
}

void TcpClient::socket_readyRead() {
    auto data = _socket.readAll();
    emit data_ready(data);
}
