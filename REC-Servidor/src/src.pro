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
	 ssl/ssl.hpp \
    puertoescucha/puertoescucha.hpp \
    usuario/usuario.h

FORMS    += rec/rec.ui \
    acerca/acerca.ui \
    puertoescucha/puertoescucha.ui

RESOURCES += ../iconos.qrc \

PROTOS = ../proto/captura.proto
include(../protobuf.pri)
