#pragma once
#include <QAbstractNativeEventFilter>
#include <QApplication>
#include <QtCore>
#include "TWAINSession.h"
#include "DSMInterface.h"
#include "LoggingThread.h"

class ScanApp : public QApplication, public CTWAINSession
{
	Q_OBJECT

public:
	ScanApp(int& argc, char **argv, int = QT_VERSION);
	/////////////////////////////////////////////////////
	bool aquireDSMList(QStringList * dsmList);
	bool selectCurrentDS(QString strName);
	bool showSetupDialog();
	virtual void OnSignalDSRequest();
	virtual void OnError(const TW_STATUS & twStatus) override;
	virtual void OnNativeTransfer(const TW_MEMREF & pData, bool & bAbort, bool & bFree) override;
	virtual void OnMemoryTransfer(const TW_IMAGEINFO & twInfo, const TW_IMAGEMEMXFER & twData, const TW_UINT16 & twRC, bool & bAbort) override;
	virtual void OnFileTransfer(const TW_SETUPFILEXFER & twFileXfer, bool & bAbort) override;
	virtual void OnFileMemTransfer(const TW_IMAGEMEMXFER & twData, const TW_UINT16 & twRC, bool & bAbort) override;
	virtual void OnImageBegin(const TW_IMAGEINFO &twInfo, TW_UINT16 twXferMech);
	////////////////////////////////////////////////////
	inline pTW_PENDINGXFERS GetPendingXfers() { return &m_twPendingXfers; }
	void doScan(TW_HANDLE refMem);
	void writeLogging(LPSTR text);
	void closeThread();
	void CreateScanConfig();
	virtual ~ScanApp();
	
signals:
	void signal_saveFileFinish(QString CurrentFilePath);
private:
	
	QString m_FilePath = "C://Windows//Temp//ScanBox";
	TW_UINT16 m_twMemoryFileFormat;
	QString m_FileExtendName = ".bmp";
	QString m_currentDSName = "";
	LoggingThread m_loggingOut;
	int itemp = 0;
};

class NativeEventFilterZ : public QAbstractNativeEventFilter
{

public:
	virtual bool nativeEventFilter(const QByteArray &eventType, void *pMsg, long *result) Q_DECL_OVERRIDE
	{
		bool bRC = false;
		ScanApp *pApp = dynamic_cast<ScanApp*>(QApplication::instance());
		//First make sure we have the correct type of instance, there is a window and in the right TWAIN state
		if (pApp && (5 <= pApp->GetCurrentState()))
		{
			//Initialize the event structure
			TW_EVENT twEvent = { pMsg, MSG_NULL };
			//Dispatch the even to the Data Source
			if (TWRC_DSEVENT == pApp->ProcessEvent(twEvent))
			{
				//The event was handled by the DataSource
				bRC = true;
			}
		}
		//The event was not handled by the DataSource
		return bRC;
	}
};