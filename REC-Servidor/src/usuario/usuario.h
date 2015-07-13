#ifndef USUARIO_H
#define USUARIO_H

#include <QTcpSocket>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QPainter>
#include <QSettings>
#include <QtSql>

#include "captura.pb.h"


class usuario : public QObject
{
    Q_OBJECT
public:
    usuario(QObject *parent = 0);
    usuario(QTcpSocket* c, QLabel* v, int i, QObject *parent =0);
    ~usuario();
    void set_id(int i);
    int get_id();
    void set_cliente(QTcpSocket *client, QLabel *lvideo, int i);

private slots:

    void leer_datos();

private:
    int id;
    int estado;
    int bytes_a;
    static int n_users;
    QTcpSocket *cliente;
    QLabel *video;
    QLabel *mi_vid;
    Captura captura;
    QSettings preferencias;
    QSqlDatabase database;
    void guardarImagen(QPixmap imagen, QString usuario, QString dispositivo, uint timestamp);
    void guardarImagenBDD(QPixmap imagen, QString usuario, QString dispositivo, uint timestamp);

};

#endif // USUARIO_H
