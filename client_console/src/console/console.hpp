 #ifndef CONSOLE_H
#define  CONSOLE_H

#include <QObject>
#include <iostream>
#include <QTextStream>
#include <QSettings>
#include <QCamera>
#include <QByteArray>
#include <QString>

class Console:public QObject {

    Q_OBJECT

public:

    explicit Console(QObject *parent = 0);
    ~Console();
    void show_menu();
    void show_ip();
    void show_port();
    void show_device();

private:

    QTextStream *in_;
    QTextStream *out_;

    QString eleccion_;
    QString ip_;
    QString port_;
    QSettings config_;
    QList <QByteArray> devices_;


};

#endif
