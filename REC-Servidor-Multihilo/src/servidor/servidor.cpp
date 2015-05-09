#include "servidor.hpp"

Servidor::Servidor(int port, QObject* parent):
    QTcpServer(parent),
    cliente(NULL) {
        puerto = port;
}


Servidor::~Servidor() {

    //qDebug() << "DEBUGEANDO 6...";

    if (cliente) {
        delete cliente;
        cliente = NULL;
    }
    //qDebug() << "DEBUGEANDO 7...";
}


void Servidor::recibirImagen(Captura img) {

    emit nuevaImagen(img);
}


bool Servidor::iniciar() {

    // Mantenerse a la escucha por el puerto establecido
    if(!this->listen(QHostAddress::Any,puerto)) {
        qDebug() << "No se ha podido iniciar el servidor";
        return false;
    }
    else {
        qDebug() << "Servidor iniciado: escuchando por el puerto " << puerto << "...";
        return true;
    }
}

void Servidor::detener() {

    //qDebug() << "DEBUGEANDO 4...";
    //qDebug() << cliente->getSocket();
    //if (cliente->getSocket() == NULL)
    //cliente->getSocket()->disconnectFromHost();
       // qDebug() << "DEBUGEANDO 5...";

}




Cliente* Servidor::getCliente() {

    return cliente;
}


void Servidor::incomingConnection(qintptr descriptor) {

    qDebug() << descriptor << " iniciando conexión...";

    //Cliente *cliente = new Cliente(descriptor, this);
    cliente = new Cliente(descriptor, this);


    // Eliminar cliente al finalizar la conexión
    connect(cliente, SIGNAL(finished()), cliente, SLOT(deleteLater()));
    connect(cliente, SIGNAL(nuevaImagen(Captura)), this, SLOT(recibirImagen(Captura)));
    //connect(cliente, SIGNAL(finished()), cliente, SLOT(desconectar()));

    emit nuevoCliente(clientes.size()+1);

    cliente->start();

    clientes.push_back(cliente);
    qDebug() << clientes.size() << " clientes conectados.";
}
