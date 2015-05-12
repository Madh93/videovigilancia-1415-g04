#include "rec.hpp"
#include "ui_rec.h"

Rec::Rec(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Rec),
    label(NULL) {

        ui->setupUi(this);
        crearLabel();
	
	//SQLite
	
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("data.sqlite");

    if (!db.open()) {
      QMessageBox::critical(NULL, tr("Error"), tr("No se pudo acceder a los datos."));
	}
	
	//-->Creación de tablas
	
	QSqlQuery query_;

    qDebug ("entrando");

    query_.exec("CREATE TABLE IF NOT EXISTS datos"
                "(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                " IDCAMERA VARCHAR(40),"
                " TIMESTAMP VARCHAR(40),"
                " IMAGE VARCHAR(200))");

    qDebug ("guardo tonterias a ver si funciona");
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


void Rec::guardarImagen(QPixmap imagen, QString usuario, uint timestamp) {

    // Ejemplo: /home/$USER/.rec/usuario/25042015-00443576.jpg

    // Comprobar path de REC
    QString path = QDir::homePath()+"/.rec";
    QString path_usuario = path+"/"+usuario;
    QDir dir(path);

    // Si no existe, crear carpeta
    if (!dir.exists())
        dir.mkpath(path);

    // Comprobar que existe directorio del usuario
    dir.setPath(path_usuario);
    if (!dir.exists())
        dir.mkpath(path_usuario);

    // Almacenar imagen en disco duro
    QDateTime fecha = QDateTime::currentDateTime().fromTime_t(timestamp);
    QString formato = fecha.toString(QLatin1String("ddMMyyyy-hhmmsszz"));
    QString path_imagen = path_usuario + QString::fromLatin1("/%1.jpg").arg(formato);

    imagen.save(path_imagen,0,60);
}


/***************************
 SLOTS
**************************/



/***************************
 ARCHIVO
**************************/

void Rec::on_actionIniciarServidor_triggered() {

    activarFuncionalidades(true);
    this->setWindowTitle(WINDOW_TITLE_ON);
    label->setText("Servidor iniciado...");
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
