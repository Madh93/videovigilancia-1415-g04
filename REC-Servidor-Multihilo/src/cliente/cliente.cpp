#include "cliente.hpp"

Cliente::Cliente(qintptr descriptor, QObject* parent):
    QThread(parent),
    socket(NULL) {
        socket_descriptor = descriptor;
}


Cliente::~Cliente() {

    if (socket) {
        delete socket;
        socket = NULL;
    }
}


/***************************
 SLOTS
**************************/

void Cliente::leer() { }


void Cliente::desconectado() {

    qDebug() << "El cliente " << socket_descriptor << " se ha desconecto";
    socket->deleteLater();
    exit(0);
}

/***************************
 MÉTODOS
**************************/

void Cliente::run() {

    qDebug() << "Nuevo cliente!";

    socket = new QTcpSocket();

    // Comprobar la asignación del descriptor de socket
    if(!socket->setSocketDescriptor(socket_descriptor)) {

        emit error(socket->error());
        return;
    }

    // Configurar señales de estado
    connect(socket, SIGNAL(readyRead()), this, SLOT(leer()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(desconectado()));

    qDebug() << "El cliente " << socket_descriptor << " se ha conectado";

    exec();
}
