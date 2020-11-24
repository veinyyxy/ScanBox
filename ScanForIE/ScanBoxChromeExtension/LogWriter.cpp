#include "pch.h"
#include "LogWriter.h"


LogWriter::LogWriter()
{
}

LogWriter::~LogWriter()
{
}

void LogWriter::WrtieLogging(const INOF_LOG& strLogging)
{
	m_logQueueMutex.lock();
	if (m_LoggingQueue.size() <= m_iQueueSize)
	{
		m_LoggingQueue.append(strLogging);
	}
	m_logQueueMutex.unlock();
}

void LogWriter::WrtieLogging(const QString& name, const QString& text, const LOG_MODE& lm)
{
	INOF_LOG il;
	il.name = name;
	il.text = text;
	il.lm = lm;

	WrtieLogging(il);
}

void LogWriter::run()
{
	while (!m_Stop)
	{
		m_logQueueMutex.lock();
		int iSize = m_LoggingQueue.size();

		if (iSize == 0)
			msleep(300);

		for (int i = 0; i < iSize; i++)
		{
			INOF_LOG il = m_LoggingQueue.at(i);

			if (il.lm == LOCALLOGGING)
			{
				emit LogMessage(il.text);
			}
			else
			{
				QFile logFile(il.name);

				if (logFile.open(QIODevice::Append | QIODevice::Text))
				{
					QTextStream out(&logFile);

					out << il.text << endl;
				}

				logFile.close();
			}
		}

		m_LoggingQueue.clear();
		m_logQueueMutex.unlock();
	}
}
