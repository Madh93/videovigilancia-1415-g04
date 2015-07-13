#ifndef CLIENTE_HPP
#define CLIENTE_HPP

#include <QThread>
#include <QMutex>
#include <QTcpSocket>
#include <QDebug>
#include <stdexcept>

#include "captura.pb.h"

class Cliente : public QThread {

    Q_OBJECT

    private:

        QTcpSocket *socket;
        QMutex mutex;
        qintptr socket_descriptor;
        Captura captura;
        bool finalizar;

        void leerDatos();

    signals:

        void error(QTcpSocket::SocketError);
        void nuevaImagen();
        void desconectado(Cliente*);

    public:

        explicit Cliente(qintptr descriptor, QObject *parent = 0);
        ~Cliente();

        void run();
        void desconectar();
        Captura& getCaptura();
        qintptr getDescriptor();
        QString getId();
};

#endif // CLIENTE_HPP
