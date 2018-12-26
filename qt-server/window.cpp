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
	}else {
		QMessageBox::waring(this,tr("Error"),tr("Cannot start register server!"));
		this->appendLogInfo("Cannot start register server.");

		if(DEBUG)
			qDebug() << "Cannot start register server.";
		return;
	}

	connect(this->server,SIGNAL(newRegisterConnection(RegisterConnection *)),this,SLOT(handleNewConnection(RegisterConnection *)));
}

//停止注册中心服务器
void Window::stopNetwork() {
	if(DEBUG)
		qDebug() << "Register server stopped!";
	
	this->appendLogInfo("Register server stopped.";
	this->server->close();
	for(int i =0 ; i<this->registerConns.size();i++)
	{
		RegisterConnection *c = this->registerConns.at(i);
		c->close();
	}

	//Widget update
	this->registerListWidget->clear();
	this->startButton->setEnabled(true);
	this->ipBox->setEnabled(true);
	this->portBox->setEnabled(true);
	this->stopButton->setEnabled(false);
}

//处理新建立的连接，即当有远端客户端发起注册请求时，建立TCP连接，发送注册信息
void Window::handleNewConnection(RegisterConnection * connection) {
	if(DEBUG)
		qDebug() << "Handle new connection.";
	this->addRegisterConnection(connection);

	connect(connection,SIGNAL(newRegisterMsg(RegisterConnection * ,QString)),
			this,SLOT(handleNewRegisterMsg(RegisterConnection *,QString)));
	connect(connection,SIGNAL(registerError(RegisterConnection *,QAbstractSocket::SocketError)),
			this,SLOT(handleRegisterError(RegisterConnection * ,QAbstractSocket::SocketError)));
}

//处理新的注册信息,如果注册信息更新，则更新相应的表项
void Window::handleNewRegisterMsg(RegisterConnection * connection,QString message) {
	if(DEBUG)
	{
		qDebug() << "Enter handle_new_register_msg function: ";
		qDebug() << "New register message : " << message;
	}

	QStringList infoList = message.split("@");
	QString hostName = infoList.at(0);
	QString addr = infoList.at(1);
	
	QStringList addrList = addr.split(":");
	QString ipStr = addrList.at(0);
	QHostAddress ip = QHostAddress(ipStr);
	QString portStr = addrList.at(1);
	int port = portStr.toInt();

	if(DEBUG)
	{
		qDebug() << "Register message infomation:";
		qDebug() << "HostName: " << hostName;
		qDebug() << "IP Address: " << ipStr;
		qDebug() << "Port:" << portStr;
	}

	PeerInfo * pi = findPeer(hostName,ip,port);

	if(pi)
	{
		pi->refresh();
		this->appendLogInfo("Activate exist register: " + message);
		if(DEBUG)
			qDebug() << "Active timer of PeerInfo reset.";
	}else {
		pi = new PeerInfo(this,hostName,ip,port);
		this->addPeerInfoIntoList(pi);

		connect(pi,SIGNAL(peerExpire(PeerInfo *)),this,SLOT(handlePeerTimeout(PeerInfo *)));

		//刷新Register的列表
		this->updateRegisterListWidget();
		this->appendLogInfo("New Register: " + message);

		//发送新的PeerList到远端
		this->broadcastPeerChange();
	}

	if(DEBUG)
		qDebug() << "Handle new register msg done.";

}

//处理PeerList 表项过期Peer
void Window::handlePeerTimeout(PeerInfo * pi){
	if(DEBUG)
		qDebug() << "PeerInfo expired: " << pi->getPeerName();
	this->appendLogInfo("PeerInfo expired : " + pi->getPeerName());

	RegisterConnection *c = this->findConnection(pi);

	this->removePeerInfoFromList(pi);
	this->removeRegisterConnection(c);
	this->broadcastPeerChange();
	this->updateRegisterListWidget();

	if(DEBUG)
		qDebug() << "Handle expire done.";
}

//处理注册client断线异常
void Window::handleRegisterError(RegisterConnection *connection,QAbstractSocket::SocketError error) {
	
}
