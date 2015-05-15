QT       += core gui \
            multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = REC-Cliente
CONFIG += ordered
TEMPLATE = app

INCLUDEPATH += $$PWD/acerca \
    $$PWD/capturebuffer \
    $$PWD/conexion \
    $$PWD/dispositivos \
    $$PWD/../iconos \
    $$PWD/../proto \    
    $$PWD/rec \
    $$PWD/cv

SOURCES += main.cpp\
    acerca/acerca.cpp \
    capturebuffer/capturebuffer.cpp \
    conexion/conexion.cpp \
    dispositivos/dispositivos.cpp \
    rec/rec.cpp \
    cv/cvmatandqimage.cpp

HEADERS  += rec/rec.hpp \
    acerca/acerca.hpp \
    capturebuffer/capturebuffer.hpp \
    conexion/conexion.hpp \
    dispositivos/dispositivos.hpp \
    cv/cvmatandqimage.h

FORMS    += rec/rec.ui \
    acerca/acerca.ui \
    conexion/conexion.ui \
    dispositivos/dispositivos.ui

RESOURCES += ../iconos.qrc \

PROTOS = ../proto/captura.proto
include(../protobuf.pri)

include(../QtOpenCV.pri)

add_opencv_modules(core video imgproc)
