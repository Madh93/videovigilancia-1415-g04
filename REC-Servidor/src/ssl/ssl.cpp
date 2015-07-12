#include "ssl.hpp"
#include <QtNetwork>
Server::Server(QObject *parent) : QTcpServer (parent) {

    connection_=new QSslSocket;
    settings_=new QSettings;
    settings_->setValue("key", "/home/eneas/cifrando/videovigilancia-1415-g04/REC-Servidor/SSL/server.key");
    settings_->setValue("certificate", "/home/eneas/cifrando/videovigilancia-1415-g04/REC-Servidor/SSL/server.crt");

}

Server::~Server(){
    delete connection_;
    delete settings_;
}

void Server::incomingConnection(qintptr socketDescriptor){

    qDebug("estableciendo conexion");

        //Inicializamos SSL connection (key+certificate)
        key_=settings_->value("key").toByteArray();
        certificate_=settings_->value("certificate").toByteArray();

    QFile key_file(key_);
    QFile crt_file(certificate_);
    QSslKey key_ssl(key_,QSsl::Rsa);
    QSslCertificate crt_ssl(certificate_);
    QList<QSslError> errors_;

    qDebug()<<key_file.fileName();

    if(connection_->setSocketDescriptor(socketDescriptor)) {

        if(key_file.open(QIODevice::ReadOnly)) {
            key_=key_file.readAll();
            key_file.close();
        } else {
            qDebug("Error en la clave");
        }

        if(crt_file.open(QIODevice::ReadOnly)) {
            certificate_=crt_file.readAll();
            crt_file.close();
        } else {
            qDebug("Error en el certificado");
        }

        //Establecer en la conexion la clave y el certificado de los ficheros.
        connection_->setPrivateKey(key_ssl);
        connection_->setLocalCertificate(crt_ssl);


        connect(connection_, SIGNAL(encrypted()), this, SLOT(signal2()));
        connect(connection_, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connection_refused(QAbstractSocket::SocketError)));
        connect(connection_, SIGNAL(disconnected()), this, SLOT(connection_disconnected()));

        qDebug("Encriptando..");
        connection_->startServerEncryption();

        errors_.append(QSslError::SelfSignedCertificate);
        errors_.append(QSslError::CertificateUntrusted);

        connection_->ignoreSslErrors(errors_);
    }
}

void Server::signal2() {
    emit signal();
}

void Server::connection_refused(QAbstractSocket::SocketError erroroso) {
    qDebug()<<erroroso;
    qDebug("Fallo en la conexion");
    //connection_->disconnect();
    //connection_->deleteLater();
    QNetworkRequest request;
    QSslConfiguration conf = connection_->sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    connection_->setSslConfiguration(conf);
    //QList<QSslError> expectedSslErrors;
    //expectedSslErrors.append(error.);
    //connection_->ignoreSslErrors(expectedSslErrors);
}

void Server::connection_disconnected(){
    qDebug("Conexion cerrada");
    connection_->disconnect();
    connection_->deleteLater();

}
