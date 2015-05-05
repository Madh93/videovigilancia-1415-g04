QT       += core gui \
            multimedia multimediawidgets \
            network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = REC-Servidor
CONFIG += ordered
TEMPLATE = app

INCLUDEPATH += $$PWD/acerca \
    $$PWD/cliente \
    $$PWD/puertoescucha \
    $$PWD/../iconos \
    $$PWD/../proto \
    $$PWD/rec

SOURCES += main.cpp\
    acerca/acerca.cpp \
    cliente/cliente.cpp \
    puertoescucha/puertoescucha.cpp \
    rec/rec.cpp

HEADERS  += rec/rec.hpp \
    acerca/acerca.hpp \
    cliente/cliente.hpp \
    puertoescucha/puertoescucha.hpp

FORMS    += rec/rec.ui \
    acerca/acerca.ui \
    puertoescucha/puertoescucha.ui

RESOURCES += ../iconos.qrc \

PROTOS = ../proto/captura.proto
include(../protobuf.pri)