#include "usuario.h"

usuario::usuario(QObject *parent) : QObject(parent)
{
    id=0;
    estado=0;
    cliente=NULL;
    bytes_a=0;
    mi_vid=NULL;
}


usuario::usuario(QTcpSocket* c, QLabel* v, int i, QObject *parent) : QObject(parent){

    estado=0;
    bytes_a=0;
    mi_vid=NULL;

    cliente=c;
    video=v;
    id=i;
    n_users++;
    qDebug() << "Hola";

    connect(cliente,SIGNAL(readyRead()),this,SLOT(leer_datos()));

}

usuario::~usuario()
{
    mi_vid=NULL;
    delete mi_vid;
    cliente=NULL;
    delete cliente;
    video=NULL;
    delete video;
}

int usuario::n_users=0;

int usuario::get_id(){

    return id;

}

void usuario::set_id(int i){

    id=i;
}

void usuario::set_cliente(QTcpSocket *client, QLabel *lvideo, int i){

    cliente=client;
    video=lvideo;
    id=i;
    n_users++;
}

void usuario::leer_datos(){

    // Recibir mensaje serializado (tamaño+mensaje)
    int size;
    std::string datos;

    cliente->read(reinterpret_cast<char*>(&size),sizeof(size));
    datos.resize(size);
    cliente->read(const_cast<char*>(datos.c_str()), (qint64)size);

    // Deserializar mensaje
    //captura.ParseFromString(datos);
    captura.ParsePartialFromString(datos);

    // Recuperar imagen
    QImage img = QImage::fromData(reinterpret_cast<const uchar*>(captura.imagen().c_str()),
                                  captura.imagen().size(),
                                  "jpeg");
    QPixmap pixmap = QPixmap(QPixmap::fromImage(img));

    // Añadir información
    QPainter painter(&pixmap);
    painter.setPen(Qt::red);
    painter.setFont(QFont("",14));
    painter.drawText(20,30,tr("Cliente: %1").arg(captura.usuario().c_str()));
    painter.drawText(20,50,tr("Timestamp: %1").arg(captura.timestamp()));

    // Mostrar imagen
    video->setPixmap(pixmap);

}
