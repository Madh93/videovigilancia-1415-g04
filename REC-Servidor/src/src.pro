QT       += core gui \
            multimedia multimediawidgets \
            network \
            sql


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = REC-Servidor
CONFIG += ordered
TEMPLATE = app

INCLUDEPATH += $$PWD/acerca \
    $$PWD/puertoescucha \
    $$PWD/../iconos \
    $$PWD/../proto \
    $$PWD/rec

SOURCES += main.cpp\
    acerca/acerca.cpp \
    puertoescucha/puertoescucha.cpp \
    rec/rec.cpp

HEADERS  += rec/rec.hpp \
    acerca/acerca.hpp \
    puertoescucha/puertoescucha.hpp

FORMS    += rec/rec.ui \
    acerca/acerca.ui \
    puertoescucha/puertoescucha.ui

RESOURCES += ../iconos.qrc \

PROTOS = ../proto/captura.proto
include(../protobuf.pri)
