#ifndef CLIENTE_HPP
#define CLIENTE_HPP

#include <QThread>
#include <QTcpSocket>
#include <QDebug>

class Cliente : public QThread {

    Q_OBJECT

    private:

        QTcpSocket *socket;
        qintptr socket_descriptor;

    signals:

        void error(QTcpSocket::SocketError socketerror);

    public slots:

        void leer();
        void desconectado();

    public:

        explicit Cliente(qintptr descriptor, QObject *parent = 0);
        ~Cliente();

        void run();
};

#endif // CLIENTE_HPP
