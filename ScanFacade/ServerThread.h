#pragma once
#include <qthread.h>
#include <QLocalSocket>
#include <QLocalServer>
#include <QDataStream>
//#pragma pack(push)
//#pragma pack(16)
//struct test
//{
//	char m1;
//	int m3;
//	double m4;
//};
//#pragma pack(pop)
#define BufferSize 1024 * 1024 * 2
typedef struct __INVOKE_DATA__
{
	qint32 type;
	qint32 param_type;
	char name[40];
	char param[BufferSize];
	char result[BufferSize];
} INVOKE_DATA, *P_INVOKE_DATA;

class InvokeInterface;

class ServerThread : public QThread
{
	Q_OBJECT

signals:
	QVariant invokeFuntion(char* name, char* param);

public:
	ServerThread();
	virtual void run();
	ServerThread(InvokeInterface* pII);
	bool startListen();
	void connectClientSignals();
	
	~ServerThread();
public slots:
	void slot_newConnection();
	void slot_connected();
	void slot_disconnected();
	void slot_error(QLocalSocket::LocalSocketError socketError);
	void slot_stateChanged(QLocalSocket::LocalSocketState socketState);
	void slot_readyRead();

	//QVariant slot_remoteInvoke(char* name, char* param);
private:
	QLocalSocket* localSocket = nullptr;
	QLocalServer localServer;
	//InvokeInterface* m_pInvokeInterface = nullptr;
};

