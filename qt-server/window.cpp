#include "window.h"

//窗口界面
Window::Window(QWidget *parent) : QMainWindow(parent) {
    //左侧server list
    this->listLabel             = new QLabel(QObject::tr("<b>Register Host List:</b>"),this);
    //
    this->registerListWidget    = new QListWidget(this);
    //右侧label
    this->logLabel              = new QLabel(QObject::tr("<b>Register Log:</b>"),this);
    this->logRegister           = new QTextEdit(this);

    //
    this->ipLabel               = new QLabel(QObject::tr("Server IP:"),this);
    this->ipBox                 = new QComboBox(this);

	this->ipLabel->setBuddy(this->ipBox);

    this->portLabel             = new QLabel(QObject::tr("Server Port:"),this);
    this->portBox               = new QSpinBox(this);
	this->portLabel->setBuddy(this->portBox);

	this->networkInterfacesCombo();
	this->portBox->setRange(1024,65535);
	this->portBox->setValue(10001);

    this->startButton           = new QPushButton(QObject::tr("Start"),this);
    this->stopButton            = new QPushButton(QObject::tr("Stop"),this);
	this->logRegister->setReadOnly(true);
	this->startButton->setFocusPolicy(Qt::StrongFocus);
	this->stopButton->setEnabled(false);

	this->peerList.clear();
	this->registerConns.clear();
    this->serverIP              = QHostAddress("127.0.0.1");
    this->serverPort            = 10001;
    this->server                = new RegisterServer();

	this->initLayout();
    this->resize(QSize(800,550));

	connect(startButton,SIGNAL(clicked()),this,SLOT(startNetwork()));
	connect(stopButton,SIGNAL(clicked()),this,SLOT(stopNetwork()));
}

Window::~Window(){
    this->server->close();
}

//通过网卡接口读取本机的所有的IP地址
void Window::networkInterfacesCombo() {
	QList<QHostAddress> list = QNetworkInterface::allAddresses();

	foreach(QHostAddress address,list){
		if(address.protocol() == QAbstractSocket::IPv4Protocol)
			this->ipBox->addItem(address.toString());
	}
}

//初始化窗口布局
void Window::initLayout() {
    QVBoxLayout *listLayout = new QVBoxLayout; //
    listLayout->addWidget(this->listLabel); //
	listLayout->addWidget(this->registerListWidget);
	
    //IP Hor
	QHBoxLayout *ipLayout = new QHBoxLayout;
	ipLayout->addWidget(this->ipLabel);
    ipLayout->addWidget(this->ipBox);
	ipLayout->setStretch(0,1);
	ipLayout->setStretch(1,3);

    //port
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

    //水平控件
	QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addLayout(listLayout);
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
        this->appendLogInfo("Register server started.",this->serverIP,this->serverPort);
		
		if(DEBUG)
            qDebug() << "Registerserver started. on ["
                     << this->serverIP.toString() << ":" << this->serverPort << "].";
	}else {
        QMessageBox::warning(this,tr("Error"),tr("Cannot start register server!"));
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
	
    this->appendLogInfo("Register server stopped.");
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
void Window::handleNewRegisterMsg(RegisterConnection * connection ,QString message) {
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
	if(DEBUG)
		qDebug() << "Register connection network error.";

	QString errorString;
	switch(error)
	{
	case QAbstractSocket::ConnectionRefusedError:
		errorString = "Connection refused!";
		break;
	case QAbstractSocket::RemoteHostClosedError:
		errorString = "Remote host closed.";
		break;
	case QAbstractSocket::HostNotFoundError:
		errorString = "Host not found!";
		break;
	case QAbstractSocket::NetworkError:
		errorString = "Network error.";
		break;
	case QAbstractSocket::AddressInUseError:
		errorString = "Address already in use.";
		break;
    case QAbstractSocket::UnknownSocketError:
	default:
		errorString = "Remote host closed.";
		break;
	}
	if(DEBUG)
		qDebug() << "Register connection error: " << errorString;

	this->appendLogInfo("Connection Error:" + errorString);

	PeerInfo *pi = this->findPeer(connection->getPeerServerName(),connection->getPeerServerIP(),connection->getPeerServerPort());

	this->appendLogInfo("Delete register: " + pi->getPeerName());
	this->removePeerInfoFromList(pi);
	pi->clear();

	this->removeRegisterConnection(connection);
	this->broadcastPeerChange();
	this->updateRegisterListWidget();

	if(DEBUG)
		qDebug() << "Handle network error done.";
}

//更新ListWidget控件信息
void Window::updateRegisterListWidget() {
	this->registerListWidget->clear();
	for(int i=0; i < this->peerList.size();i++)
	{
		PeerInfo *pi = this->peerList.at(i);
 		QString itemString = 
			pi->getPeerName() + "@" + pi->getPeerIP() + ":" 
			+ QString::number(pi->getPeerPort());

		this->registerListWidget->addItem(new QListWidgetItem(itemString));
							
	}

}

PeerInfo * Window::findPeer(QString name,QHostAddress ip,int port) {
	for(int i = 0 ;i < this->peerList.size(); i++) {
		PeerInfo *p = this->peerList.at(i);
		if(p->getPeerName() == name && 
				p->getPeerIP() == ip.toString() &&
				p->getPeerPort() == port)
			return p;
	}
    return nullptr;
}

RegisterConnection * Window::findConnection(PeerInfo * pi){
	qDebug() << "Enter findConnection.";
	for(int i=0 ; i < this->registerConns.size();i++) {
		RegisterConnection *c = this->registerConns.at(i);
		if(c->getPeerServerName() == pi->getPeerName() 
				&& c->getPeerServerIP().toString() == pi->getPeerIP() 
				&& c->getPeerServerPort() == pi->getPeerPort())
			return c;
	}
    return nullptr;
}

QString Window::genPeerListStr() {
	if(DEBUG)
		qDebug() << "Generate PeerList String.";

	QString peerListStr("");
	for(int i = 0; i < this->peerList.size();i++)
	{
		PeerInfo *p = this->peerList.at(i);
		QString peerStr = p->getPeerName() + "@"
			+ p->getPeerIP() + ":"
			+ QString::number(p->getPeerPort()) + ";";
		peerListStr += peerStr;
	}

	peerListStr.chop(1);

	return peerListStr;
}


void Window::broadcastPeerChange() {
	if(DEBUG)
		qDebug() << "Broadcast peerList changed message";

	QString str = this->genPeerListStr();

	for(int i = 0;i < this->registerConns.size(); i++)
	{
		RegisterConnection * c = this->registerConns.at(i);
		c->sendPeriodicPeerlistMessage(str);
	}

	if(DEBUG)
		qDebug() << "Add new connection into registerConns.";	
}

void Window::addRegisterConnection(RegisterConnection *connection) {
    this->mutex2.lock();
    this->registerConns.push_back(connection);
    this->mutex2.unlock();

    if(DEBUG)
        qDebug() << "Add new connection into registerConns.";
}

void Window::removeRegisterConnection(RegisterConnection *connection)
{
	this->mutex2.lock();
	this->registerConns.removeOne(connection);
	connection->deleteLater();
	this->mutex2.unlock();

	if(DEBUG)
		qDebug() << "Remove connection from registerConns.";
}

void Window::addPeerInfoIntoList(PeerInfo *pi) {
	this->mutex1.lock();
	this->peerList.removeOne(pi);
	this->mutex1.unlock();

	if(DEBUG)
		qDebug() << "Remove PeerInfo from peerList" << pi->getPeerName();
}

void Window::removePeerInfoFromList(PeerInfo *pi){
    this->mutex1.lock();
    this->peerList.removeOne(pi);
    this->mutex1.unlock();

    if(DEBUG)
        qDebug() << "Remove PeerInfo from peerList " << pi->getPeerName();
}

void Window::appendLogInfo(QString msg)
{
	QDateTime time = QDateTime::currentDateTime();
	QString timeString = time.toString("yyyy-MM-dd hh:mm:ss");
	this->logRegister->append(timeString + " >\n" +msg);
}

void Window::appendLogInfo(QString msg,QHostAddress serIP,int serPort) {
    QDateTime time = QDateTime::currentDateTime();
    QString timeString = time.toString("yyyy-MM-dd hh:mm:ss");
    this->logRegister->append(timeString
                              + ">\n"+msg +
                              "no [" + serIP.toString() +
                              ":" + QString::number(serPort) + "]");
}


