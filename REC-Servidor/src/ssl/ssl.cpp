#include "ssl.hpp"

#include <QtNetwork>
Server::Server(QObject *parent) : QTcpServer (parent) {

    //connection_=new QSslSocket;
    settings_=new QSettings;
    settings_->setValue("key", "/home/eneas/cifrando/videovigilancia-1415-g04/REC-Servidor/SSL/server.key");
    settings_->setValue("certificate", "/home/eneas/cifrando/videovigilancia-1415-g04/REC-Servidor/SSL/server.crt");

}

Server::~Server(){
    //delete connection_;
    delete settings_;
}

void Server::add_label(QLabel* l){

    label=l;

}

void Server::incomingConnection(qintptr socketDescriptor){

    qDebug("estableciendo conexion");

        //Inicializamos SSL connection (key+certificate)
        key_=settings_->value("key").toByteArray();
        certificate_=settings_->value("certificate").toByteArray();


    QSslCertificate crt_ssl(certificate_);
    QList<QSslError> errors_;

    QSslSocket* connection_=new QSslSocket;



    if(connection_->setSocketDescriptor(socketDescriptor)) {

        //Establecer en la conexion la clave y el certificado de los ficheros.
        connection_->setPrivateKey(key_);
        connection_->setLocalCertificate(certificate_);


        //connect(connection_, SIGNAL(encrypted()), this, SLOT(signal2()));
        //connect(connection_, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connection_refused(QAbstractSocket::SocketError)));
        //connect(connection_, SIGNAL(disconnected()), this, SLOT(connection_disconnected()));
        //connect(connection_, SIGNAL(sslErrors(const QList<QSslError> &)),this, SLOT(errorOccured(const QList<QSslError> &)));

        errors_.append(QSslError::SelfSignedCertificate);
        errors_.append(QSslError::CertificateUntrusted);

        connection_->ignoreSslErrors(errors_);

        qDebug("Encriptando..");
        connection_->startServerEncryption();
        //this->addPendingConnection(connection_);

        static int i=0;
        usuario* usu= new usuario(connection_,label,i);
        users.push_back(usu);
        i++;

    }
}
/*
void Server::signal2() {
    emit signal();
}*/
/*
void Server::connection_refused(QAbstractSocket::SocketError error_) {
    qDebug()<<error_;
    qDebug("Fallo en la conexion");
    qDebug()<<connection_->errorString();
    connection_->disconnect();
    connection_->deleteLater();

}

void Server::connection_disconnected(){
    qDebug("Conexion cerrada");
    connection_->disconnect();
    connection_->deleteLater();

}*/

/*void Server::errorOccured(const QList<QSslError> & error)
{
  // simply ignore the errors
  // it should be very careful when ignoring errors

    for (int i=0;i<error.size();i++)
        error[i].errorString();
  //connection_->ignoreSslErrors(error);
}
*/
