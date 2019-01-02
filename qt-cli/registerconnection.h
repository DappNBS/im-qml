#ifndef REGISTERCONNECTION_H
#define REGISTERCONNECTION_H

#include "header.h"
#include <QObject>

class RegisterConnection : public QTcpSocket
{
    Q_OBJECT
public:
    explicit RegisterConnection(QObject *parent = nullptr);

    enum RegisterConnectionState {
        WaitingForGreeting,
        ReadingGreeting,
        ReadyForUse
    };

    enum DataType {
        GREETING,
        REGISTER,
        PEERLIST,
        UNDEFINED
    };

    void setLocalServerInfo(QString &name,QHostAddress &ip,int &port);

signals:
    void        newPeerList(QString &peerlist);

protected:
    void    timerEvent(QTimerEvent *);

public slots:
    void    processReadyRead();
    void    sendGreetingMessage();
    bool    sendPeriodicRegisterMessage();

private:
    int             readDataIntoBuffer(int maxSize = MaxBufferSize);
    int             dataLengthForCurrentDataType();
    bool            readProtocolHeader();
    bool            hasEnoughData();
    void            processData();

    QString         localUserName;
    QHostAddress    localServerIP;
    int             localServerPort;

    QByteArray      buffer;
    QTimer          periodicTimer;
    QString         peerListString;
    QString         greetingMessage;
    QString         registerMessage;

    enum RegisterConnectionState state;
    enum DataType currentDataType;

    int numBytesForCurrentDataType;
    int transferTimerId;

    bool isGreetingMessageSent;
};

#endif // REGISTERCONNECTION_H
