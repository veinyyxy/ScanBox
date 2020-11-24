#include "ScanApp.h"
#include "ScanFacade.h"
#include <QtWidgets/QApplication>
#include <QStyleFactory>
#include <QProcess>
#include <QAlgorithms.h>
#include <LogWriter.h>

LogWriter* g_logWriter = new LogWriter;
bool g_bRemoteEnable = false;

void writeLoging(LPCCH strFormat, ...)
{
	FILE *fp = NULL;
	fopen_s(&fp, "D:\\ScanFacade.log", "a");
	///////////////////////////////////////////////////
	int _Result;
	va_list _ArgList;
	va_start(_ArgList, strFormat);
	_Result = _vfprintf_s_l(fp, strFormat, NULL, _ArgList);
	va_end(_ArgList);
	//return _Result;
	///////////////////////////////////////////////////
	fclose(fp);
}

int main(int argc, char *argv[])
{
#if 1
	/*QStringList environment = QProcess::systemEnvironment();
	QString path;
	foreach(path, environment)
	{
		if (path.startsWith("QTDIR="))
		{
			path = path.mid(6);
			break;
		}
	}
	QApplication::addLibraryPath(path + "\\plugins");*/
	//DebugBreak();
	//writeLoging("%d \r\n", argc);
	//writeLoging("%s \r\n", argv[0]);
	//writeLoging("%s \r\n", argv[1]);
	//writeLoging("%s \r\n", argv[2]);
	//writeLoging("%s \r\n", argv[3]);
	//__debugbreak();

	g_logWriter->start();
	ScanApp a(argc, argv);
	QStringList keysList = QStyleFactory::keys();
	a.setStyle(QStyleFactory::create("Fusion"));
	ScanFacade w;
	
	w.SetApp(&a);
	w.restoreConfig();
	w.connectAllSignalsToSlots();

	for (int i = 0; i < argc; i++)
	{
		QString argString = QString::fromLocal8Bit(argv[i]);
		if (argString == QString::fromLocal8Bit("start"))
		{
			w.slot_startServer();
		}
		else if (argString == QString::fromLocal8Bit("hide"))
		{
			w.hide();
		}
		else
		{
			w.show();
			w.slot_startServer();
		}
	}
	
	/*if (argc == 3)
	{
		w.slot_startServer();
		if (!w.isListen()) return -1;
		w.hide();
	}
	else
	{
		w.show();
		w.slot_startServer();
	}*/

	return a.exec();

	g_logWriter->Active(false);
#else
	int m_iCurrentIndex = 3;
	QStringList m_FileConfig;
	m_FileConfig.push_back("0");
	m_FileConfig.push_back("1");
	m_FileConfig.push_back("2");
	m_FileConfig.push_back("3");
	QStringList tail;
	tail = m_FileConfig.mid(m_iCurrentIndex);
	m_FileConfig = m_FileConfig.mid(0, m_iCurrentIndex);
	m_FileConfig.push_back("-2");
	m_FileConfig.push_back("-1");
	m_FileConfig += tail;
#endif
}
