#include "client_console.hpp"
#include "console.hpp"
#include <QApplication>
#include <QCoreApplication>
#include <QSettings>
#include <iostream>

int main(int argc, char *argv[]) {

    //Make install
        QSettings settings(APP_CONFFILE, QSettings::IniFormat);

    //Opciones de consola para el cliente
        QCoreApplication a(argc, argv);
        Console c;
        c.show_menu();
        setupUnixSignalHandlers();
        return a.exec();
    }

