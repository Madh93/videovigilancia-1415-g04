#include "demonio.hpp"

int Demonio::sigHupSd[2];
int Demonio::sigTermSd[2];
int Demonio::sigIntSd[2];

Demonio::Demonio(QObject* parent):

    QObject(parent) {

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
}


Demonio::~Demonio() {

    delete sigHupNotifier;
    delete sigTermNotifier;
    delete sigIntNotifier;
}


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


void Demonio::handleSigTerm() {

   //Desactivar la monitorizacion para que por el momento no lleguen mas señales de QT
   sigTermNotifier->setEnabled(false);

   //Leer y desechar el byte enviado
   char tmp;
   read(sigTermSd[1], &tmp, sizeof(tmp));

   //..tu codigo aqui... OJO!  <------------------------------------------------------
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
