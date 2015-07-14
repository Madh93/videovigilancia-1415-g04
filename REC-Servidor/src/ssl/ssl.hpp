#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QSslSocket>
#include <QFile>
#include <QSslKey>
#include <QSettings>
#include <QLabel>

#include "usuario.h"

class Server : public QTcpServer {

    Q_OBJECT

public:

    explicit Server(QObject *parent=0);
    void add_label(QLabel* l);
    ~Server();

protected:
    void incomingConnection(qintptr socketDescriptor);

signals:

    void signal();

public slots:

    //void connection_refused(QAbstractSocket::SocketError error_);
    //void connection_disconnected();
    //void signal2();
    //void errorOccured(const QList<QSslError> &error);

private:

    //QSslSocket *connection_;
    QByteArray key_;
    QByteArray certificate_;
    QSettings *settings_;
    QVector<usuario*> users;
    QLabel* label;

};

#endif // SERVER_H
