/**
 * 
 * author : lambor.cai <lamborcai@gmail.com>			
 *
 * Copyright (c) 2018-2025 lanbery,NBS Co.
 *  
 * 
 */

#ifndef REGISTERCONNECTION_H
#define REGISTERCONNECTION_H

#include "header.h"
#include <QObject>

class RegisterConnection : public QTcpSocket
{
	Q_OBJECT
public:
	enum RegisterConnectionState {
		WaitingForGreeting,
		ReadingGreeting,
		ReadyForUse
	};

	enum DataType
	{
		GREETING,
		REGISTER,
		PEERLIST,
		UNDEFINED
	};

    explicit RegisterConnection(QObject *parent = nullptr);
	~RegisterConnection();

    QString         getPeerServerName();

	QHostAddress 	getPeerServerIP();
    int             getPeerServerPort();

signals:
    void            newRegisterMsg(RegisterConnection *,QString);
    void            registerError(RegisterConnection *,QAbstractSocket::SocketError);

protected:
    void            timerEvent(QTimerEvent *);

public slots:
    void            processReadyRead();
    void            sendGreetingMessage();
    bool            sendPeriodicPeerlistMessage(QString message);
    void            handleSocketError(QAbstractSocket::SocketError);

private:
    int             readDataIntoBuffer(int maxSize = MaxBufferSize);
    int             dataLengthForCurrentDataType();
    bool            readProtocolHeader();
    bool            hasEnoughData();
    void            processData();

    QString         name;
	QHostAddress	ip;
    int             port;

    QByteArray      buffer;
    enum            RegisterConnectionState state;
    enum            DataType	currentDataType;

    int             numBytesForCurrentDataType;
    int             transferTimerId;
    bool            isGreetingMessageSent;
    QString         greetingMessage;
    QString         registerMessage;

};

#endif //REGISTERCONNECTION_H //

