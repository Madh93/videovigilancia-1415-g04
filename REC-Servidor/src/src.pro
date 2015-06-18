QT       += core gui \
            multimedia multimediawidgets \
            network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = REC-Servidor
CONFIG += ordered
TEMPLATE = app

INCLUDEPATH += $$PWD/acerca \
    $$PWD/puertoescucha \
    $$PWD/../iconos \
    $$PWD/../proto \
    $$PWD/rec \
	 $$PWD/ssl \
    $$PWD/usuario

SOURCES += main.cpp\
    acerca/acerca.cpp \
    puertoescucha/puertoescucha.cpp \
    rec/rec.cpp \
	 ssl/ssl.cpp \
    usuario/usuario.cpp

HEADERS  += rec/rec.hpp \
    acerca/acerca.hpp \
    puertoescucha/puertoescucha.hpp \
	 ssl/ssl.hpp \
    usuario/usuario.h

FORMS    += rec/rec.ui \
    acerca/acerca.ui \
    puertoescucha/puertoescucha.ui

RESOURCES += ../iconos.qrc \

PROTOS = ../proto/captura.proto
include(../protobuf.pri)

unix {

    INSTALLS += target ssl config vardir script

    ssl.path = $$DATADIR/SSL
    ssl.files += SSL/server.crt \
                 SSL/server.key
}
