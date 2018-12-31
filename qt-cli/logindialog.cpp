#include "logindialog.h"

//登录Dialog
Logindialog::Logindialog(QWidget *parent) : QDialog (parent)
{
    this->loginLogo             = new QLabel(QObject::tr("Welcome NBS Chat Client"),this);
    this->loginLogo->setAlignment(Qt::AlignCenter);

    this->loginUsername         = new QLabel(QObject::tr("Username:"),this);
    this->editUsername          = new QLineEdit(this);

    this->localIPAddress        = new QLabel(QObject::tr("Local IP:"),this);
    this->localIPAddressBox     = new QComboBox(this);

    this->localPort             = new QLabel(QObject::tr("Local Port:"),this);
    this->localPortBox          = new QSpinBox(this);

    this->registerIpLabel       = new QLabel(QObject::tr("Server IP:"),this);
    this->registerIpEdit        = new QLineEdit(this);

    this->registerPortLabel     = new QLabel(QObject::tr("Server Port:"),this);
    this->registerPortBox       = new QSpinBox(this);

    this->registerIpLabel->setBuddy(this->registerIpEdit);
    this->registerPortLabel->setBuddy(this->registerPortBox);

    this->loginUsername->setBuddy(this->editUsername);
    this->localIPAddress->setBuddy(this->localIPAddressBox);
    this->localPort->setBuddy(this->localPortBox);

    this->networkInterfacesCombo();

    this->localPortBox->setRange(1024,65535);
    this->localPortBox->setValue(9527);
    this->registerPortBox->setRange(1024,65535);
    this->registerPortBox->setValue(100001);

    this->editUsername->setText("unknown");
    this->registerIpEdit->setText("192.168.103.99");//co IP

    //Button setting
    this->doneButton            = new QPushButton(QObject::tr("Register"),this);
    this->cancelButton          = new QPushButton(QObject::tr("Cancel"),this);

    this->doneButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    this->cancelButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    this->initLayout();
    this->resize(QSize(600,450));
    this->setMinimumSize(QSize(300,220));
    this->setWindowTitle(QObject::tr("NBS Chat Client"));

    connect(doneButton,SIGNAL(clicked()),this,SLOT(on_doneButton_clicked()));
    connect(this,SIGNAL(clicked()),this,SLOT(on_cancelButton_clicked()));

}


Logindialog::~Logindialog(){

}

//
void Logindialog::networkInterfacesCombo(){
    QList<QHostAddress> list= QNetworkInterface::allAddresses();

    foreach(QHostAddress address,list){
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
            this->localIPAddressBox->addItem(address.toString());
    }
}

void Logindialog::initLayout(){
    //layout manager
    QHBoxLayout *userLayout = new QHBoxLayout;
    userLayout->addWidget(this->loginUsername);
    userLayout->addWidget(this->editUsername);

    userLayout->setStretch(0,1);
    userLayout->setStretch(1,3);

    //
    QHBoxLayout *ipLayout = new QHBoxLayout;
    ipLayout->addWidget(this->localIPAddress);
    ipLayout->addWidget(this->localIPAddressBox);
    ipLayout->setStretch(0,1);
    ipLayout->setStretch(1,3);

    QHBoxLayout *portLayout = new QHBoxLayout;
    portLayout->addWidget(this->localPort);
    portLayout->addWidget(this->localPortBox);
    portLayout->setStretch(0,1);
    portLayout->setStretch(1,3);

    QHBoxLayout *svrIpLayout = new QHBoxLayout;
    svrIpLayout->addWidget(this->registerIpLabel);
    svrIpLayout->addWidget(this->registerIpEdit);
    svrIpLayout->setStretch(0,1);
    svrIpLayout->setStretch(1,3);

    QHBoxLayout *svrPortLayout = new QHBoxLayout;
    svrPortLayout->addWidget(this->registerPortLabel);
    svrPortLayout->addWidget(this->registerPortBox);
    svrPortLayout->setStretch(0,1);
    svrPortLayout->setStretch(1,3);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(this->doneButton);
    btnLayout->addStretch();
    btnLayout->addWidget(this->cancelButton);
    btnLayout->addStretch();

    //main layout UI manager
    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->addWidget(this->loginLogo);

    mainLayout->addLayout(userLayout);
    mainLayout->addLayout(ipLayout);
    mainLayout->addLayout(portLayout);
    mainLayout->addLayout(svrIpLayout);
    mainLayout->addLayout(svrPortLayout);
    mainLayout->addStretch();

    mainLayout->addLayout(btnLayout);

    mainLayout->setStretch(0,2);
    mainLayout->setStretch(1,1);
    mainLayout->setStretch(2,1);
    mainLayout->setStretch(3,1);
    mainLayout->setStretch(4,1);
    mainLayout->setStretch(5,1);
    mainLayout->setStretch(6,1);

    this->setLayout(mainLayout);
}

QString Logindialog::getUserName(){
    QString username = this->editUsername->text();
    if(username.isEmpty())
        username = "unknown";

    return username;
}

QString Logindialog::getLocalServerIP(){
    QString ipAddress = this->localIPAddressBox->currentText();
    if(ipAddress.isEmpty())
        ipAddress = "localhost";

    return ipAddress;
}

int Logindialog::getLocalServerPort(){
    return this->localPortBox->value();
}

QString Logindialog::getRegisterServerIP(){
    QString serverIP = this->registerIpEdit->text();
    if(serverIP.isEmpty())
        serverIP = "192.168.103.99";
    return serverIP;
}


int Logindialog::getRegisterServerPort(){
    return this->registerPortBox->value();
}

void Logindialog::on_cancelButton_clicked(){
    this->reject();
}

void Logindialog::on_doneButton_clicked(){
    this->accept();
}
