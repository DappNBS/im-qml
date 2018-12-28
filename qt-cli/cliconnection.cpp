#include "cliconnection.h"

static const int    TransferTimeout = 30 * 1000;
static const char   SeparatorToken  = ' ';

CliConnection::CliConnection(QObject *parent) : QTcpSocket (parent)
{
    this->greetingMessage = QObject::tr("undefined");
    this->userName = QObject::tr("unknown");
    this->state = WaitingForGreeting;
    this->currentDataType = UNDEFINED;
    this->numBytesForCurrentDataType = -1;
    this->transferTimerId = 0;
    this->isGreetingMessageSent = false;

    connect(this,SIGNAL(readyRead()),this,SLOT(processReadyRead()));
    connect(this,SIGNAL(connected()),this,SLOT(sendGreetingMessage()));
    connect(this,SIGNAL(disconnected()),this,SLOT(handleDisconnected()));
    connect(this,SIGNAL(error(QAbstractSocket::SocketError)),this,
            SLOT(handleSocketError()));
}

QString CliConnection::getPeerName(){
    return this->userName;
}

QString CliConnection::getPeerIP(){
    return this->ip.toString();
}

int CliConnection::getPeerPort(){
    return this->port;
}

void CliConnection::handleSocketError(QAbstractSocket::SocketError error){
    emit this->socketError(this,error);
}

void CliConnection::handleDisconnected(){
    emit this->disconnectedChat(this);
}

void CliConnection::setGreetingMessage(QString greet){
    this->greetingMessage = greet;
}

void CliConnection::timerEvent(QTimerEvent *timerEvent){
    if(timerEvent->timerId() == this->transferTimerId){
        this->abort();
        this->killTimer(transferTimerId);
        transferTimerId = 0;
    }
}

//发送message消息
bool CliConnection::sendMessage(const QString message){
    if(message.isEmpty())
        return false;
    QByteArray msg = message.toUtf8();
    QByteArray data = "MESSAGE " + QByteArray::number(msg.size())
            + ' ' + msg;

    return this->write(data) == data.size();
}

//读取当前消息到buffer，返回读取长度
int CliConnection::readDataIntoBuffer(int maxSize){
    if(maxSize > MaxBufferSize)
        return 0;

    int numBytesBeforeRead = this->buffer.size();

    if(numBytesBeforeRead == MaxBufferSize){
        this->abort();
        return 0;
    }

    while (bytesAvailable()>0 && buffer.size() < maxSize) {
        buffer.append(this->read(1));
        if(buffer.endsWith(SeparatorToken))
            break;
    }

    return buffer.size() - numBytesBeforeRead;
}

int CliConnection::dataLengthForCurrentDataType(){
    if(this->bytesAvailable() <= 0 || readDataIntoBuffer(MaxBufferSize) <= 0
            || !buffer.endsWith(SeparatorToken))
        return 0;

    this->buffer.chop(1);
    int length = buffer.toInt();
    this->buffer.clear();
    return length;
}

bool CliConnection::readProtocolHeader(){
    if(this->transferTimerId != 0){
        this->killTimer(this->transferTimerId);
        this->transferTimerId = 0;
    }

    if(this->readDataIntoBuffer() <= 0){
        this->transferTimerId = this->startTimer(TransferTimeout);
        return false;
    }

    if(buffer == "MESSAGE ")
        this->currentDataType = MESSAGE;
    else if(buffer == "GREETING " ){
        this->currentDataType = GREETING;
    }else
    {
        this->currentDataType = UNDEFINED;
        this->abort();
        return false;
    }

    buffer.clear();
    this->numBytesForCurrentDataType = this->dataLengthForCurrentDataType();

    return true;
}

bool CliConnection::hasEnoughData(){
    if(this->transferTimerId){
        this->killTimer(transferTimerId);
        transferTimerId = 0;
    }

    if(numBytesForCurrentDataType <= 0)
        numBytesForCurrentDataType = this->dataLengthForCurrentDataType();

    if(bytesAvailable() < numBytesForCurrentDataType
            || numBytesForCurrentDataType <= 0)
    {
        this->transferTimerId = this->startTimer(TransferTimeout);
        return false;
    }
    return true;
}

//
void CliConnection::processReadyRead(){
    if(this->state == WaitingForGreeting ){
        if(!this->readProtocolHeader())
            return;
        if(this->currentDataType != GREETING ) {
            this->abort();
            return;
        }
        this->state = ReadingGreeting;
    }

    if(this->state == ReadingGreeting ) {
        if(!this->hasEnoughData())
            return;

        this->buffer = read(this->numBytesForCurrentDataType);

        if(buffer.size() != this->numBytesForCurrentDataType){
            this->abort();
            return;
        }

        QString greeting = QString::fromUtf8(this->buffer);

        if(DEBUG)
            qDebug() << "Get greeting message: " << greeting;
        QStringList addrList = greeting.split(":");

        this->userName = addrList.at(0);
        QString ipStr = addrList.at(1);
        QString protStr = addrList.at(2);

        this->ip = QHostAddress(ipStr);
        this->port = protStr.toInt();

        this->currentDataType = UNDEFINED;
        this->numBytesForCurrentDataType = 0;
        this->buffer.clear();

        if(!this->isValid()){
            this->abort();
            return;
        }

        if(!this->isGreetingMessageSent)
            this->sendGreetingMessage();

        this->state = ReadyForUse;
        emit this->endGreeting(this);
    }

    do {
        if(this->currentDataType == UNDEFINED) {
            if(!readProtocolHeader())
                return;
        }

        if(!this->hasEnoughData())
            return;

        this->processData();

    }while( this->bytesAvailable()>0);
}

void CliConnection::processData(){
    this->buffer = this->read(this->numBytesForCurrentDataType);

    if(buffer.size() != this->numBytesForCurrentDataType){
        this->abort();
        return;
    }

    switch (currentDataType) {
    case MESSAGE:
        emit newMessage(this->userName,QString::fromUtf8(buffer));
        break;
    default:
        break;
    }

    this->currentDataType = UNDEFINED;
    this->numBytesForCurrentDataType = -1;
    this->buffer.clear();
}
