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
}
