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
    socket->read(reinterpret_cast<char*>(&size),sizeof(size));
    try {
        datos.resize(size); // FALLA A MENUDO
    } catch (const std::length_error&) {
        qDebug() << "OJO: length error";
    }
    if (socket->read(const_cast<char*>(datos.c_str()), (qint64)size) < 0)
        return;

    // Deserializar mensaje
    captura.ParseFromString(datos);

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

    qDebug() << "El cliente" << descriptor << "se ha conectado.";

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
        if (socket->waitForDisconnected(-1))
            qDebug() << "El cliente" << descriptor << "se ha desconectado.";
    socket->deleteLater();
    qRegisterMetaType<Cliente*>("Cliente");
    emit desconectado(this);
    mutex.unlock();
}


void Cliente::desconectar() {

    //QMutexLocker locker(&mutex);  // Tiempo de espera MUY Largo (10 segundos)
    finalizar = true;
}


Captura& Cliente::getCaptura() {

    //QMutexLocker locker(&mutex);  // No funciona bien así
    return captura;
}


qintptr Cliente::getDescriptor() {

    QMutexLocker locker(&mutex);
    return socket_descriptor;
}


QString Cliente::getId() {
    QString id = QString("%1 - %2").arg(QString::fromUtf8(captura.usuario().c_str()),
                                        QString::fromUtf8(captura.dispositivo().c_str()));
    return id;
}
