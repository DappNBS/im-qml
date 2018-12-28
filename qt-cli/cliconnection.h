#ifndef CLICONNECTION_H
#define CLICONNECTION_H

#include "header.h"
#include <QObject>


class CliConnection : public QTcpSocket
{
    Q_OBJECT
public:
    explicit CliConnection(QObject *parent = nullptr);

    //客户端连接状态
    enum CliConnectionState {
        WaitingForGreeting,
        ReadingGreeting,
        ReadyForUse
    };

    enum DataType{
        MESSAGE,
        GREETING,
        UNDEFINED
    };

    QString         getPeerName();
    QString         getPeerIP();
    int             getPeerPort();
    void            setGreetingMessage(QString message);
    bool            sendMessage(QString message);

signals:
    void            endGreeting(CliConnection *);
    void            newMessage(QString from,QString message);
    void            disconnectedChat(CliConnection *);
    void            socketError(CliConnection *,QAbstractSocket::SocketError);

protected:
    void            timerEvent(QTimerEvent *);

private slots:
    void            processReadyRead();
    void            sendGreetingMessage();
    void            handleDisconnected();
    void            handleSocketError(QAbstractSocket::SocketError);

private:
    int             readDataIntoBuffer(int maxSize = MaxBufferSize);
    int             dataLengthForCurrentDataType();
    bool            readProtocolHeader();
    bool            hasEnoughData();
    void            processData();

    //私有变量
    QString                     userName;
    QHostAddress                ip;
    int                         port;
    QString                     greetingMessage;
    QByteArray                  buffer;
    enum CliConnectionState     state;
    enum DataType               currentDataType;
    int                         numBytesForCurrentDataType;
    int                         transferTimerId;
    bool                        isGreetingMessageSent;
};

#endif // CLICONNECTION_H
