#ifndef CONEXION_HPP
#define CONEXION_HPP

#include <QDialog>

namespace Ui {
    class Conexion;
}

class Conexion : public QDialog {

    Q_OBJECT

    private:
        Ui::Conexion *ui;

    public:
        explicit Conexion(QString usuario = "", QString direccion = "", quint16 puerto = 0);
        ~Conexion();

        QString getUsuario();
        QString getDireccion();
        quint16 getPuerto();
};

#endif // CONEXION_HPP
