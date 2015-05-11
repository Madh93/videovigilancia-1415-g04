#include "cliente.hpp"

Cliente::Cliente(qintptr descriptor, QObject* parent):
    QThread(parent),
    socket(NULL),
    socket_descriptor(descriptor),
    finalizar(false) {}


Cliente::~Cliente() {

    // Cuando se invoque el desconstructor, poner a TRUE finalizar
    // para avisar a run() que debe finalizar su bucle
    mutex.lock();
    finalizar = true;
    mutex.unlock();

    // Esperar a que finalice run() para invocar a destructor padre
    this->wait();
}


/***************************
 MÉTODOS PRIVADOS
**************************/

void Cliente::leerDatos() {

    // Bloquear datos
    QMutexLocker locker(&mutex);

    // qDebug() << "Lee datos...";

    // Recibir mensaje serializado (tamaño+mensaje)
    int size;
    std::string datos;

    // Esperar a que se hayan recibido los datos antes de leerlos
    socket->waitForReadyRead(-1);
    //socket->waitForReadyRead(100);
    socket->read(reinterpret_cast<char*>(&size),sizeof(size));
    try {
        datos.resize(size); // FALLA A MENUDO
    } catch (const std::length_error&) {
        qDebug() << "length error";
    }
    socket->read(const_cast<char*>(datos.c_str()), (qint64)size);

    // Deserializar mensaje
    if (!captura.ParseFromString(datos))
        return;

    emit nuevaImagen();
}


/***************************
 MÉTODOS PÚBLICOS
**************************/

void Cliente::run() {

    mutex.lock();
    socket = new QTcpSocket();
    qintptr descriptor = socket_descriptor;
    // Asignar descriptor al socket
    if(!socket->setSocketDescriptor(descriptor)) {
        emit error(socket->error());
        return;
    }
    mutex.unlock();

    qDebug() << "El cliente " << descriptor << " se ha conectado.";

    while (1) {

        leerDatos();

        // Salir del bucle
        QMutexLocker locker(&mutex); // Mis dudas con QMutexLocker AQUI
        if (finalizar || socket->state() == QAbstractSocket::UnconnectedState)
            break;
    }

    // Cerrar conexión y salir de run()
    mutex.lock();   // ¿Lock y Unlock, o sigue en pie Locker?
    socket->disconnectFromHost();
    if (socket->state() != QAbstractSocket::UnconnectedState)
        if (socket->waitForDisconnected(-1)) {
            qDebug() << "El cliente " << descriptor << " se ha desconectado.";
        }
    qRegisterMetaType<Cliente*>("Cliente");
    emit desconectado(this);
    mutex.unlock();
}


void Cliente::desconectar() {

    QMutexLocker locker(&mutex);
    finalizar = true;
}


Captura& Cliente::getCaptura() {

    //QMutexLocker locker(&mutex);  // No funciona bien así
    return captura;
}


qintptr Cliente::getDescriptor(){

    QMutexLocker locker(&mutex);
    return socket_descriptor;
}
