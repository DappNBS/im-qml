//
#ifndef REGISTERSERVER_H
#define REGISTERSERVER_H

#include "header.h"
#include "registerconnection.h"

#include <QTcpServer>

class RegisterServer : public QTcpServer
{
	Q_OBJECT
public:
	explicit	RegisterServer(QObject *parent = nullptr);

	void		setServerInfo(QHostAddress ip,int port);
	bool		startServer();

signals:
	void		newRegisterConnection(RegisterConnection *connection);

protected:
	void		incomingConnection(qintptr socketDescriptor);

private:
	QHostAddress	myIP;
	int		myPort;

};
#endif
