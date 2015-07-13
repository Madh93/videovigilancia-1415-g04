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
    void connection_refused(QAbstractSocket::SocketError error_);
    void connection_disconnected();
    void signal2();
    void errorOccured(const QList<QSslError> &error);

private:

    QSslSocket *connection_;
    QByteArray key_;
    QByteArray certificate_;
    QSettings *settings_;

};

#endif // SERVER_H
