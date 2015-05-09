#ifndef SERVIDOR_HPP
#define SERVIDOR_HPP

#include <QTcpServer>
#include <QVector>
#include "captura.pb.h"
#include "cliente.hpp"

class Servidor : public QTcpServer {

    Q_OBJECT

    private:

        int puerto;
        Cliente* cliente;
        QVector<Cliente*> clientes;

    signals:

        void nuevaImagen(Captura);
        void nuevoCliente(int);

    public slots:

        void recibirImagen(Captura);

    public:

        explicit Servidor(int port, QObject *parent = 0);
        ~Servidor();
        bool iniciar();
        void detener();
        Cliente* getCliente();

    protected:

        void incomingConnection(qintptr descriptor);

};

#endif // SERVIDOR_HPP
