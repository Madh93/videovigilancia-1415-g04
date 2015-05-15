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
    conectados->setMaximumWidth(25);
    conectados->setCurrentRow(0);

    ui->horizontalLayoutPrincipal->addWidget(conectados);
}


void Rec::cerrarServidor() {

    conectados->clear();

    if (servidor) {
        servidor->detener();
        delete servidor;
        servidor = NULL;
    }
}


void Rec::guardarImagen(QPixmap imagen, QString usuario, uint timestamp) {

    // Ejemplo: /home/$USER/.rec/00/0d/34/f2/25042015-00443576.jpg

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


/***************************
 SLOTS
**************************/

void Rec::recibirImagen(Captura captura) {

    if (captura.IsInitialized()) {

        // Recuperar imagen
        QImage img = QImage::fromData(reinterpret_cast<const uchar*>(captura.imagen().c_str()),
                                      captura.imagen().size(),
                                      "jpeg");
        pixmap = QPixmap(QPixmap::fromImage(img));

        // Añadir información
        QPainter painter(&pixmap);
        painter.setPen(Qt::red);
        painter.setFont(QFont("",11));
        painter.drawText(15,15,tr("Cliente: %1").arg(captura.usuario().c_str()));
        painter.drawText(15,30,tr("Dispositivo: %1").arg(captura.dispositivo().c_str()));
        painter.drawText(15,45,tr("Timestamp: %1").arg(captura.timestamp()));

        // Mostrar imagen
        label->setPixmap(pixmap);

        // Guardar en disco duro
        //guardarImagen(pixmap,
        //              captura.usuario().c_str(),
        //              captura.timestamp());
    }   
}


void Rec::nuevoCliente(int cliente) {

    QListWidgetItem *item = new QListWidgetItem;
    item->setText(QString::number(cliente));
    conectados->addItem(item);
}


void Rec::clienteDesconectado(int cliente) {

    conectados->takeItem(cliente);
    label->setText(QString("Servidor iniciado...\nCliente %1: desconectado.").
                   arg(conectados->currentRow()+2));
}


/***************************
 ARCHIVO
**************************/

void Rec::on_actionIniciarServidor_triggered() {

    qDebug() << ("Iniciando servidor...");

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

    // Ajustes
    activarFuncionalidades(true);
    this->setWindowTitle(WINDOW_TITLE_ON);
    label->setText("Servidor iniciado...");
    statusIzda.setText("Dirección IP: " + servidor->serverAddress().toString());
    statusDcha.setText("Puerto: " + QString::number(servidor->serverPort()));

    // Señales y slots
    connect(servidor, SIGNAL(nuevaImagen(Captura)), this, SLOT(recibirImagen(Captura)));
    connect(servidor, SIGNAL(nuevoCliente(int)), this, SLOT(nuevoCliente(int)));
    connect(servidor, SIGNAL(clienteDesconectado(int)), this, SLOT(clienteDesconectado(int)));
    connect(conectados, SIGNAL(currentRowChanged(int)), servidor, SLOT(setActual(int)));
}


void Rec::on_actionCerrar_triggered() {

    cerrarServidor();

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
