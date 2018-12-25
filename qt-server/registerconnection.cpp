#include "registerconnection.h"

static const int TransferTimeout = 40 * 1000;
static const char SeparateToken = '  ';

RegisterConnection::RegisterConnection(QObject *parent) : QTcpSocket(parent)
{
	this->greetingMessage = QString("Register Validation");
	this->registerMessage = QString("");
	this->state = WaitingForGreeting;
	this->currentDataType = UNDEFINED;
	this->isGreetingMessageSent = false;
	this->numBytesForCurrentDataType = -1;
	this->transferTimerId = 0;

	connect(this,SIGNAL(readyRead()),this,SLOT(processReadyRead()));

	connect(this,SIGNAL(connected()),this,SLOT(sendGreetingMessage()));
	connect(this,SIGNAL(error(QAbstractsocket::SocketError)),this,SLOT(handleSocketError(QAbstractSocket::SocketError)));
}

RegisterConnection::~RegisterConnection(){

}

QString RegisterConnection::getPeerServerName(){
	return this->name;
}

QHostAddress RegisterConnection::getPeerServerIP(){
	return this->ip;
}

int RegisterConnection::getPeerServerPort(){
	return this->port;
}

void RegisterConnection::handleSocketError(QAbstractSocket::SocketError error) {
	emit this->registerError(this,error);
}

void RegisterConnection::timerEvent(QTimerEvent *timerEvent) {
	if(timerEvent->timerId() == this->transferTimerId){
		this->abort();
		this->killTimer(transferTimerId);
		transferTimerId = 0;
	}

}

void RegisterConnection::sendGreetingMessage(){
	QByteArray greet = this->greetingMessage.toUtf8();
	QByteArray msg = "GREETING " + QByteArray::number(greet.size()) + ' ' + greet;
	if(this->write(msg) == msg.size())
		this->isGreetingMessageSent = 1;
}
