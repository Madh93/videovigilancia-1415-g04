#ifndef CLIENTE_HPP
#define CLIENTE_HPP

#include <QThread>
#include <QTcpSocket>
#include <QImage>
#include "captura.pb.h"
#include <QDebug>

class Cliente : public QThread {

    Q_OBJECT

    private:

        QTcpSocket *socket;
        qintptr socket_descriptor;

    signals:

        void error(QTcpSocket::SocketError socketerror);
        void nuevaImagen(Captura);

    public slots:

        void leer();
        void desconectar();

    public:

        explicit Cliente(qintptr descriptor, QObject *parent = 0);
        ~Cliente();

        void run();
        QTcpSocket* getSocket();
};

#endif // CLIENTE_HPP
