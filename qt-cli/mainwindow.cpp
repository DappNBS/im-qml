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
