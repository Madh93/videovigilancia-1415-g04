#include "cliente.hpp"

Cliente::Cliente(qintptr descriptor, QObject* parent):
    QThread(parent),
    socket(NULL) {
        socket_descriptor = descriptor;
}


Cliente::~Cliente() {

    qDebug() << &socket;
    //qDebug() << "DEBUGEANDO 8...";
    if (socket) {
      //  qDebug() << "DEBUGEANDO 81...";
        socket->disconnectFromHost();
        //qDebug() << "DEBUGEANDO 82...";
        if (socket->state() == QAbstractSocket::UnconnectedState || socket->waitForDisconnected(1000)) {
          //  qDebug() << "DEBUGEANDO 83...";
            //delete socket;
            //qDebug() << "DEBUGEANDO 84...";
            //socket = NULL;
            socket->deleteLater();
        }
    }

    //qDebug() << "DEBUGEANDO 9...";
}


/***************************
 SLOTS
**************************/

void Cliente::leer() {

    //qDebug() << "Lee";

    // Recibir mensaje serializado (tamaño+mensaje)
    int size;
    std::string datos;

    socket->waitForReadyRead(10);
    socket->read(reinterpret_cast<char*>(&size),sizeof(size));
    datos.resize(size);
    socket->read(const_cast<char*>(datos.c_str()), (qint64)size);
    //socket->waitForReadyRead(-1);

    // Deserializar mensaje
    Captura captura;
    qRegisterMetaType<Captura>("Captura");
    captura.ParseFromString(datos);

//    qDebug() << captura.usuario().c_str();

    // Recuperar imagen
   // QImage img = QImage::fromData(reinterpret_cast<const uchar*>(captura.imagen().c_str()),
     //                             captura.imagen().size(),
       //                           "jpeg");

    emit nuevaImagen(captura);
}


void Cliente::desconectar() {

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
    //connect(socket, SIGNAL(disconnected()), this, SLOT(desconectar()));

    qDebug() << "El cliente " << socket_descriptor << " se ha conectado";

    exec();
}


QTcpSocket* Cliente::getSocket() {
    return socket;
}
