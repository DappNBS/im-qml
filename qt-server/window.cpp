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

Window::~Window(){

}
//通过网卡接口读取本机的所有的IP地址
void Window::networkInterfaceCombo() {
	QList<QHostAddress> list = QNetworkInterface::allAddresses();

	foreach(QHostAddress address,list){
		if(address.protocol() == QAbstractSocket::IPv4Protocol)
			this->ipBox->addItem(address.toString());
	}
}

void Window::initLayout() {
	QVBoxLayout *listLayout = new QVBoxLayout;
	listLayout->addWidget(this->listLabel);
	listLayout->addWidget(this->registerListWidget);
	
	QHBoxLayout *ipLayout = new QHBoxLayout;
	ipLayout->addWidget(this->ipLabel);
	ipLayout->addWidget(this->portBox);
	ipLayout->setStretch(0,1);
	ipLayout->setStretch(1,3);

	QHBoxLayout *portLayout = new QHBoxLayout;
	portLayout->addWidget(this->portLabel);
	portLayout->addWidget(this->portBox);
	portLayout->setStretch(0,1);
	portLayout->setStretch(1,3);


	QVBoxLayout *logLayout = new QVBoxLayout;
	logLayout->addWidget(this->logLabel);
	logLayout->addWidget(this->logRegister);
	logLayout->addLayout(ipLayout);
	logLayout->addLayout(portLayout);

	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch();
	buttonLayout->addWidget(this->startButton);
	buttonLayout->addWidget(this->stopButton);

	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->addLayout(hLayout);
	hLayout->addLayout(logLayout);
	hLayout->setStretch(0,2);
	hLayout->setStretch(1,3);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(hLayout);
	mainLayout->addLayout(buttonLayout);
	mainLayout->setStretch(1,8);
	mainLayout->setStretch(2,1);

	QWidget *widget = new QWidget(this);
	widget->setLayout(mainLayout);

	this->setCentralWidget(widget);
}

//
void Window::startNetwork() {
	QString ip = this->ipBox->currentText();
	this->serverIP = QHostAddress(ip);
	this->serverPort = this->portBox->value();

	if(DEBUG){
		qDebug() << "Register server ip address: " << this->serverIP.toString();
		qDebug() << "Register server port: " << this->serverPort;
	}

	this->server->setServerInfo(this->serverIP,this->serverPort);
	if(this->server->startServer()) {
		this->startButton->setEnabled(false);
		this->ipBox->setEnabled(false);
		this->portBox->setEnabled(false);
		this->stopButton->setEnabled(true);
		this->appendLogInfo("Register server started.";
		
		if(DEBUG)
			qDebug() << "Registerserver started.";
	}
}
