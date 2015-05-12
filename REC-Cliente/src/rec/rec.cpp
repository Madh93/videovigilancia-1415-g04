#include "rec.hpp"
#include "ui_rec.h"

Rec::Rec(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Rec),
    camara(NULL),
    buffer(NULL),
    label(NULL),
    cliente(NULL),
    conectado_(false),
    backgroundSubtractor(500,16,false) {

        ui->setupUi(this);
        crearLabel();
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

    activarFuncionalidades(false);
    crearLabel();
}


bool Rec::detectar_movimiento(QImage *imagen){

    cv::Mat images;
    images=QtOcv::image2Mat(*imagen);

    cv::Mat foregroundMask;
    cv::Mat back;

       //extraccion del fondo

        backgroundSubtractor(images, foregroundMask);
        backgroundSubtractor.getBackgroundImage(back);
        // Operaciones morfolóficas para eliminar las regiones de
        // pequeño tamaño. Erode() las encoge y dilate() las vuelve a
        // agrandar.

        cv::erode(foregroundMask, foregroundMask, cv::Mat());
        cv::dilate(foregroundMask, foregroundMask, cv::Mat());

        // Obtener los contornos que bordean las regiones externas
        // (CV_RETR_EXTERNAL) encontradas. Cada contorno es un vector
        // de puntos y se devuelve uno por región en la máscara.
        ContoursType contours;
        cv::findContours(foregroundMask, contours, CV_RETR_EXTERNAL,
                         CV_CHAIN_APPROX_NONE);
        bool movimiento=true;
        if (contours.empty()){

                movimiento=false;
        }

        cv::Rect rectangulo;
        std::vector<cv::Rect> boxes;
        for (int i=0;i < contours.size();i++){
            rectangulo= cv::boundingRect(contours[i]);
        boxes.push_back(rectangulo);
       }

       for (int ii=0; ii<boxes.size(); ii++) {

           char tipo='r';
           QBuffer buffer;
           QVector<int> roi;


           roi.push_back(boxes[ii].x);
           roi.push_back(boxes[ii].y);
           roi.push_back(boxes[ii].width);
           roi.push_back(boxes[ii].height);

           int tam=sizeof(roi);

           /*
           cliente->write((char *) &tipo,sizeof(tipo));
           cliente->write((char *) &a, sizeof(a));
           cliente->write(bytes);*/

           qDebug() << "boxes[ii].x";
           cliente->write((char *) &tipo,sizeof(tipo));
           qDebug() << tipo;
           cliente->write((char  *) &tam,sizeof(tam));
           qDebug()<<"tamaño"<<sizeof(roi.data());
           qDebug()<<"tamaño2"<<roi.data();
           qDebug()<<"tamaño3"<<&roi;
           qDebug()<<"tamaño4"<<roi;

           cliente->write((char *) &roi,sizeof(roi));

           qDebug() << sizeof(roi);


       /*qDebug() << boxes[ii].x << endl;
       qDebug() << boxes[ii].y << endl;
       qDebug() << boxes[ii].width << endl;
       qDebug() << boxes[ii].height << endl;*/
       //cv::rectangle(images,boxes[ii],cv::Scalar(0,255,0),2);
       }

               /* cv::drawContours( images, // draw contours here
                                                  contours, // draw these contours
                                                  -1, // draw all contours
                                                  cv::Scalar(0,0,255), // set color
                                                  2); // set thickness
               //cv::rectangle(images,rectangulo,cv::Scalar(0,255,0),2);
               //cv::rectangle(images,boxes,cv::Scalar(0,255,0),2);
*/
    *imagen=QtOcv::mat2Image(images);

    return movimiento;

}

void Rec::conectado(void){

    conectado_=true;
    qDebug() << "conectado wey";
}

void Rec::establecer_conexion(void){

    cliente=new QTcpSocket(this);

    cliente->connectToHost(preferencias.value("direccion").toString(),preferencias.value("puerto").toInt());
    qDebug() <<"conectado a:" <<preferencias.value("direccion").toString()<<preferencias.value("puerto").toInt();
    connect(cliente, SIGNAL(connected()), this, SLOT(conectado()));
    qDebug() << conectado_;
    connect(cliente, SIGNAL(disconnected()), cliente, SLOT(deleteLater()));

}


/***************************
 SLOTS
**************************/



void Rec::actualizarImagen(QImage imagen){

    bool movimiento= detectar_movimiento(&imagen);

    //qDebug() << movimiento;

    pixmap = QPixmap(QPixmap::fromImage(imagen.scaled(label->size())));

    // Mostrar hora
    QPainter painter(&pixmap);
    painter.setPen(Qt::green);
    painter.setFont(QFont("",20));
    painter.drawText(pixmap.width()-120,
                     pixmap.height()-20,
                     QTime().currentTime().toString());

    label->setPixmap(pixmap);

    if (conectado_ && movimiento){

        //qDebug() << "envio";

        QBuffer buffer;
        QImageWriter writer;
        writer.setDevice(&buffer);

        writer.setFormat("jpeg");
        writer.setCompression(70);
        writer.write(imagen);

        QByteArray bytes = buffer.buffer();

        int a = bytes.size();
        char tipo='i';

        /*cliente->write((char *) &tipo,sizeof(tipo));
        cliente->write((char *) &a, sizeof(a));
        cliente->write(bytes);
*/
    }

    else{
        //qDebug()<< "No envio";
    }
}


/***************************
 ARCHIVO
**************************/

void Rec::on_actionCapturar_triggered() {

    // Borrar camara anterior
    on_actionCerrar_triggered();
    establecer_conexion();
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

    // Iniciar captura
    buffer = new CaptureBuffer;
    camara->setViewfinder(buffer);
    camara->start();

    // Ajustes
    activarFuncionalidades(true);
    this->setWindowTitle(WINDOW_RECORDING);
    connect(buffer, SIGNAL(transmitirImagen(QImage)), this, SLOT(actualizarImagen(QImage)));
}

void Rec::on_actionCerrar_triggered() { limpiarCamara(); }

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
