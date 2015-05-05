#include "servidor.hpp"

Servidor::Servidor(int port, QObject* parent):
    QTcpServer(parent) {
        puerto = port;
}


Servidor::~Servidor() {}


void Servidor::iniciarServidor() {

    // Mantenerse a la escucha por el puerto establecido
    if(!this->listen(QHostAddress::Any,puerto))
        qDebug() << "No se ha podido iniciar el servidor";
    else
        qDebug() << "Servidor iniciado: escuchando por el puerto " << puerto << "...";
}


void Servidor::incomingConnection(qintptr descriptor) {

    qDebug() << descriptor << " iniciando conexión...";

    Cliente *cliente = new Cliente(descriptor, this);

    // Eliminar cliente al finalizar la conexión
    connect(cliente, SIGNAL(finished()), cliente, SLOT(deleteLater()));

    cliente->start();
}
