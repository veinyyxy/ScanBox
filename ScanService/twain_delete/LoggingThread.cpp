#include "pch.h"
#include "LoggingThread.h"
#include <QDir>
#include <QTextStream>

LoggingThread::LoggingThread(const QString& fileFullName) : m_fileFullName(fileFullName), m_shareLoggingText("")
{
}

LoggingThread::~LoggingThread()
{

}

void LoggingThread::run()
{
	while (1)
	{
		QString localText = "";
		m_mutex.lock();
		localText = m_shareLoggingText;
		m_shareLoggingText = "";
		

		QFile loggingfile(m_fileFullName + QDir::separator() + "ScanService.log");

		if (loggingfile.open(QIODevice::Append | QIODevice::Text))
		{
			QTextStream out(&loggingfile);
			out << localText << endl;

			loggingfile.close();
		}

		m_mutex.unlock();

		m_controlMutex.lock();
		if (true == m_ExitState)
		{
			break;
		}
		m_controlMutex.unlock();

	}
}

void LoggingThread::writeLogging(const QString & strText)
{
	m_mutex.lock();
	m_shareLoggingText = strText;
	m_mutex.unlock();
}

void LoggingThread::closeLoggingThread()
{
	m_controlMutex.lock();
	m_ExitState = true;
	m_controlMutex.unlock();
}
