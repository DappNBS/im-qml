#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include "header.h"
#include "cliconnection.h"
#include <QDialog>


class ChatDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ChatDialog(QWidget *parent = nullptr,QString username="localhost",
                        QString peername="unknown",QString peeraddr = "null",
                        CliConnection *connection = nullptr);

    ~ChatDialog();

    void    readAndDisplayHistory(QList<QString>);

signals:
    void    dialogExit(CliConnection *);

public slots:
    void    handleReceivedMessage(QString from,QString message);
    void    handleToSendMessage();
    void    handleConnectionError(QAbstractSocket::SocketError);
    void    handleRejected();

private:

    void    initLayout();

    QLabel          *peerName;
    QLabel          *peerAddress;
    QTextEdit       *logChat;
    QTextLine       *msgSend;
    QPushButton     *sendButton;

    QString         userName;
    CliConnection   *cliSession;
};

#endif // CHATDIALOG_H
