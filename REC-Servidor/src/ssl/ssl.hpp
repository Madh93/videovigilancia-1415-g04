#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QSslSocket>
#include <QFile>
#include <QSslKey>
#include <QSettings>

class Server : public QTcpServer {

    Q_OBJECT

public:

    explicit Server(QObject *parent=0);
    ~Server();

signals:

    void signal();

public slots:

    void incomingConnection(qintptr socketDescriptor);
    void connection_refused();
    void connection_disconnected();
    void signal2();

private:

    QSslSocket *connection_;
    QByteArray key_;
    QByteArray certificate_;
    QSettings *settings_;

};

#endif // SERVER_H
