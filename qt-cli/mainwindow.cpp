#include "mainwindow.h"
/*
 * #include "ui_mainwindow.h"
 */

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    this->myName = new QLabel(QObject::tr("Username: none"),this);
    this->myAddress = new QLabel(QObject::tr("Address:client unregisted."),this);

    this->infoLabel = new QLabel(QObject::tr("Active peer list:"),this);

    this->peerListWidget = new QListWidget();

    this->cliButton = new QPushButton(QObject::tr("new Chat"),this);
    this->registerButton = new QPushButton(QObject::tr("Register"),this);
    this->checkButton = new QPushButton(QObject::tr("Recv Chat"),this);
    this->remoteServerLabel = new QLabel("Not set register server info.");

    this->server = new CliServer(this);
    this->registerConnection = new RegisterConnection(this);

    this->initLayout();
    this->setWindowTitle(QObject::tr("Chat Room"));
    this->resize(QSize(340,500));

    this->setMinimumSize(QSize(280,320));

    connect(this->cliButton,SIGNAL(clicked())
            ,this,SLOT(handleActiveConnection()));
    connect(this->registerButton,SIGNAL(clicked())
            ,this,SLOT(handleRegisterAction()));
    connect(this->checkButton,SIGNAL(clicked())
            ,this,SLOT(handleCheckDialog()));

}

MainWindow::~MainWindow()
{
    this->server->close();
    this->registerConnection->close();
}

void MainWindow::initLayout(){
    QVBoxLayout *myTextLayout = new QVBoxLayout;
    myTextLayout->addWidget(myName);
    myTextLayout->addWidget(myAddress);

    QHBoxLayout *cliLayout = new QHBoxLayout;
    cliLayout->addWidget(registerButton);
    cliLayout->addStretch();
    cliLayout->addWidget(checkButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->addLayout(myTextLayout);
    mainLayout->addWidget(infoLabel);
    mainLayout->addWidget(peerListWidget);
    mainLayout->addLayout(cliLayout);
    mainLayout->addWidget(remoteServerLabel);

    QWidget *widget = new QWidget(this);
    widget->setLayout(mainLayout);

    this->setCentralWidget(widget);
}

void MainWindow::initNetwork(){
    //
    if(DEBUG)
    {
        qDebug() << "Local server ip address: " << this->localServerIP.toString();
        qDebug() << "Local server port: " << this->localServerPort;
    }

    this->server->setCliSeverInfo(this->localServerIP,this->localServerPort);

    if(this->server->startServer()){
        QString address = this->localServerIP.toString() + ":"
                + QString::number(this->localServerPort);

        this->myName->setText("Username: " +this->userName);
        this->myAddress->setText("Address: " +address);

        QString remoteAddress = QString("Register Server Info: ")
                + this->remoteServerIP.toString() + ":"
                + QString::number(this->remoteServerPort);

        this->registerButton->setText(remoteAddress);

        this->registerButton->setEnabled(false);

        if(DEBUG)
            qDebug() << "Local chat server started.";
    }else{
        this->remoteServerLabel->setText("Cannot start local chat server.");

        this->registerButton->setEnabled(true);

        QMessageBox::warning(this,tr("ERROR"),tr("Cannot start local chat server!"));

        if(DEBUG)
            qDebug() << "Cannot start local chat server.";

        return;
    }

    connect(server,SIGNAL(newCliConnection(CliConnection*)),this,
            SLOT(handlePassiveConnection(CliConnection*)));

    //连接注册中心注册服务器
    if(DEBUG)
        qDebug() << "Connect to register server.";
    this->registerConnection->setLocalServerInfo(this->userName,this->localServerIP,this->localServerPort);
    this->registerConnection->connectToHost(this->remoteServerIP,this->remoteServerPort);

    if(DEBUG)
        qDebug() << "Connected to register server.";
    connect(registerConnection,SIGNAL(newPeerList(QString&))
            ,this,SLOT(handleNewPeerList(QString &)));
    connect(registerConnection,SIGNAL(error(QAbstractSocket::SocketError))
            ,this,SLOT(QAbstractSocket::SocketError));
}

void MainWindow::handleRegisterAction() {
    Logindialog *ld = new Logindialog(this);
    if(ld->exec() != QDialog::Accepted){
        QMessageBox::warning(this,tr("Warning"),tr("Chat register canceled!"));
        return;
    }

    this->userName = ld->getUserName();
    this->localServerIP = ld->getLocalServerIP();
    this->localServerPort = ld->getLocalServerPort();
    this->remoteServerIP = ld->getRegisterServerIP();
    this->remoteServerPort = ld->getRegisterServerPort();

    if(DEBUG)
    {
        qDebug() << "Register client configuration done!";
        qDebug() << "Local server name:" << this->userName;
        qDebug() << "Local server IP address:" << this->localServerIP;
        qDebug() << "Local server port:" << this->localServerPort;
    }

    this->initNetwork();
}

void MainWindow::updateListWidget(){
    this->peerListWidget->clear();

    for(int i = 0; i < this->peerList.size();i++){
        QString comment = "";
        PeerInfo *pi = this->peerList.at(i);
        QString name = pi->getPeerName();

        if(name == this->userName)
            comment = "  ( local )";
        if(this->tempMsgs.contains(name))
            comment = "  ( new message: " + QString::number(this->tempMsgs[name].size())
                    +")";
        QString itemString = pi->getPeerName()
                + "@" + pi->getPeerIP() + ":"
                + QString::number(pi->getPeerPort()) + comment;

        this->peerListWidget->addItem(new QListWidgetItem(itemString));
    }
}

void MainWindow::handleNewPeerList(QString &peerListString){
    this->peerList.clear();

    if(peerListString.isEmpty())
        return;

    QString username,comment;
    QHostAddress ip;
    int port;

    QStringList peerInfoList = peerListString.split(";");

    foreach(QString peerInfo,peerInfoList){
        comment = QString("");
        QStringList items = peerInfo.split("@");
        username = items.at(0);
        QString addr = items.at(1);

        QStringList addrList = addr.split(":");

        QString ipString = addrList.at(0);
        ip = QHostAddress(ipString);
        QString portString = addrList.at(1);
        port = portString.toInt();

        PeerInfo *pinfo = new PeerInfo(this,username,ip,port);
        this->peerList.push_back(pinfo);
    }

    this->updateListWidget();
}

void MainWindow::handlePassiveConnection(CliConnection *connection){
    if(DEBUG)
        qDebug() << "New passive chat connection.";

    QString greeting = this->userName + ":" + this->localServerIP.toString()
            +":" + QString::number(this->localServerPort);

    connection->setGreetingMessage(greeting);

    this->addCliConnection(connection);

    connect(connection,SIGNAL(socketError(CliConnection*,QAbstractSocket::SocketError)),
            this,SLOT(handleCliConnectionError(CliConnection*,QAbstractSocket::SocketError)));

    connect(connection,SIGNAL(endGreeting(CliConnection*)),this,
            SLOT(handlePassiveNewCli(CliConnection *)));
}

//激活链接
void MainWindow::handleActiveConnection(){
    if(DEBUG)
        qDebug() << "New active chat connection.";

    QListWidgetItem *item = this->peerListWidget->currentItem();
    if(!item){
        QMessageBox::warning(this,tr("Warning"),tr("Please select one peer."));

        if(DEBUG)
            qDebug() << "Select no peer to connect.";
        return;
    }

    QString connInfo = item->text();

}
