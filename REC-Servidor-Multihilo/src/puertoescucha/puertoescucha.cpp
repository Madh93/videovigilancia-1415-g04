#include "puertoescucha.hpp"
#include "ui_puertoescucha.h"

PuertoEscucha::PuertoEscucha(quint16 actual) :
    QDialog(0),
    ui(new Ui::PuertoEscucha) {

        ui->setupUi(this);
        ui->lineEdit->setText(QString::number(actual));
        ui->lineEdit->setValidator(new QIntValidator(1, 65535, this));
}


PuertoEscucha::~PuertoEscucha() {

    delete ui;
}


quint16 PuertoEscucha::getPuerto() {

    quint16 puerto = ui->lineEdit->text().toInt();
    return puerto ? puerto : 0;
}
