#ifndef CLISERVER_H
#define CLISERVER_H

#include "header.h"
#include "cliconnection.h"
#include <QObject>

class CliServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit    CliServer(QObject *parent = nullptr);

    void        setCliSeverInfo(QHostAddress &ip,int &port);
    bool        startServer();

signals:
    void        newCliConnection(CliConnection *connection);

protected:
    void        incomingConnection(qintptr socketDescriptor);

private:
    QHostAddress    myIp;
    int    myPort;
};

#endif // CLISERVER_H
