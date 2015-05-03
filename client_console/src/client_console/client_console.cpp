#include "client_console.hpp"

int ClientConsole::sigHupSd[2];
int ClientConsole::sigTermSd[2];
int ClientConsole::sigIntSd[2];

ClientConsole::ClientConsole(QObject *parent):

    QObject(parent) {

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

}


ClientConsole::~ClientConsole() {

    delete sigHupNotifier;
    delete sigTermNotifier;
    delete sigIntNotifier;
}


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
