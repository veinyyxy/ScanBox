#pragma once
#include <qthread.h>
#include <QLocalSocket>
#include <QMutex>

#define BufferSize 1024 * 1024 * 2

typedef struct __INVOKE_DATA__
{
	qint32 type;
	qint32 param_type;
	char name[40];
	char param[BufferSize];
	char result[BufferSize];
} INVOKE_DATA, *P_INVOKE_DATA;

class ClientThread : public QThread
{
	Q_OBJECT
public:
	virtual void run();
	ClientThread();
	QVariant invokeFuntion(char* funName, char* param, size_t len, int iConnectTimeout = 30000, int iWriteTimeout = 30000, int iReadTimeout = 30000);
	~ClientThread();
	void connectServer();
	inline QString& GetRemoveInvokeError() { return m_strError; }
	inline void writeLogging(bool bState) { m_bWriteLogging = bState; }
public slots:
	void slot_readyRead();
	void slot_connected();
	void slot_disconnected();
	void slot_error(QLocalSocket::LocalSocketError socketError);
	void slot_stateChanged(QLocalSocket::LocalSocketState socketState);
private:
	QLocalSocket localSocket;
	QMutex m_callLock;
	QString m_strError;
	bool m_bWriteLogging = false;
	QString m_strLoggingPath;
};

