#ifndef USUARIO_H
#define USUARIO_H

#include <QTcpSocket>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QPainter>
#include <QSslSocket>

#include "captura.pb.h"


class usuario : public QObject
{
    Q_OBJECT
public:
    usuario(QObject *parent = 0);
    usuario(QSslSocket* c, QLabel* v, int i, QObject *parent =0);
    ~usuario();
    void set_id(int i);
    int get_id();
    void set_cliente(QSslSocket *client, QLabel *lvideo, int i);

private slots:

    void leer_datos();
    void connection_refused(QAbstractSocket::SocketError error_);
    void connection_disconnected();
    void errorOccured(const QList<QSslError> &error);

private:
    int id;
    int estado;
    int bytes_a;
    static int n_users;
    QSslSocket *cliente;
    QLabel *video;
    QLabel *mi_vid;
    Captura captura;
    int size;

};

#endif // USUARIO_H
