#ifndef SERVIDOR_HPP
#define SERVIDOR_HPP

#include <QTcpServer>
#include "cliente.hpp"

class Servidor : public QTcpServer {

    Q_OBJECT

    private:

        int puerto;

    public:

        explicit Servidor(int port, QObject *parent = 0);
        ~Servidor();
        void iniciarServidor();

    protected:

        void incomingConnection(qintptr descriptor);

};

#endif // SERVIDOR_HPP
