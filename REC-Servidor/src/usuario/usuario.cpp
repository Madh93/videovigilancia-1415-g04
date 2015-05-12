#include "usuario.h"

usuario::usuario(QObject *parent) : QObject(parent)
{
    id=0;
    estado=0;
    cliente=NULL;
    bytes_a=0;
    mi_vid=NULL;
    //tipo_dato=" ";
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
    qDebug()<<estado;
    qDebug()<<cliente->bytesAvailable();
//    qDebug()<<"TODO GUAY";
    while(true){

        if (estado==0 && cliente->bytesAvailable()>=sizeof(tipo_dato)){

            cliente->read((char *) &tipo_dato,sizeof(tipo_dato));
            //cliente->read()
            qDebug() <<"tipo" << tipo_dato;
            estado=1;


        }else
            break;

    if (tipo_dato=='i'){ //si lo que recibe es una imagen

        if (estado==1 && cliente->bytesAvailable()>=sizeof(bytes_a)){

            cliente->read((char *) &bytes_a,sizeof(bytes_a));
            estado=2;
        }else
            break;

        qDebug()<<"bytes a" <<bytes_a;
        qDebug()<<"bytes disponibles"<<cliente->bytesAvailable();
        qDebug()<<"estado"<<estado;

        if (estado==2 && cliente->bytesAvailable()>=bytes_a){
            qDebug() <<  "cliente->bytesAvailable()";


            estado=0;
            tipo_dato=' ';
                QByteArray byte = cliente->read(bytes_a);
                qDebug() <<  "LEíDO";
                QImage imagen;
                imagen.loadFromData(byte,"JPEG");
                QPixmap pixmap;
                pixmap.convertFromImage(imagen);

            //video->setPixmap(pixmap);


                if (id==n_users-1)
                    video->setPixmap(pixmap);
                else{
                    if (mi_vid==NULL){
                        mi_vid= new QLabel();
                        //mi_vid->show();
                }
                mi_vid->setPixmap(pixmap);
                mi_vid->show();
            }


    }else
        break;


        }else
            if (tipo_dato=='r'){
                qDebug() << cliente->bytesAvailable()<<"disp";
                if (estado==1 && cliente->bytesAvailable()>=sizeof(bytes_a)){

                    cliente->read((char *) &bytes_a,sizeof(bytes_a));
                    estado=2;
                    qDebug()<<"bytes aaaa" <<bytes_a;

                }

            qDebug()<<"bytes a" <<bytes_a;
            qDebug()<<"bytes disponibles"<<cliente->bytesAvailable();
            qDebug()<<"estado"<<estado;

                if (estado==2 && cliente->bytesAvailable()>=bytes_a){
                    qDebug() <<  bytes_a;
                    qDebug()<<"bytes_aaavailables"<< cliente->bytesAvailable();


                    estado=0;
                    tipo_dato=' ';
                    int roi;
                    cliente->read((char *) &roi,sizeof(bytes_a));
                    qDebug() <<  "roi " << roi;
                }else
                    break;
            }
    }
}

