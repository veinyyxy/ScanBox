#pragma once
#include <qthread.h>
#include <QMutex>

class LoggingThread :
	public QThread
{
public:
	LoggingThread(const QString& fileFullName);
	~LoggingThread();
	virtual void run();
	void writeLogging(const QString& strText);
	void closeLoggingThread();
private:
	QString m_fileFullName;
	QMutex m_mutex;
	QMutex m_controlMutex;
	QString m_shareLoggingText;
	bool m_ExitState = false;
};

