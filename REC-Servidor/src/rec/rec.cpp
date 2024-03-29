#include "rec.hpp"
#include "ui_rec.h"

Rec::Rec(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Rec),
    servidor(NULL),
    cliente(NULL),
    estado(0),
    bytes_a(0),
    label(NULL) {

        ui->setupUi(this);
        crearLabel();

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
            QMessageBox::critical(this, WINDOW_CRITICAL, "No se puede acceder a la base de datos.");
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


Rec::~Rec() {

    delete ui;
    delete label;
}


/***************************
 MÉTODOS PRIVADOS
**************************/

void Rec::activarFuncionalidades(bool cond) {

    ui->actionCerrar->setEnabled(cond);
    //ui->actionTomarInstantanea->setEnabled(cond);
}


void Rec::crearLabel() {

    if (label) {
        delete label;
        label = NULL;
    }

    // Crear label
    label = new QLabel;
    label->setText("Esperando a iniciar servidor...");
    label->setAutoFillBackground(true);
    label->setAlignment(Qt::AlignCenter);
    label->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);

    // Añadir color de fondo
    QPalette paleta = this->palette();
    paleta.setColor(QPalette::Background, QColor(90,90,90));
    paleta.setColor(QPalette::WindowText, Qt::white);
    label->setPalette(paleta);

    ui->verticalLayoutPrincipal->addWidget(label);
}


void Rec::guardarImagen(QPixmap imagen, QString usuario, QString dispositivo, uint timestamp) {


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


void Rec::guardarImagenBDD(QPixmap imagen, QString usuario, QString dispositivo, uint timestamp) {

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


/***************************
 SLOTS
**************************/


void Rec::leer_datos(){

    while(true){

    if (estado==0 && cliente->bytesAvailable()>=sizeof(bytes_a)){

        cliente->read((char *) &bytes_a,sizeof(bytes_a));
        estado=1;
    }
    qDebug()<<"bytes a" <<bytes_a;
    qDebug()<<"bytes disponibles"<<cliente->bytesAvailable();
    qDebug()<<"estado"<<estado;

    if (estado==1 && cliente->bytesAvailable()>=bytes_a){
        qDebug() <<  "cliente->bytesAvailable()";


        estado=0;

    QByteArray byte = cliente->read(bytes_a);
    QImage imagen;
    imagen.loadFromData(byte,"JPEG");
    QPixmap pixmap;
    pixmap.convertFromImage(imagen);
    label->setPixmap(pixmap);

    }else
        break;

    }


}

void Rec::nueva_conexion(){

    qDebug()<<"nueva conexion";
/*
    while (servidor->hasPendingConnections()){

        static int i=0;
        usuario* usu= new usuario(servidor->nextPendingConnection(),label,i);
        users.push_back(usu);
        i++;

    }*/
}

/***************************
 ARCHIVO
**************************/

void Rec::on_actionIniciarServidor_triggered() {

    activarFuncionalidades(true);
    this->setWindowTitle(WINDOW_TITLE_ON);
    label->setText("Servidor iniciado...");


    servidor=new Server(this);
    servidor->add_label(label);

    servidor->listen(QHostAddress::Any,preferencias.value("puerto").toInt());
    //QLabel *hola = new QLabel(tr("direccion ip: %1\npuerto: %2").arg(ip).arg(server->serverPort()));
    //hola->show();
    qDebug()<<"conectado a: "<<servidor->serverAddress()<< "y " << servidor->serverPort();
    //connect(servidor, SIGNAL(newConnection()), this, SLOT(nueva_conexion()));

}


void Rec::on_actionCerrar_triggered() {

    activarFuncionalidades(false);
    this->setWindowTitle(WINDOW_TITLE_OFF);
    label->setText("Esperando a iniciar servidor...");

}


void Rec::on_actionSalir_triggered() { qApp->quit(); }


/***************************
 HERRAMIENTAS
**************************/

void Rec::on_actionTomarInstantanea_triggered() {

    // Seleccionar ruta nueva
    QString ruta = QFileDialog::getSaveFileName(this, tr("Guardar instantánea"), QString(),
            tr("Todos los archivos (*);;"
               "Imagen BPM (*.bpm);;"
               "Imagen GIF (*.gif);"
               ";Imagen JPG (*.jpg);;"
               "Imagen JPEG (*.jpeg);;"
               "Imagen PNG (*.png);;"
               "Imagen PPM (*.ppm);;"
               "Imagen XBM (*.xbm);;"
               "Imagen XPM (*.xpm);;"));

    if (!ruta.isEmpty()) {

        QFile file(ruta);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(this, WINDOW_CRITICAL, "No se puede guardar la instantánea.");
            return;
        }

        // Guardar captura en la ruta indicada
        pixmap.save(ruta,0,60);
    }
}


/***************************
 PREFERENCIAS
**************************/

void Rec::on_actionEstablecerPuerto_triggered() {

    PuertoEscucha w(preferencias.value("puerto").toInt());

    if (w.exec() == QDialog::Accepted)
        preferencias.setValue("puerto", w.getPuerto());
}


void Rec::on_actionPantallaCompleta_toggled(bool cond) {

    if (cond)
        this->showFullScreen();
    else
        this->showMaximized();
}


/***************************
 AYUDA
**************************/

void Rec::on_actionAyuda_triggered() {

    QDesktopServices::openUrl(
                QUrl("https://github.com/ull-etsii-sistemas-operativos/videovigilancia-1415-g04/wiki",
                QUrl::TolerantMode));
}

void Rec::on_actionAcercaDe_triggered() { Acerca w; w.exec(); }

void Rec::on_actionAcercaDeQt_triggered() { qApp->aboutQt(); }
