#include "mainwindow.hpp"

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setup_TcpClient();
}

MainWindow::~MainWindow() { delete ui; }

/*
 *  Validate inputed ip address and change input box color accordingly
 *
 *  @param arg1: passed ip address
 */
void MainWindow::on_InIPAddress_textChanged(const QString &arg1) {
    QString state = "";
    QHostAddress address(arg1);

    // If it is a valid IPV4 change background to green else red
    state = arg1 == "..."                                         ? ""
            : QAbstractSocket::IPv4Protocol == address.protocol() ? "1"
                                                                  : "0";

    ui->InIPAddress->setProperty("state", state);
    style()->polish(ui->InIPAddress);
}

/* Handle connect button click */
void MainWindow::on_btnConnect_clicked() {
    // If socket is active act as a diconnect button else connect button
    if (_client.is_connected()) {
        _client.close_connection();
        std::cout << "Disconnecting" << std::endl;
    } else {
        auto ip = ui->InIPAddress->text();
        auto port = ui->spnPort->value();
        _client.connect_to_server(ip, port);
        std::cout << "Connecting" << std::endl;
    }
}

void MainWindow::device_connected() {
    ui->lstConsole->addItem("Connected to Server");
    ui->btnConnect->setText("Disconnect");
}

void MainWindow::device_disconnected() {
    ui->lstConsole->addItem("Disconnected form Server");
    ui->btnConnect->setText("Connect");
}

void MainWindow::device_stateChanged(QAbstractSocket::SocketState state) {
    return;
}

void MainWindow::device_errorOccurred(QAbstractSocket::SocketError error) {
    QString str;

    switch (error) {
        case QAbstractSocket::ConnectionRefusedError:
            str = "ConnectionRefusedError";
            break;
        case QAbstractSocket::RemoteHostClosedError:
            str = "RemoteHostClosedError";
            break;
        case QAbstractSocket::HostNotFoundError:
            str = "HostNotFoundError";
            break;
        case QAbstractSocket::SocketAccessError:
            str = "SocketAccessError";
            break;
        case QAbstractSocket::SocketResourceError:
            str = "SocketResourceError";
            break;
        case QAbstractSocket::SocketTimeoutError:
            str = "SocketTimeoutError";
            break;
        case QAbstractSocket::DatagramTooLargeError:
            str = "DatagramTooLargeError";
            break;
        case QAbstractSocket::NetworkError:
            str = "NetworkError";
            break;
        case QAbstractSocket::AddressInUseError:
            str = "AddressInUseError";
            break;
        case QAbstractSocket::SocketAddressNotAvailableError:
            str = "SocketAddressNotAvailableError";
            break;
        case QAbstractSocket::UnsupportedSocketOperationError:
            str = "UnsupportedSocketOperationError";
            break;
        case QAbstractSocket::UnfinishedSocketOperationError:
            str = "UnfinishedSocketOperationError";
            break;
        case QAbstractSocket::ProxyAuthenticationRequiredError:
            str = "ProxyAuthenticationRequiredError";
            break;
        case QAbstractSocket::SslHandshakeFailedError:
            str = "SslHandshakeFailedError";
            break;
        case QAbstractSocket::ProxyConnectionRefusedError:
            str = "ProxyConnectionRefusedError";
            break;
        case QAbstractSocket::ProxyConnectionClosedError:
            str = "ProxyConnectionClosedError";
            break;
        case QAbstractSocket::ProxyConnectionTimeoutError:
            str = "ProxyConnectionTimeoutError";
            break;
        case QAbstractSocket::ProxyNotFoundError:
            str = "ProxyNotFoundError";
            break;
        case QAbstractSocket::ProxyProtocolError:
            str = "ProxyProtocolError";
            break;
        case QAbstractSocket::OperationError:
            str = "OperationError";
            break;
        case QAbstractSocket::SslInternalError:
            str = "SslInternalError";
            break;
        case QAbstractSocket::SslInvalidUserDataError:
            str = "SslInvalidUserDataError";
            break;
        case QAbstractSocket::TemporaryError:
            str = "TemporaryError";
            break;
        default:
            str = "UnknownSocketError";
            break;
    }

    ui->lstConsole->addItem(str);
}

void MainWindow::setup_TcpClient() {
    // Connect signals of QTcpSocket to this class
    connect(&_client, &TcpClient::connected, this,
            &MainWindow::device_connected);
    connect(&_client, &TcpClient::disconnected, this,
            &MainWindow::device_disconnected);
    connect(&_client, &TcpClient::stateChanged, this,
            &MainWindow::device_stateChanged);
    connect(&_client, &TcpClient::errorOccurred, this,
            &MainWindow::device_errorOccurred);
}
