#include "rec.hpp"
#include "ui_rec.h"

Rec::Rec(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Rec),
    servidor(NULL),
    label(NULL),
    conectados(NULL) {

        ui->setupUi(this);
        crearLabel();
        crearConectados();

        // Añadir información del estado del servidor
        ui->statusBar->addWidget(&statusIzda);
        ui->statusBar->addPermanentWidget(&statusDcha);
}


Rec::~Rec() {

    delete ui;
    delete label;

    if (servidor) {
        delete servidor;
        servidor = NULL;
    }
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

    ui->horizontalLayoutPrincipal->addWidget(label);
}


void Rec::crearConectados() {

    if (conectados) {
        delete conectados;
        conectados = NULL;
    }

    // Crear lista de conectados
    conectados = new QListWidget;
    conectados->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    conectados->setResizeMode(QListView::Adjust);
    conectados->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
//    label->setText("Esperando a iniciar servidor...");
//    label->setAutoFillBackground(true);
//    label->setAlignment(Qt::AlignCenter);
//    label->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);

    // Añadir color de fondo
//    QPalette paleta = this->palette();
//    paleta.setColor(QPalette::Background, QColor(90,90,90));
//    paleta.setColor(QPalette::WindowText, Qt::white);
//    label->setPalette(paleta);

    ui->horizontalLayoutPrincipal->addWidget(conectados);
}


void Rec::cerrarServidor() {

    servidor->detener();

    if (servidor) {
        qDebug() << "DEBUGEANDO 1...";
        servidor->close();
        //qDebug() << "DEBUGEANDO 2...";
        // disconnect(server, SIGNAL(newConnection()), this, SLOT(aceptarConexiones()));
        // disconnect(server, SIGNAL(acceptError(QAbstractSocket::SocketError)),
        //             this, SLOT(serverError(QAbstractSocket::SocketError)));
        delete servidor;
        //qDebug() << "DEBUGEANDO 3...";
        servidor = NULL;
    }
}


void Rec::recibirImagen(Captura captura) {

    // Recuperar imagen
    QImage img = QImage::fromData(reinterpret_cast<const uchar*>(captura.imagen().c_str()),
                                  captura.imagen().size(),
                                  "jpeg");
    pixmap = QPixmap(QPixmap::fromImage(img));

    // Añadir información
    QPainter painter(&pixmap);
    painter.setPen(Qt::red);
    painter.setFont(QFont("",14));
    painter.drawText(20,30,tr("Cliente: %1").arg(captura.usuario().c_str()));
    painter.drawText(20,50,tr("Timestamp: %1").arg(captura.timestamp()));

    // Mostrar imagen
    label->setPixmap(pixmap);
}


void Rec::nuevoCliente(int cliente) {

        //qDebug() << "mostrar en lista";
//    QListWidgetItem item(QString(cliente));
//    qDebug() << cliente;
//    qDebug() << QString(cliente);
    QListWidgetItem *item = new QListWidgetItem(QString::number(cliente),conectados);
//    item->setData(Qt::UserRole, QVariant(1));
    //conectados->addItem(&item);
    //conectados->setItemWidget(&item,this);
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

    // Comprobar alguna instancia anterior
    if (servidor)
        cerrarServidor();

    servidor = new Servidor(preferencias.value("puerto").toInt(),this);

    // Iniciar servidor
    if (!servidor->iniciar()) {
        QMessageBox::critical(this, WINDOW_CRITICAL,
                              tr("No se puede iniciar el servidor: %1.").arg(servidor->errorString()));
        return;
    }

    // connect(server, SIGNAL(newConnection()), this, SLOT(aceptarConexiones()));
    // connect(server, SIGNAL(acceptError(QAbstractSocket::SocketError)),
    //         this, SLOT(serverError(QAbstractSocket::SocketError)));

    // Ajustes
    activarFuncionalidades(true);
    this->setWindowTitle(WINDOW_TITLE_ON);
    label->setText("Servidor iniciado...");
    statusIzda.setText("Dirección IP: " + servidor->serverAddress().toString());
    statusDcha.setText("Puerto: " + QString::number(servidor->serverPort()));
    //connect(servidor->getCliente(), SIGNAL(nuevaImagen(Captura)), this, SLOT(recibirImagen(Captura)));
    connect(servidor, SIGNAL(nuevaImagen(Captura)), this, SLOT(recibirImagen(Captura)));
    connect(servidor, SIGNAL(nuevoCliente(int)), this, SLOT(nuevoCliente(int)));
}


void Rec::on_actionCerrar_triggered() {

    //qDebug() << "DEBUGEANDO 10...";
    cerrarServidor();
    //qDebug() << "DEBUGEANDO 20...";

    // Ajustes
    activarFuncionalidades(false);
    this->setWindowTitle(WINDOW_TITLE_OFF);
    label->setText("Esperando a iniciar servidor...");
    statusIzda.setText("");
    statusDcha.setText("");
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
