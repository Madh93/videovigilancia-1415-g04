#ifndef USUARIO_H
#define USUARIO_H

#include <QTcpSocket>
#include <QImage>
#include <QPixmap>
#include <QLabel>


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
    char tipo_dato;
    static int n_users;
    QTcpSocket *cliente;
    QLabel *video;
    QLabel *mi_vid;

};

#endif // USUARIO_H
