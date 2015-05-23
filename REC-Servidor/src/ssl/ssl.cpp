#include "ssl.hpp"

Server::Server(QObject *parent) : QTcpServer (parent) {

    socket_=new QSslSocket;
    settings_=new QSettings;
    settings_->setValue("key", "SSL/server.key");
    settings_->setValue("certificate", "SSL/server.crt");
}

Server::~Server(){
    delete socket_;
    delete settings_;
}

void Server::incomingConnection(qintptr socketDescriptor){
    qDebug("estableciendo conexion");
}

void Server::connection_refused() {
    qDebug("Fallo en la conexion");
}

void Server::connection_disconnected(){
    qDebug("Conexion cerrada");
    socket_->disconnect();
    socket_->deleteLater();

}
