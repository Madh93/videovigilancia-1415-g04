QT       += core gui \
            multimedia multimediawidgets \
            network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = REC-Servidor
CONFIG += ordered
TEMPLATE = app

INCLUDEPATH += $$PWD/acerca \
    $$PWD/cliente \
    $$PWD/demonio \
    $$PWD/puertoescucha \
    $$PWD/../iconos \
    $$PWD/../proto \
    $$PWD/rec \
    $$PWD/servidor

SOURCES += main.cpp\
    acerca/acerca.cpp \
    cliente/cliente.cpp \
    demonio/demonio.cpp \
    puertoescucha/puertoescucha.cpp \
    rec/rec.cpp \
    servidor/servidor.cpp \
    rec/demonio.cpp

HEADERS  += rec/rec.hpp \
    acerca/acerca.hpp \
    cliente/cliente.hpp \
    demonio/demonio.hpp \
    puertoescucha/puertoescucha.hpp \
    servidor/servidor.hpp \
    rec/demonio.hpp

FORMS    += rec/rec.ui \
    acerca/acerca.ui \
    puertoescucha/puertoescucha.ui

RESOURCES += ../iconos.qrc \

PROTOS = ../proto/captura.proto
include(../protobuf.pri)

unix {          # Esta es configuración específica de Linux y UNIX
    # Variables
    #
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    BINDIR  = $$PREFIX/bin
    DATADIR = $$PREFIX/share
    CONFDIR = /etc
    isEmpty(VARDIR) {
        VARDIR  = /var/lib/$${TARGET}
    }

    DEFINES += APP_DATADIR=\"$$DATADIR\"
    DEFINES += APP_VARDIR=\"$$VARDIR\"
    DEFINES += APP_CONFFILE=\"$$CONFDIR/$${TARGET}.ini\"

    # Install
    #
    INSTALLS += target config desktop icon32 vardir

    ## Instalar ejecutable
    target.path = $$BINDIR

    ## Instalar archivo de configuración
    config.path = $$CONFDIR
    config.files += $${TARGET}.ini

    ## Instalar acceso directo en el menú del escritorio
    desktop.path = $$DATADIR/applications
    desktop.files += $${TARGET}.desktop

    ## Instalar icono de aplicación
    icon32.path = $$DATADIR/icons/hicolor/32x32/apps
    icon32.files += ./data/32x32/$${TARGET}.png

    ## Crear directorio de archivos variables
    vardir.path = $$VARDIR
    vardir.commands = true
}
