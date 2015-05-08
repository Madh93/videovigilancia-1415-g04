#ifndef SERVIDOR_HPP
#define SERVIDOR_HPP

#include <QTcpServer>
#include "captura.pb.h"
#include "cliente.hpp"

class Servidor : public QTcpServer {

    Q_OBJECT

    private:

        int puerto;
        Cliente* cliente;

    signals:

        void nuevaImagen(Captura);

    public slots:

        void recibirImagen(Captura);

    public:

        explicit Servidor(int port, QObject *parent = 0);
        ~Servidor();
        bool iniciar();
        void detener();

    protected:

        void incomingConnection(qintptr descriptor);

};

#endif // SERVIDOR_HPP
