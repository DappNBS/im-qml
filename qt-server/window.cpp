#include "window.h"

Window::Window(QWidget *parent) : QMainWindow(parent) {
	this->listLabel = new QLabel(QObject::tr("<b>Register Host List:</b>"),this);

	this->registerListWidget = new QListWidget(this);

	this->logLabel = new QLabel(QObject::tr("<b>Register Log:</b>"),this);

	this->ipLabel = new QTextEdit(this);

	this->ipLabel = new QLabel(QObject::tr("Server IP:"),this);
	this->ipBox = new QComboBox(this);
	this->ipLabel->setBuddy(this->ipBox);
	this->portLabel = new QLabel(QObject::tr("Server Port:"),this);
	this->portBox = new QSpinBox(this);
	this->portLabel->setBuddy(this->portBox);

	this->networkInterfacesCombo();
	this->portBox->setRange(1024,65535);
	this->portBox->setValue(10001);

	this->startButton = new QPushButton(QObject::tr("Start"),this);
	this->stopButton = new QPushButton(QObject::tr("Stop"),this);
	this->logRegister->setReadOnly(true);
	this->startButton->setFocusPolicy(Qt::StrongFocus);
	this->stopButton->setEnabled(false);

	this->peerList.clear();
	this->registerConns.clear();
	this->serverIP = QHostAddress("127.0.0.1");
	this->serverPort = 10001;
	this->server = new RegisterServer();

	this->initLayout();
	this->rsize(QSize(550,360));

	connect(startButton,SIGNAL(clicked()),this,SLOT(startNetwork()));
	connect(stopButton,SIGNAL(clicked()),this,SLOT(stopNetwork()));
}



