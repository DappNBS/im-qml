#ifndef PEERINFO_H
#define PEERINFO_H

#include "header.h"
#include <QObject>

class PeerInfo : public QObject {
	Q_OBJECT

public:
	explicit PeerInfo(QObject *parent = nullptr,QString peername = "unknow", 
			QHostAddress ip = QHostAddress("127.0.0.1"), int port = 0);

	~PeerInfo();

    QString     getPeerName();
    QString     getPeerIP();
    int         getPeerPort();

    void        setPeerInfo(QString,QHostAddress,int);
    void        refresh();
    void        clear();

signals:
	void peerExpire(PeerInfo *);

public slots:
    void handleTimeOut();

private:
    QString         peerName;
	QHostAddress	peerIP;
    int             peerPort;
    bool            state;

    QTimer          *activeTimer;

};
#endif //PEERINFO_H /END
