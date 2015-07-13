#include "demonio.hpp"

int Demonio::sigHupSd[2];
int Demonio::sigTermSd[2];
int Demonio::sigIntSd[2];

Demonio::Demonio() {

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sigHupSd))
        qFatal("Couldn't create HUP socketpair");
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sigTermSd))
        qFatal("Couldn't create TERM socketpair");
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sigIntSd))
        qFatal("Couldn't create INT socketpair");

    sigHupNotifier = new QSocketNotifier(sigHupSd[1], QSocketNotifier::Read, this);
    sigTermNotifier = new QSocketNotifier(sigTermSd[1], QSocketNotifier::Read, this);
    sigIntNotifier = new QSocketNotifier(sigIntSd[1], QSocketNotifier::Read, this);

    connect(sigHupNotifier, SIGNAL(activated(int)), this, SLOT(handleSigHup()));
    connect(sigTermNotifier, SIGNAL(activated(int)), this, SLOT(handleSigTerm()));
    connect(sigIntNotifier, SIGNAL(activated(int)), this, SLOT(handleSigInt()));

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
        return;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS recdata"
               "(id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "usuario VARCHAR(50), "
               "dispositivo VARCHAR(50), "
               "timestamp LONG, "
               "imagen BLOB)");

    servidor = NULL;
    // Iniciar servidor
    on_actionIniciarServidor_triggered();
}


Demonio::~Demonio() {

    delete sigHupNotifier;
    delete sigTermNotifier;
    delete sigIntNotifier;

    if (servidor) {
        delete servidor;
        servidor = NULL;
    }
}


/***************************
 MÉTODOS PRIVADOS
**************************/

void Demonio::cerrarServidor() {

    if (servidor) {
        servidor->detener();
        delete servidor;
        servidor = NULL;
    }
}


void Demonio::guardarImagen(QPixmap imagen, QString usuario, QString dispositivo, uint timestamp) {

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


void Demonio::guardarImagenBDD(QPixmap imagen, QString usuario, QString dispositivo, uint timestamp) {

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

void Demonio::handleSigTerm() {

   //Desactivar la monitorizacion para que por el momento no lleguen mas señales de QT
   sigTermNotifier->setEnabled(false);

   //Leer y desechar el byte enviado
   char tmp;
   read(sigTermSd[1], &tmp, sizeof(tmp));

   //..tu codigo aqui... OJO!  <------------------------------------------------------
   cerrarServidor();
   on_actionIniciarServidor_triggered();

   QCoreApplication::quit();  //Por ejemplo detener aplicación
   qDebug("Sigterm");

   //Activar de nuevo la monitorizacion
   sigTermNotifier->setEnabled(true);
}


void Demonio::handleSigHup() {

    //Desactivar la monitorizacion para que por el momento no lleguen mas señales de QT
    sigHupNotifier->setEnabled(false);

    //Leer y desechar el byte enviado
    char tmp;
    read(sigHupSd[1], &tmp, sizeof(tmp));

    //..tu codigo aqui... OJO!  <------------------------------------------------------
    cerrarServidor();

    qDebug("Sigup");
    QCoreApplication::quit();

    //Activar de nuevo la monitorizacion
    sigHupNotifier->setEnabled(true);
}

void Demonio::handleSigInt() {

    //Desactivar la monitorizacion para que por el momento no lleguen mas señales de QT
    sigIntNotifier->setEnabled(false);

    //Leer y desechar el byte enviado
    char tmp;
    read(sigIntSd[1], &tmp, sizeof(tmp));

    //..tu codigo aqui... OJO!  <------------------------------------------------------
    cerrarServidor();

    qDebug("Sigint");
    qDebug ("HOLA SEÑAL");
    QCoreApplication::quit();
    qDebug ("ADIOS SEÑAL");

    //Activar de nuevo la monitorizacion
    sigIntNotifier->setEnabled(true);
}


void Demonio::recibirImagen(Captura captura) {

    if (captura.IsInitialized()) {

        // Recuperar imagen
        QImage img = QImage::fromData(reinterpret_cast<const uchar*>(captura.imagen().c_str()),
                                      captura.imagen().size(),
                                      "jpeg");
        pixmap = QPixmap(QPixmap::fromImage(img));

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
    }
}



/***************************
 METODOS PUBLICOS
**************************/

//-------------Manejadores de las señales SIGHUP, SIGTERM y SIGINT------------------//

void Demonio::hupSignalHandler(int) {
    char a = 1;  //Con enviar un byte ya es suficiente.
    write(sigHupSd[0], &a, sizeof(a));
}

void Demonio::termSignalHandler(int) {
    char a = 1;  //Con enviar un byte ya es suficiente.
    write(sigTermSd[0], &a, sizeof(a));
}

void Demonio::intSignalHandler(int) {
    char a = 1;  //Con enviar un byte ya es suficiente.
    write(sigIntSd[0], &a, sizeof(a));
}



void Demonio::on_actionIniciarServidor_triggered() {

    qDebug() << ("Iniciando servidor...");

    // Comprobar alguna instancia anterior
    if (servidor)
        cerrarServidor();

    servidor = new Servidor(12345,this);

    // Iniciar servidor
    if (!servidor->iniciar()) {
        qDebug() << ("No se puede iniciar el servidor...");
        return;
    }

    // Señales y slots
    connect(servidor, SIGNAL(nuevaImagen(Captura)), this, SLOT(recibirImagen(Captura)));
    connect(servidor, SIGNAL(nuevoCliente(int)), this, SLOT(nuevoCliente(int)));
    connect(servidor, SIGNAL(clienteDesconectado(int)), this, SLOT(clienteDesconectado(int)));
}


void Demonio::on_actionCerrar_triggered() {

    cerrarServidor();
}


//TRES TIPOS DE SEÑALES

int setupUnixSignalHandlers() {

    //Lo tenemos que hacer para los tres, term hup e int.
    struct::sigaction term;
    struct::sigaction hup;
    struct::sigaction int_;  //Si creamos la variable como "int" qt piensa que es
                             //un tipo de variable en vez de una variable en si.

    //TERM
    term.sa_handler = &Demonio::termSignalHandler;
    sigemptyset(&term.sa_mask);
    term.sa_flags = SA_RESTART;

    if (sigaction(SIGTERM, &term, 0) > 0)
        return 1;

    //HUP
    hup.sa_handler = &Demonio::termSignalHandler;
    sigemptyset(&hup.sa_mask);
    term.sa_flags = SA_RESTART;

    if (sigaction(SIGTERM, &hup, 0) > 0)
        return 2;

    //INT
    int_.sa_handler = &Demonio::termSignalHandler;
    sigemptyset(&int_.sa_mask);
    int_.sa_flags = SA_RESTART;

    if (sigaction(SIGTERM, &int_, 0) > 0)
        return 3;

    return 0;
}
