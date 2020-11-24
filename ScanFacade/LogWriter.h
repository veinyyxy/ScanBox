#pragma once
#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QFile>
#include <QTextStream>

class LogWriter :
	public QThread
{
	Q_OBJECT
public:
	enum LOG_MODE { LOCALLOGGING, FILELOGGING };
	struct INOF_LOG
	{
		LOG_MODE lm;
		QString name;
		QString text;
	};

	
	LogWriter();
	~LogWriter();
	inline void Active(bool bA) { m_Stop = bA; }
	void WrtieLogging(const INOF_LOG& strLogging);
	void WrtieLogging(const QString& name, const QString& text, const LOG_MODE& lm);
	virtual void run();
signals:
	void LogMessage(const QString strMessage);

private:
	QMutex m_logWriteMutex;
	QMutex m_logQueueMutex;
	bool m_Stop = false;
	int m_iQueueSize = 10000;
	QQueue<INOF_LOG> m_LoggingQueue;
};

