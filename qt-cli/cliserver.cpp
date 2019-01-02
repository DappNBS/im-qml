#include "cliserver.h"

CliServer::CliServer(QObject *parent) : QTcpServer (parent)
{
    this->myIp      = QHostAddress::AnyIPv4;
    this->myPort    = 9001;
}

void CliServer::setCliSeverInfo(QHostAddress &ip,int &port){
    this->myIp      = ip;
    this->myPort    = port;
}

bool CliServer::startServer(){
    return this->listen(this->myIp,this->myPort);
}

void CliServer::incomingConnection(qintptr socketDescriptor){
    CliConnection *connection = new CliConnection(this);

    connection->setSocketDescriptor(socketDescriptor);

    emit newCliConnection(connection);
}
