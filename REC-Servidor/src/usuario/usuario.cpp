#include "usuario.h"

usuario::usuario(QObject *parent) : QObject(parent)
{
    id=0;
    estado=0;
    cliente=NULL;
    bytes_a=0;
    mi_vid=NULL;
}


usuario::usuario(QSslSocket* c, QLabel* v, int i, QObject *parent) : QObject(parent){

    estado=0;
    bytes_a=0;
    mi_vid=NULL;

    cliente=c;
    video=v;
    id=i;
    n_users++;
    qDebug() << "Hola";

    connect(cliente,SIGNAL(readyRead()),this,SLOT(leer_datos()));
    connect(cliente, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connection_refused(QAbstractSocket::SocketError)));
    connect(cliente, SIGNAL(disconnected()), this, SLOT(connection_disconnected()));
    connect(cliente, SIGNAL(sslErrors(const QList<QSslError> &)),this, SLOT(errorOccured(const QList<QSslError> &)));



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

void usuario::set_cliente(QSslSocket *client, QLabel *lvideo, int i){

    cliente=client;
    video=lvideo;
    id=i;
    n_users++;
}

void usuario::leer_datos(){

    // Recibir mensaje serializado (tamaño+mensaje)


    while(true){

        qDebug()<<"entrando al while";

        if (estado==0 && cliente->bytesAvailable()>=sizeof(size)){

            cliente->read(reinterpret_cast<char*>(&size),sizeof(size));
            qDebug()<<size;

            estado=1;
            qDebug()<<"estado1 "<<estado;
        }

           qDebug()<<"estado2 "<<estado;
           qDebug()<<"bytes available"<<cliente->bytesAvailable();
           qDebug()<<"(qint64)size)"<<(qint64)size;


        if (estado==1 && cliente->bytesAvailable()>=(qint64)size){
            std::string datos;
            datos.resize(size);
            cliente->read(const_cast<char*>(datos.c_str()), (qint64)size);
            estado=0;
            qDebug()<<"estado3 "<<estado;


            // Deserializar mensaje
            captura.ParseFromString(datos);
            //captura.ParsePartialFromString(datos);

            QVector<Captura::Roi> rois;

            for (int i=0; i<captura.rois_size(); i++){
                rois.push_back(captura.rois(i));
             qDebug() << "rois";
             qDebug() << rois[i].x();
             qDebug() <<rois[i].y();
             qDebug() <<rois[i].width();
             qDebug() <<rois[i].height();
            }
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

            qDebug() << "leyendo datos";
            //estado==0;
            //Mostrar imagen
            video->setPixmap(pixmap);


            //QLabel* hola=new QLabel();
            //hola->setPixmap(pixmap);
            //hola->show();

            qDebug()<<"datos leidos";
        }
        else
            break;
        break;
    }

}

void usuario::connection_refused(QAbstractSocket::SocketError error_) {
    qDebug()<<error_;
    qDebug("Fallo en la conexion");
    qDebug()<<cliente->errorString();
    cliente->disconnect();
    cliente->deleteLater();

}

void usuario::connection_disconnected(){
    qDebug("Conexion cerrada");
    cliente->disconnect();
    cliente->deleteLater();

}

void usuario::errorOccured(const QList<QSslError> & error)
{
  // simply ignore the errors
  // it should be very careful when ignoring errors

    for (int i=0;i<error.size();i++)
        error[i].errorString();
  cliente->ignoreSslErrors(error);
}


