#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "header.h"
#include "cliconnection.h"
#include "registerconnection.h"
#include "cliserver.h"
#include "logindialog.h"
#include "chatdialog.h"
#include "peerinfo.h"

#include <QMainWindow>

/*
namespace Ui {
class MainWindow;
}
*/

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:

public slots:
    void            handleRegisterAction();
    void            handleNewPeerList(QString &);
    void            handleActiveConnection();
    void            handleCheckDialog();
    void            handlePassiveConnection(CliConnection *);
    void            handleCliConnectionError(CliConnection *,QAbstractSocket::SocketError);

    void            handleRegisterError(QAbstractSocket::SocketError);
    void            handleNewChatDialog(CliConnection *);
    void            handlePassiveMsgRecv(QString,QString);
    void            handlePassiveNewCli(CliConnection *);

private:
    void            initLayout();
    void            initNetwork();
    void            updateListWidget();

    //connections
    void            addCliConnection(CliConnection *);
    void            removeCliConnection(CliConnection *);
    CliConnection * findCliConnection(QString name,QString ip);

    //gui widgets
    QLabel          * myName;
    QLabel          * myAddress;
    QLabel          * infoLabel;

    QListWidget     * peerListWidget;
    QPushButton     * cliButton;
    QPushButton     * registerButton;
    QPushButton     * checkButton;
    QLabel          * remoteServerLabel;

    //Variables
    QString         userName;
    QHostAddress    localServerIP;
    int             localServerPort;
    QHostAddress    remoteServerIP;
    int             remoteServerPort;

    //connections
    QMap<QString,QList<QString>>    tempMsgs;           //记录被动连接的接收信息

    QMap<QString,ChatDialog *>      cliMap;             //记录已打开的客户聊天窗口
    QList<PeerInfo *>               peerList;           //记录活跃的远端主机

    QList<CliConnection *>          connList;           //管理所有聊天连接
    CliServer                       *server;            //本地聊天连接监听服务器
    RegisterConnection              *registerConnection;//与中心服务器相连接


    //generate by Qt IDE
    //Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
