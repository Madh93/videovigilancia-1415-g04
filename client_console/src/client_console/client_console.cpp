#include "client_console.hpp"

int ClientConsole::sigHupSd[2];
int ClientConsole::sigTermSd[2];
int ClientConsole::sigIntSd[2];

ClientConsole::ClientConsole(QString host, int port, QString nameClient, int device) :

    camara(NULL),
    buffer(NULL),
    label(NULL) {

    port_=port;
    host_=host;
    clientName=nameClient;
    //device_=device;


    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sigHupSd))
        qFatal("Couldn't create HUP socketpair");
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sigTermSd))
        qFatal("Couldn't create TERM socketpair");
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sigIntSd))
        qFatal("Couldn't create INT socketpair");

    sigHupNotifier = new QSocketNotifier(sigHupSd[1],
       QSocketNotifier::Read, this);
    sigTermNotifier = new QSocketNotifier(sigTermSd[1],
       QSocketNotifier::Read, this);
    sigIntNotifier = new QSocketNotifier(sigIntSd[1],
       QSocketNotifier::Read, this);


    connect(sigHupNotifier, SIGNAL(activated(int)), this,
       SLOT(handleSigHup()));
    connect(sigTermNotifier, SIGNAL(activated(int)), this,
       SLOT(handleSigTerm()));
    connect(sigIntNotifier, SIGNAL(activated(int)), this,
       SLOT(handleSigInt()));


    //crearLabel();
}


ClientConsole::~ClientConsole() {

    //delete ui;
    delete label;

    if (camara) {
        delete camara;
        camara = NULL;
    }

    if (buffer) {
        delete buffer;
        buffer = NULL;
    }

    delete sigHupNotifier;
    delete sigTermNotifier;
    delete sigIntNotifier;
}


/***************************
 MÉTODOS PRIVADOS
**************************/

/*void ClientConsole::activarFuncionalidades(bool cond) {

    //ui->actionCerrar->setEnabled(cond);
    //ui->actionTomarInstantanea->setEnabled(cond);
}*/


/*void ClientConsole::crearLabel() {

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
    //QPalette paleta = this->palette();
    paleta.setColor(QPalette::Background, QColor(90,90,90));
    paleta.setColor(QPalette::WindowText, Qt::white);
    label->setPalette(paleta);

    //ui->verticalLayoutPrincipal->addWidget(label);
}*/


void ClientConsole::limpiarCamara() {

    if (camara) {
        disconnect(buffer, SIGNAL(transmitirImagen(QImage)), this, SLOT(actualizarImagen(QImage)));
        camara->stop();
        delete camara;
        camara = NULL;
        //this->setWindowTitle(WINDOW_TITLE);
    }

    if (buffer) {
        delete buffer;
        buffer = NULL;
    }

    //activarFuncionalidades(false);
    //crearLabel();
}


/***************************
 SLOTS
**************************/

void ClientConsole::actualizarImagen(QImage imagen){

    pixmap = QPixmap(QPixmap::fromImage(imagen.scaled(label->size())));

    // Mostrar hora
    QPainter painter(&pixmap);
    painter.setPen(Qt::green);
    painter.setFont(QFont("",20));
    painter.drawText(pixmap.width()-120,
                     pixmap.height()-20,
                     QTime().currentTime().toString());

    label->setPixmap(pixmap);
}


/***************************
 ARCHIVO
**************************/

void ClientConsole::on_actionCapturar_triggered() {

    // Borrar camara anterior
    on_actionCerrar_triggered();

    // Abrir camara por defecto o guardada en preferencias
    QString ruta = preferencias.value("dispositivo").toString();

    if (ruta.isEmpty()) {
        camara = new QCamera;
        preferencias.setValue("dispositivo", "");
    } else {
        //QList<QCameraInfo> dispositivos = QCameraInfo::availableCameras();
        //foreach (const QCameraInfo &disp, dispositivos)
                //if (disp.deviceName() == ruta) {
                    //camara = new QCamera(disp);
                    //break;
                }

        // Si la camara ya no existe
        if (!camara) {
            camara = new QCamera;
            preferencias.setValue("dispositivo", "");
        }
    }

    // Si no hay camara
    //if (!camara->isAvailable()) {
        //QMessageBox::critical(this, WINDOW_CRITICAL, "No existe ningún dispositivo o está ocupado.");
        //return;
    //}

    // Iniciar captura
    //buffer = new CaptureBuffer;
    //camara->setViewfinder(buffer);
    //camara->start();

    // Ajustes
    //activarFuncionalidades(true);
    //this->setWindowTitle(WINDOW_ClientConsoleORDING);
    //connect(buffer, SIGNAL(transmitirImagen(QImage)), this, SLOT(actualizarImagen(QImage)));
//}

void ClientConsole::on_actionCerrar_triggered() { limpiarCamara(); }

void ClientConsole::on_actionSalir_triggered() { qApp->quit(); }


/***************************
 HERRAMIENTAS
**************************/

/*void ClientConsole::on_actionTomarInstantanea_triggered() {

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
}*/


/***************************
 PREFERENCIAS
**************************/

/*void ClientConsole::on_actionConfigurarConexion_triggered() {

    Conexion w(preferencias.value("usuario").toString(),
               preferencias.value("diClientConsolecion").toString(),
               preferencias.value("puerto").toInt());

    if (w.exec() == QDialog::Accepted) {
        preferencias.setValue("usuario", w.getUsuario());
        preferencias.setValue("diClientConsolecion", w.getDiClientConsolecion());
        preferencias.setValue("puerto", w.getPuerto());
    }
}*/


/*void ClientConsole::on_actionDispositivos_triggered() {

    Dispositivos w(preferencias.value("dispositivo").toString());

    if (w.exec() == QDialog::Accepted) {
        preferencias.setValue("dispositivo", w.getDispositivo());
        on_actionCapturar_triggered();
    }
}*/


/*void ClientConsole::on_actionPantallaCompleta_toggled(bool cond) {

    if (cond)
        this->showFullScreen();
    else
        this->showMaximized();
}*/


/***************************
 AYUDA
**************************/

void ClientConsole::on_actionAyuda_triggered() {

    QDesktopServices::openUrl(
                QUrl("https://github.com/ull-etsii-sistemas-operativos/videovigilancia-1415-g04/wiki",
                QUrl::TolerantMode));
}

//void ClientConsole::on_actionAcercaDe_triggered() { Acerca w; w.exec(); }

//void ClientConsole::on_actionAcercaDeQt_triggered() { qApp->aboutQt(); }




//-------------Manejadores de las señales SIGHUP, SIGTERM y SIGINT------------------//

void ClientConsole::hupSignalHandler(int) {
    char a = 1;  //Con enviar un byte ya es suficiente.
    write(sigHupSd[0], &a, sizeof(a));
}

void ClientConsole::termSignalHandler(int) {
    char a = 1;  //Con enviar un byte ya es suficiente.
    write(sigTermSd[0], &a, sizeof(a));
}

void ClientConsole::intSignalHandler(int) {
    char a = 1;  //Con enviar un byte ya es suficiente.
    write(sigIntSd[0], &a, sizeof(a));
}


void ClientConsole::handleSigTerm() {

   //Desactivar la monitorizacion para que por el momento no lleguen mas señales de QT
   sigTermNotifier->setEnabled(false);

   //Leer y desechar el byte enviado
   char tmp;
   read(sigTermSd[1], &tmp, sizeof(tmp));

   //..tu codigo aqui
   QCoreApplication::quit();  //Por ejemplo detener aplicación
   qDebug("Sigterm");
   //Activar de nuevo la monitorizacion
   sigTermNotifier->setEnabled(true);
}


void ClientConsole::handleSigHup() {
    //Desactivar la monitorizacion para que por el momento no lleguen mas señales de QT
    sigHupNotifier->setEnabled(false);

    //Leer y desechar el byte enviado
    char tmp;
    read(sigHupSd[1], &tmp, sizeof(tmp));

    //..tu codigo aqui
    qDebug("Sigup");
    QCoreApplication::quit();

    //Activar de nuevo la monitorizacion
    sigHupNotifier->setEnabled(true);
}

void ClientConsole::handleSigInt() {
    //Desactivar la monitorizacion para que por el momento no lleguen mas señales de QT
    sigIntNotifier->setEnabled(false);

    //Leer y desechar el byte enviado
    char tmp;
    read(sigIntSd[1], &tmp, sizeof(tmp));

    //..tu codigo aqui
    qDebug("Sigint");
    qDebug ("HOLA SEÑAL");
    QCoreApplication::quit();
    qDebug ("ADIOS SEÑAL");

    //Activar de nuevo la monitorizacion
    sigIntNotifier->setEnabled(true);
}


//TRES TIPOS DE SEÑALES

int setupUnixSignalHandlers() {

    //Lo tenemos que hacer para los tres, term hup e int.
    struct::sigaction term;
    struct::sigaction hup;
    struct::sigaction int_;  //Si creamos la variable como "int" qt piensa que es un tipo de variable en vez de una variable en si.

    //TERM
    term.sa_handler = &ClientConsole::termSignalHandler;
    sigemptyset(&term.sa_mask);
    term.sa_flags = SA_RESTART;

    if (sigaction(SIGTERM, &term, 0) > 0)
        return 1;

    //HUP
    hup.sa_handler = &ClientConsole::termSignalHandler;
    sigemptyset(&hup.sa_mask);
    term.sa_flags = SA_RESTART;

    if (sigaction(SIGTERM, &hup, 0) > 0)
        return 2;

    //INT
    int_.sa_handler = &ClientConsole::termSignalHandler;
    sigemptyset(&int_.sa_mask);
    int_.sa_flags = SA_RESTART;

    if (sigaction(SIGTERM, &int_, 0) > 0)
        return 3;

    return 0;
}
