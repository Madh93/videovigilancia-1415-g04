#ifndef REC_HPP
#define REC_HPP

// Títulos de ventana
#define WINDOW_TITLE "REC"
#define WINDOW_RECORDING "Capturando - REC"
#define WINDOW_CRITICAL "Error - REC"
#define WINDOW_WARNING "Aviso - REC"

#include <QMainWindow>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QPainter>
#include <QSettings>
#include <QTime>
#include <QCameraInfo>
#include <QCamera>
#include <QCoreApplication>
#include <QSocketNotifier>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <QSslSocket>

#include "capturebuffer.hpp"


int setupUnixSignalHandlers();

class ClientConsole : public QObject {

    Q_OBJECT


    private:

        //---------UTILIZACIÓN DE SEÑALES------//

        //Pareja de sockets. Un par por señal a manejar
        static int sigHupSd[2];
        static int sigTermSd[2];
        static int sigIntSd[2];

        //Objeto para monitorizar la pareja de sockets
        QSocketNotifier *sigHupNotifier;
        QSocketNotifier *sigTermNotifier;
        QSocketNotifier *sigIntNotifier;

    public slots:

        void handleSigHup();
        void handleSigTerm();
        void handleSigInt();

    public:

        explicit ClientConsole(QObject *parent);
        ~ClientConsole();

        //---------UTILIZACIÓN DE SEÑALES POSIX------//
        static void hupSignalHandler(int unused);
        static void termSignalHandler(int unused);
        static void intSignalHandler(int unused);
};

#endif // REC_HPP
