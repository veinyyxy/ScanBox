#pragma once
#include <qthread.h>
#include <QLocalSocket>
#include <QMutex>

typedef struct __INVOKE_DATA__
{
	qint32 type;
	qint32 param_type;
	char name[40];
	char param[256];
	char result[512];
} INVOKE_DATA, *P_INVOKE_DATA;

class ClientThread : public QThread
{
	Q_OBJECT
public:
	virtual void run();
	ClientThread();
	QVariant invokeFuntion(char* funName, char* param);
	~ClientThread();
	void connectServer();
public slots:
	void slot_readyRead();
	void slot_connected();
	void slot_disconnected();
	void slot_error(QLocalSocket::LocalSocketError socketError);
	void slot_stateChanged(QLocalSocket::LocalSocketState socketState);
private:
	QLocalSocket localSocket;
	QMutex m_callLock;
};

