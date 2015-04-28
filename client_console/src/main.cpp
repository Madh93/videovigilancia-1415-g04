#include "client_console.hpp"
#include <QApplication>
#include <QCoreApplication>
#include <QSettings>
#include <iostream>

int main(int argc, char *argv[]) {

    QByteArray disp, port;
    QString host, nameClient;

    //Make install
    QSettings settings(APP_CONFFILE, QSettings::IniFormat);

    //Opciones de consola para el cliente

    if (argc >= 2) {

        for(int i = 1; i < argc; ++i){

            if(strcmp(argv[i],"h") == 0 || strcmp(argv[i], "help") == 0 || strcmp(argv[i], "?") == 0) {

                std::cout << "Dispositivo:    d o disp <num dispositivo>" << std::endl;
                std::cout << "Puerto:         p o port <puerto>" << std::endl;
                std::cout << "Host:           ht o host <host>" << std::endl;
                std::cout << "Nombre cliente: n o name" << std::endl;
            }

            if(strcmp(argv[i],"d") == 0 || strcmp(argv[i], "disp") == 0){
                 disp = argv[i+1];
            }
            if(strcmp(argv[i],"p") == 0 || strcmp(argv[i], "port") == 0){
                 port = argv[i+1];
            }
            if(strcmp(argv[i],"ho") == 0 || strcmp(argv[i], "host") == 0){
                 host = argv[i+1];
            }
            if(strcmp(argv[i],"n") == 0 || strcmp(argv[i], "name") == 0) {
                 nameClient = argv[i+1];
            }
        }
    }

        QCoreApplication a(argc, argv);
        ClientConsole c (host,port.toInt(),nameClient, disp.toInt());
        setupUnixSignalHandlers();
        return a.exec();
    }

