#include "conexion.hpp"
#include "ui_conexion.h"

Conexion::Conexion(QString usuario, QString direccion, quint16 puerto) :
    QDialog(0),
    ui(new Ui::Conexion) {

        ui->setupUi(this);
        ui->lineEditUsuario->setText(usuario);
        ui->lineEditDireccion->setText(direccion);
        ui->lineEditPuerto->setText(QString::number(puerto));
        ui->lineEditPuerto->setValidator(new QIntValidator(1, 65535, this));
}


Conexion::~Conexion() {

    delete ui;
}


QString Conexion::getUsuario() {

    return ui->lineEditUsuario->text();
}


QString Conexion::getDireccion() {

    return ui->lineEditDireccion->text();
}


quint16 Conexion::getPuerto() {

    quint16 puerto = ui->lineEditPuerto->text().toInt();
    return puerto ? puerto : 0;
}
