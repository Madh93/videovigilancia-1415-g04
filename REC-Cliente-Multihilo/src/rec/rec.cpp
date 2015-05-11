#include "rec.hpp"
#include "ui_rec.h"

Rec::Rec(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Rec),
    camara(NULL),
    buffer(NULL),
    socket(NULL),
    label(NULL),
    conectado(false) {

        ui->setupUi(this);
        crearLabel();

        // Añadir información de estado
        ui->statusBar->addWidget(&statusIzda);
        statusIzda.setText("Desconectado");
}


Rec::~Rec() {

    delete ui;
    delete label;

    if (camara) {
        delete camara;
        camara = NULL;
    }

    if (buffer) {
        delete buffer;
        buffer = NULL;
    }

    if (socket)
        socket->deleteLater();
}


/***************************
 MÉTODOS PRIVADOS
**************************/

void Rec::activarFuncionalidades(bool cond) {

    ui->actionCerrar->setEnabled(cond);
    ui->actionTomarInstantanea->setEnabled(cond);
}


void Rec::crearLabel() {

    if (label) {
        delete label;
        label = NULL;
    }

    // Crear label
    label = new QLabel;
    label->setText("Esperando a capturar vídeo...");
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


void Rec::limpiarCamara() {

    if (camara) {
        disconnect(buffer, SIGNAL(transmitirImagen(QImage)), this, SLOT(actualizarImagen(QImage)));
        camara->stop();
        delete camara;
        camara = NULL;
        this->setWindowTitle(WINDOW_TITLE);
    }

    if (buffer) {
        delete buffer;
        buffer = NULL;
    }

    // Ajustes
    activarFuncionalidades(false);
    crearLabel();
    statusIzda.setText("Desconectado");
}


void Rec::limpiarSocket() {

    if (socket)
        socket->deleteLater();
}


void Rec::conectarConServidor() {

    // Iniciar conexión con el servidor
    socket = new QTcpSocket(this);
    socket->connectToHost(preferencias.value("direccion").toString(),
                          preferencias.value("puerto").toInt());

    // Señales de estado
    connect(socket, SIGNAL(connected()), this, SLOT(iniciarCamara()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(desconectar()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(socketError(QAbstractSocket::SocketError)));
}


/***************************
 SLOTS
**************************/

void Rec::actualizarImagen(QImage imagen){

    pixmap = QPixmap(QPixmap::fromImage(imagen.scaled(label->size())));

    // Mostrar hora
    QPainter painter(&pixmap);
    painter.setPen(Qt::green);
    painter.setFont(QFont("",20));
    painter.drawText(pixmap.width()-120,
                     pixmap.height()-20,
                     QTime().currentTime().toString());

    label->setPixmap(pixmap);


    if (conectado) {

        // Obtener imagen
        QBuffer img_buff;
        pixmap.toImage().save(&img_buff, "jpeg");

        //Crear paquete de protocolo
        Captura captura;
        captura.set_usuario(preferencias.value("usuario").toString().toStdString());
        captura.set_timestamp(QDateTime::currentDateTime().toTime_t());
        captura.set_imagen(img_buff.buffer().constData(), img_buff.buffer().size());
        captura.set_dispositivo(preferencias.value("dispositivo").toString().toStdString());

        // Serializar el mensaje
        std::string datos;
        captura.SerializeToString(&datos);
        int size = datos.size();

        // Enviar mensaje serializado (tamaño+mensaje)
        socket->write(reinterpret_cast<char*>(&size), sizeof(size));
        socket->write(datos.c_str(), size);

        if (socket->state() == QAbstractSocket::ConnectedState)
            socket->waitForBytesWritten(10);

        //qDebug() << "Envia";
    }
}


void Rec::iniciarCamara() {

    // Iniciar captura
    buffer = new CaptureBuffer;
    camara->setViewfinder(buffer);

    // Ajustes
    activarFuncionalidades(true);
    this->setWindowTitle(WINDOW_RECORDING);
    statusIzda.setText("Conectado");
    connect(buffer, SIGNAL(transmitirImagen(QImage)), this, SLOT(actualizarImagen(QImage)));

    camara->start();
    conectado = true;
}


void Rec::desconectar() {

    limpiarSocket();
    limpiarCamara();
    conectado = false;
}


void Rec::socketError(QAbstractSocket::SocketError error) {

    switch (error) {

        case QAbstractSocket::ConnectionRefusedError :
            QMessageBox::warning(this, WINDOW_WARNING,
                                 "No se ha encontrado el servidor.\nComprueba la dirección y el puerto.");
            limpiarSocket();
            break;

        case QAbstractSocket::HostNotFoundError :
            QMessageBox::warning(this, WINDOW_WARNING,
                                 "Dirección del servidor errónea.\nComprueba la dirección y el puerto.");
            limpiarSocket();
            break;

        case QAbstractSocket::RemoteHostClosedError :
            QMessageBox::warning(this, WINDOW_WARNING,
                                 "El servidor ha cerrado la conexión.");
            desconectar();
            break;

        default : break;
    }

}


/***************************
 ARCHIVO
**************************/

void Rec::on_actionCapturar_triggered() {

    // Borrar instancia anterior
    if (conectado)
        on_actionCerrar_triggered();

    // Abrir camara por defecto o guardada en preferencias
    QString ruta = preferencias.value("dispositivo").toString();

    if (ruta.isEmpty()) {
        camara = new QCamera;
        preferencias.setValue("dispositivo", "");
    } else {
        QList<QCameraInfo> dispositivos = QCameraInfo::availableCameras();
        foreach (const QCameraInfo &disp, dispositivos)
                if (disp.deviceName() == ruta) {
                    camara = new QCamera(disp);
                    break;
                }

        // Si la camara ya no existe
        if (!camara) {
            camara = new QCamera;
            preferencias.setValue("dispositivo", "");
        }
    }

    // Si no hay camara
    if (!camara->isAvailable()) {
        QMessageBox::critical(this, WINDOW_CRITICAL, "No existe ningún dispositivo o está ocupado.");
        return;
    }

    // Iniciar conexión con el servidor
    conectarConServidor();
}

void Rec::on_actionCerrar_triggered() {

    socket->disconnectFromHost();
    if (socket->state() == QAbstractSocket::UnconnectedState || socket->waitForDisconnected(1000))
        desconectar();
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

void Rec::on_actionConfigurarConexion_triggered() {

    Conexion w(preferencias.value("usuario").toString(),
               preferencias.value("direccion").toString(),
               preferencias.value("puerto").toInt());

    if (w.exec() == QDialog::Accepted) {
        preferencias.setValue("usuario", w.getUsuario());
        preferencias.setValue("direccion", w.getDireccion());
        preferencias.setValue("puerto", w.getPuerto());
    }
}


void Rec::on_actionDispositivos_triggered() {

    Dispositivos w(preferencias.value("dispositivo").toString());

    if (w.exec() == QDialog::Accepted) {
        preferencias.setValue("dispositivo", w.getDispositivo());
        if (camara)
            on_actionCapturar_triggered();
    }
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
