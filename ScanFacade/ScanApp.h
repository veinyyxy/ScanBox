#pragma once
#include <QAbstractNativeEventFilter>
#include <QApplication>
#include <QtCore>
#include "TWAINSession.h"
#include "DSMInterface.h"

class ScanApp : public QApplication, public CTWAINSession
{
	Q_OBJECT

public:
	ScanApp(int &argc, char **argv, int = QT_VERSION);
	TW_UINT16 GetDataSourceID(const QString& pszProductName, TW_IDENTITY & twIdentity);
	/////////////////////////////////////////////////////
	bool aquireDSMList(QStringList * dsmList);
	bool selectCurrentDS(const QString& strName);
	bool getCurrentDS();
	void modifyCurrentDSName(const QString& strName);
	bool showSetupDialog(TW_HANDLE handle);
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
	
	void writeScanConfigure(const QString& strFile);
	////////////////////////////////////////////////////////////////////////////////////
	int newScan(TW_HANDLE refMem);
	QStringList& replaceScan();
	QStringList& insertScan(bool isBehind = false);
	void packageImages();
	QString& perviousImage();
	QString& nextImage();
	void SaveConfig();
	void LoadConfig();
	int clearScan(bool delDir = true);
	int deleteImage(const QString & strIndexes);
	int deleteImage(int startIndex, int iCount);
	int UploadFile(const QString & strFile);
	int getFileIndex(const QString& strFile);
	void adjustFile(int front, int back);
	int getCurrentIndex();
	int setCurrentIndex(int iIndex);
	int getFileCount();
	inline int customFileFormat(const QString& extendName) { m_CustomFileExtendName = extendName; }
	int setScanFolder(QString strPath);
	QString getFileInfo(int iIndex);
	QString getFileInfos(int start, int count);

	void getFileList(QStringList& stringList);
	int insertExternalFile(const QString& strPath, bool isBehind = false, bool isCopy = true);
	
	int insertExternalFiles(const QStringList& strFilesList, bool isBehind = false, bool isCopy = true);
	int replaceCurrentFile(const QString& strPath, bool isCopy = true);
	//int writeTextToImage(int iIndex, int x, int y, uint cololr, uint size, double angle, bool isBold, const QString& font, const QString& text);
	//int drawImageToImage(int iIndex, int x, int y, const QString& strWatermark);
	QString getUUIDFileName(const QString& strExt);
	QString getFileFullName(int iIndex);
	inline QString getCurrentDSName() { return m_currentDSName; }
	//////////////////////////////////////////////////////////////////////////////////////
	virtual ~ScanApp();
	
signals:
	void signal_saveFileFinish(QString CurrentFilePath);
	void customDataSourceSignal();
public slots:
	void DataSourceSignal();

private:
	int FtpUpLoad(const char * URL, const char *fileName, const char* userpwd);
	int HttpUpload(const char * URL, const char *fileName);
	bool DeleteDirectory(const QString &path);
	QString m_ScanConfigFileName = "ScanBox.ini";
	QStringList m_FileConfig;
	QString m_FilePath;
	TW_UINT16 m_twMemoryFileFormat;
	QString m_TWAIN_FileExtendName = ".bmp";
	QString m_CustomFileExtendName = ".jpg";
	QString m_currentDSName;
	TW_HANDLE m_parentHandle;
	int m_iCurrentIndex = 0;
	int m_iNextIndex = 0;
	QString m_NullFile = "";
	bool m_bFirstNext = true;

	QStringList m_scanResult;
};

class NativeEventFilterZ : public QAbstractNativeEventFilter
{

public:
	virtual bool nativeEventFilter(const QByteArray &eventType, void *pMsg, long *result) Q_DECL_OVERRIDE
	{
		bool bRC = false;
		ScanApp *pApp = dynamic_cast<ScanApp*>(QApplication::instance());
		
		if (pApp && (5 <= pApp->GetCurrentState()))
		{
			TW_EVENT twEvent = { pMsg, MSG_NULL };
			if (TWRC_DSEVENT == pApp->ProcessEvent(twEvent))
			{
				bRC = true;
			}
		}
		return bRC;
	}
};