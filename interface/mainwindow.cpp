#include "mainwindow.hpp"

#include <QHostAddress>
#include <QStyle>

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

/*
 *  Validate inputed ip address and change input box color accordingly
 *  @param arg1: passed ip address
 */
void MainWindow::on_InIPAddress_textChanged(const QString &arg1) {
    QHostAddress address(arg1);

    // If it is a valid IPV4 change background to green else red
    QString state =
        QAbstractSocket::IPv4Protocol == address.protocol() ? "1" : "0";

    ui->InIPAddress->setProperty("state", state);
    style()->polish(ui->InIPAddress);
}
