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
    $$PWD/rec

SOURCES += main.cpp\
    acerca/acerca.cpp \
    capturebuffer/capturebuffer.cpp \
    conexion/conexion.cpp \
    dispositivos/dispositivos.cpp \
    rec/rec.cpp

HEADERS  += rec/rec.hpp \
    acerca/acerca.hpp \
    capturebuffer/capturebuffer.hpp \
    conexion/conexion.hpp \
    dispositivos/dispositivos.hpp

FORMS    += rec/rec.ui \
    acerca/acerca.ui \
    conexion/conexion.ui \
    dispositivos/dispositivos.ui

RESOURCES += ../iconos.qrc \
