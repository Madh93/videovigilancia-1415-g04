#ifndef DEMONIO_HPP
#define DEMONIO_HPP

#include <QCoreApplication> //QCore...
#include <QSocketNotifier>  //QSocketNotifier...
#include <unistd.h>   //write,read...
#include <netdb.h>  //sockets...
#include <signal.h> //signals...


int setupUnixSignalHandlers();

class Demonio : public QObject {

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

        explicit Demonio(QObject *parent);
        ~Demonio();

        //---------UTILIZACIÓN DE SEÑALES POSIX------//
        static void hupSignalHandler(int unused);
        static void termSignalHandler(int unused);
        static void intSignalHandler(int unused);
};

#endif // DEMONIO_HPP
