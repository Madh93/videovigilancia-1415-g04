#include "usuario.h"

usuario::usuario(QObject *parent) : QObject(parent)
{
    id=0;
    estado=0;
    cliente=NULL;
    bytes_a=0;
    mi_vid=NULL;

    // Comprobar path de REC
    QString path = QDir::homePath()+"/.rec";
    QDir dir(path);

    // Si no existe, crear carpeta e iniciar contador
    if (!dir.exists()) {
        dir.mkpath(path);
        preferencias.setValue("cuentaImagenes", 0);
    }

    // Crear base de datos
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(QDir::homePath()+"/.rec/rec.sqlite");

    if (!database.open()) {
        qDebug() << "No se puede acceder a la base de datos.";
        return;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS recdata"
               "(id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "usuario VARCHAR(50), "
               "dispositivo VARCHAR(50), "
               "timestamp LONG, "
               "imagen BLOB)");
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


            // Guardar en disco duro
            guardarImagen(pixmap,
                          captura.usuario().c_str(),
                          captura.dispositivo().c_str(),
                          captura.timestamp());

            // Guardar en base de datos
            guardarImagenBDD(pixmap,
                          captura.usuario().c_str(),
                          captura.dispositivo().c_str(),
                          captura.timestamp());


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


void usuario::guardarImagen(QPixmap imagen, QString usuario, QString dispositivo, uint timestamp) {

    // Ejemplo: /home/$USER/.rec/usuario/dispositivo/00/0d/34/f2/25042015-00443576.jpg

    // Comprobar path de REC
    QString path = QDir::homePath()+"/.rec";
    QDir dir(path);

    // Si no existe, crear carpeta e iniciar contador
    if (!dir.exists()) {
        dir.mkpath(path);
        preferencias.setValue("cuentaImagenes", 0);
    }

    // Comprobar path del usuario
    path += "/"+usuario;
    dir.setPath(path);
    if (!dir.exists())
        dir.mkpath(path);

    // Comprobar path del dispositivo
    path += "/"+dispositivo;
    dir.setPath(path);
    if (!dir.exists())
        dir.mkpath(path);

    // Recuperar valor del contador de imágenes actual
    int cuenta = preferencias.value("cuentaImagenes").toInt();
    QString path_hex = QString("%1").arg(cuenta, 8, 16, QChar('0'));

    // Crear sistema hexadecimal de directorios
    for (int i=0; i<4;i++) {
        path.append("/"+path_hex.mid(i*2,2));
        dir.setPath(path);
        if (!dir.exists())
            dir.mkpath(path);
    }

    // Almacenar imagen en disco duro
    QDateTime fecha = QDateTime::currentDateTime().fromTime_t(timestamp);
    QString formato = fecha.toString(QLatin1String("ddMMyyyy-hhmmsszz"));
    QString path_imagen = path + QString::fromLatin1("/%1.jpg").arg(formato);

    // Aumentar contador y guardar imagen
    if (cuenta == qPow(16,8)-1)
        cuenta = -1;

    preferencias.setValue("cuentaImagenes", cuenta+1);
    imagen.save(path_imagen,0,60);
}


void usuario::guardarImagenBDD(QPixmap imagen, QString usuario, QString dispositivo, uint timestamp) {

    QSqlQuery query;

    // Agilizar petición
    QSqlQuery("PRAGMA synchronous = OFF");
    QSqlQuery("PRAGMA journal_mode = OFF");


    // Guardar imagen mediante QByteArray
    QByteArray img;
    QBuffer buffer(&img);
    buffer.open(QIODevice::WriteOnly);
    imagen.save(&buffer,"jpeg");


    // Insertar en la base de datos
    query.prepare("INSERT INTO recdata (usuario, dispositivo, timestamp, imagen) "
                  "VALUES (:usuario, :dispositivo, :timestamp, :imagen)");

    query.bindValue(":usuario", usuario);
    query.bindValue(":dispositivo", dispositivo);
    query.bindValue(":timestamp", timestamp);
    query.bindValue(":imagen", img);
    qDebug() << query.exec();
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
