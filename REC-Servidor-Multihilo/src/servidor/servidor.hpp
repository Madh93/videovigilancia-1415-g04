#ifndef SERVIDOR_HPP
#define SERVIDOR_HPP

#include <QTcpServer>
#include <QList>

#include "cliente.hpp"

class Servidor : public QTcpServer {

    Q_OBJECT

    private:

        int puerto;
        int actual;
        QList<Cliente*> clientes;

    signals:

        void nuevaImagen(Captura);
        void nuevoCliente(QString);
        void clienteDesconectado(int);

    public slots:

        void recibirImagen();
        void desconectado(Cliente* cliente);
        void setActual(int actual);

    public:

        explicit Servidor(int port, QObject *parent = 0);
        ~Servidor();
        bool iniciar();
        void detener();
        Cliente* clienteActual();
        int clientesConectados();

    protected:

        void incomingConnection(qintptr descriptor);
};

#endif // SERVIDOR_HPP
