#include "servidor.hpp"

Servidor::Servidor(int port, QObject* parent):
    QTcpServer(parent),
    puerto(port),
    actual(0) { }


Servidor::~Servidor() {

    foreach(Cliente* cliente, clientes) {
        cliente->desconectar();
        clientes.removeOne(cliente);
    }

    this->close();
}


/***************************
 SLOTS
**************************/

void Servidor::desconectado(Cliente* cliente) {

    int pos = clientes.indexOf(cliente);
    qDebug() << "El cliente " << clientes[pos]->getDescriptor() << " se ha desconectado.";

    emit clienteDesconectado(pos);

    clientes.removeOne(cliente);
    cliente->desconectar();
}

void Servidor::recibirImagen() {

    if (actual >= 0)
        emit nuevaImagen(clientes[actual]->getCaptura());
}


void Servidor::setActual(int actual) {

    this->actual = actual;
}


/***************************
 MÉTODOS PÚBLICOS
**************************/

bool Servidor::iniciar() {

    // Mantenerse a la escucha por el puerto establecido
    if(!this->listen(QHostAddress::Any,puerto)) {
        qDebug() << "No se ha podido iniciar el servidor.";
        return false;
    }
    else {
        qDebug() << "Servidor iniciado: escuchando por el puerto" << puerto << "...";
        return true;
    }
}

void Servidor::detener() {

    qDebug() << "Cerrando servidor...";

    foreach(Cliente* cliente, clientes) {
        cliente->desconectar();
        clientes.removeOne(cliente);
    }

    this->close();

    qDebug() << "Servidor cerrado.";
}


Cliente* Servidor::clienteActual() {

    return clientes[actual];
}


int Servidor::clientesConectados() {

    return clientes.size();
}

/***************************
 MÉTODOS PROTEGIDOS
**************************/

void Servidor::incomingConnection(qintptr descriptor) {

    qDebug() << descriptor << "iniciando conexión...";

    Cliente *cliente = new Cliente(descriptor, this);
    clientes.push_back(cliente);

    // Eliminar cliente al finalizar la conexión
    connect(cliente, SIGNAL(finished()), cliente, SLOT(deleteLater()));
    connect(cliente, SIGNAL(nuevaImagen()), this, SLOT(recibirImagen()));
    connect(cliente, SIGNAL(desconectado(Cliente*)), this, SLOT(desconectado(Cliente*)));

    // Comenzar a leer datos
    cliente->start();

    if (clientes.size() == 1)
        qDebug() << clientes.size() << "cliente conectado actualmente.";
    else
        qDebug() << clientes.size() << "clientes conectados actualmente.";

    emit nuevoCliente(clientes.size());
}
