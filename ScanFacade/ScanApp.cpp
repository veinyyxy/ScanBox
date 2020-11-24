#include "ScanApp.h"
#include <QUuid>
#include <curl/curl.h>
#include <QPixmap>
#include <QtGui/5.10.1/QtGui/private/qzipwriter_p.h>
ScanApp::ScanApp(int & argc, char ** argv, int nVer)
	: QApplication(argc, argv, nVer)
{
	this->installNativeEventFilter(new NativeEventFilterZ());
	setScanFolder(QString::fromLocal8Bit("C:\\Windows\\Temp\\ScanBox"));

	connect(this, SIGNAL(customDataSourceSignal()), this, SLOT(DataSourceSignal()), Qt::QueuedConnection);
}

TW_UINT16 ScanApp::GetDataSourceID(const QString& pszProductName, TW_IDENTITY &twIdentity)
{
	TW_UINT16 twRC = TWRC_FAILURE;
	if (3 <= m_uiState)
	{
		bool bFound = false;
		TW_IDENTITY twDataSource = { 0 };
		//Iterate through all the DataSources
		if (TWRC_SUCCESS == (twRC = DSM_Entry(DG_CONTROL, DAT_IDENTITY, MSG_GETFIRST, &twDataSource)))
		{
			do
			{
				std::string ca = pszProductName.toStdString();
				if (0 == strncmp(twDataSource.ProductName, ca.c_str(), sizeof(twDataSource.ProductName)))
				{
					//DataSource was found, copy the information and stop looking
					twIdentity = twDataSource;
					twRC = TWRC_SUCCESS;
					bFound = true;
					break;
				}
			} while (TWRC_SUCCESS == (twRC = DSM_Entry(DG_CONTROL, DAT_IDENTITY, MSG_GETNEXT, &twDataSource)));
		}
		//last check
		if (false == bFound)
		{
			//the DS was not found
			twRC = TWRC_FAILURE;
			m_twLastStatus.ConditionCode = TWCC_BADDEST;
		}
	}
	else
	{
		//This is a sequence error
		m_twLastStatus.ConditionCode = TWCC_SEQERROR;
		twRC = TWRC_FAILURE;

		//Trace out the result
		TraceMessage("CloseDSM Failed / Result = %d, Status = %d", twRC, m_twLastStatus.ConditionCode);
	}
	return twRC;
}

bool ScanApp::aquireDSMList(QStringList* dsmList)
{
	TW_IDENTITY twDataSource;
	//Populate with all the connected data sources
	if (TWRC_SUCCESS == DSM_Entry(DG_CONTROL, DAT_IDENTITY, MSG_GETFIRST, &twDataSource))
	{
		do
		{
			dsmList->append(twDataSource.ProductName);
		} while (TWRC_SUCCESS == DSM_Entry(DG_CONTROL, DAT_IDENTITY, MSG_GETNEXT, &twDataSource));
	}

	return true;
}

bool ScanApp::selectCurrentDS(const QString& strName)
{
	TW_IDENTITY twIdentity = { 0 };
	m_currentDSName = strName;
	//Get the entire identity from the DSM
	if (TWRC_SUCCESS == GetDataSourceID(m_currentDSName, twIdentity))
	{
		//Instruct the DSM to make this the default
		DSM_Entry(DG_CONTROL, DAT_IDENTITY, MSG_SET, &twIdentity);
		return true;
	}
	else
	{
		m_currentDSName = "";
		return false;
	}
}

bool ScanApp::getCurrentDS()
{
	TW_IDENTITY twDataSource;
	//Update with the current default data source
	if (TWRC_SUCCESS == DSM_Entry(DG_CONTROL, DAT_IDENTITY, MSG_GETDEFAULT, &twDataSource))
	{
		m_currentDSName = QString::fromLocal8Bit(twDataSource.ProductName);
		return true;
	}
	else
		return false;
}

void ScanApp::modifyCurrentDSName(const QString & strName)
{
	m_currentDSName = strName;
}

bool ScanApp::showSetupDialog(TW_HANDLE handle)
{
	EnableDS(handle, true);
	return false;
}

void ScanApp::OnSignalDSRequest()
{
	emit customDataSourceSignal();
}

void ScanApp::OnError(const TW_STATUS & twStatus)
{
}

void ScanApp::OnNativeTransfer(const TW_MEMREF & pData, bool & bAbort, bool & bFree)
{
	TW_HANDLE hImage = *static_cast<TW_HANDLE*>(pData);
	if (hImage)
	{
		//lock the handle and extract the pointer
		PBITMAPINFOHEADER pDIB = static_cast<PBITMAPINFOHEADER>(_DSM_LockMemory(hImage));
		
		if (pDIB)
		{
			QUuid uuidCreater = QUuid::createUuid();
			QString strUUIDName = uuidCreater.toString();
			
			QDir rootPath(m_FilePath);
			if (!rootPath.exists())
			{
				setScanFolder("");
			}

			QString strDestName = m_FilePath + QDir::separator() + strUUIDName + m_CustomFileExtendName;

			if (strDestName.length())
			{
				if (QFile::exists(strDestName))
				{
					//remove the original file
					QFile::remove(strDestName);
				}

				//attempt to re-create the file
				QFile img(strDestName);
#if 0
				//Open the new file for writing
				if (img.open(QIODevice::ReadWrite))
				{
					//Calculate the offset
					int nOffSet = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*pDIB->biClrUsed);
					//Calculate the size of the DIB
					int nDIBSize = sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*pDIB->biClrUsed) + pDIB->biSizeImage;
					if (BI_RGB == pDIB->biCompression)
					{
						//Note: Calculate the size of the image manually when uncompressed - do not trust the biSizeImage member
						nDIBSize = sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*pDIB->biClrUsed) + ((((pDIB->biWidth * pDIB->biBitCount + 31) / 32) * 4) * pDIB->biHeight);
					}
					//Prepare a stream for writing
					QDataStream ds(&img);
					//Initialize the BitmapFileHeader structure
					BITMAPFILEHEADER bmpFIH;
					memset(&bmpFIH, 0, sizeof(BITMAPFILEHEADER));
					bmpFIH.bfType = ((WORD)('M' << 8) | 'B');
					bmpFIH.bfOffBits = nOffSet;
					bmpFIH.bfSize = sizeof(BITMAPFILEHEADER) + nDIBSize;
					//Write the BitmapFileHeader to disk
					ds.writeRawData(reinterpret_cast<char *>(&bmpFIH), sizeof(BITMAPFILEHEADER));
					//Write the Bitmap to disk
					ds.writeRawData(reinterpret_cast<char *>(pDIB), nDIBSize);
					//Close the file
					img.close();
					/*
					QImage image("D:\\myScan.bmp");

					QGraphicsPixmapItem *pixmapItem = m_graphicsScene.addPixmap(QPixmap::fromImage(image));
					*/
				}
				else
				{
					qDebug() << strDestName;
				}
#else
				//Calculate the offset
				int nOffSet = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*pDIB->biClrUsed);
				//Calculate the size of the DIB
				int nDIBSize = sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*pDIB->biClrUsed) + pDIB->biSizeImage;
				if (BI_RGB == pDIB->biCompression)
				{
					//Note: Calculate the size of the image manually when uncompressed - do not trust the biSizeImage member
					nDIBSize = sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*pDIB->biClrUsed) + ((((pDIB->biWidth * pDIB->biBitCount + 31) / 32) * 4) * pDIB->biHeight);
				}
				//Prepare a stream for writing
				QDataStream ds(&img);
				//Initialize the BitmapFileHeader structure
				BITMAPFILEHEADER bmpFIH;
				memset(&bmpFIH, 0, sizeof(BITMAPFILEHEADER));
				bmpFIH.bfType = ((WORD)('M' << 8) | 'B');
				bmpFIH.bfOffBits = nOffSet;
				bmpFIH.bfSize = sizeof(BITMAPFILEHEADER) + nDIBSize;
				
				QByteArray pixmapData;
				pixmapData.append(reinterpret_cast<char *>(&bmpFIH), sizeof(BITMAPFILEHEADER));
				pixmapData.append(reinterpret_cast<char *>(pDIB), nDIBSize);
				QPixmap TempPixmap;
				if (TempPixmap.loadFromData(pixmapData, "BMP"))
				{
					TempPixmap.save(strDestName);
				}
#endif
				if (img.exists())
				{
					//QString formatString("%1-%2");
					//QString tempConifg = formatString.arg(m_iFileNumber).arg(strDestName);
					
					m_FileConfig.push_back(strDestName);
					
					emit signal_saveFileFinish(strDestName);
					m_iCurrentIndex = m_FileConfig.size() - 1;
				}
			}
			//unlock the handle
			_DSM_UnlockMemory(hImage);
		}
	}
}

void ScanApp::OnMemoryTransfer(const TW_IMAGEINFO & twInfo, const TW_IMAGEMEMXFER & twData, const TW_UINT16 & twRC, bool & bAbort)
{
}

void ScanApp::OnFileTransfer(const TW_SETUPFILEXFER & twFileXfer, bool & bAbort)
{
}

void ScanApp::OnFileMemTransfer(const TW_IMAGEMEMXFER & twData, const TW_UINT16 & twRC, bool & bAbort)
{
}

void ScanApp::OnImageBegin(const TW_IMAGEINFO & twInfo, TW_UINT16 twXferMech)
{
	if (TWSX_MEMFILE != twXferMech)
	{
		switch (twInfo.Compression)
		{
		case TWCP_NONE:
			//uncompressed images will be stored as .bmp
			m_twMemoryFileFormat = TWFF_BMP;
			m_TWAIN_FileExtendName = ".bmp";
			break;
		case TWCP_JPEG:
			//jpeg compressed images will be stored as a .jpg
			m_twMemoryFileFormat = TWFF_JFIF;
			m_TWAIN_FileExtendName = ".jpg";
			break;
		case TWCP_GROUP4:
			//Group4 compressed images will be stored as .tif
			m_twMemoryFileFormat = TWFF_TIFF;
			m_TWAIN_FileExtendName = ".tif";
			break;
		case TWCP_PNG:
			m_twMemoryFileFormat = TWFF_PNG;
			m_TWAIN_FileExtendName = ".png";
			break;
		}
	}
	else
	{
		//This is file in memory transfer more, file format is stored in the capability
		GetCapabilityOneValue(ICAP_IMAGEFILEFORMAT, m_twMemoryFileFormat);
	}
}

void ScanApp::doScan(TW_HANDLE refMem)
{
	m_parentHandle = refMem;
	TW_UINT16 result = TWRC_FAILURE;
	result = LoadDSM();
	if (TWRC_SUCCESS == result)
	{
		//WId hWnd = this->winId();
		result = OpenDSM((TW_MEMREF)refMem);
		if (TWRC_SUCCESS == result)
		{
			QStringList list;
			aquireDSMList(&list);

			if (list.count() > 0)
			{
				if(m_currentDSName.length() > 0)
					selectCurrentDS(m_currentDSName);
				else
				{
					m_currentDSName = list[0];
					selectCurrentDS(m_currentDSName);
				}

				if (TWRC_SUCCESS == OpenDS())
				{
					result = EnableDS(refMem, false);

					if (TWRC_SUCCESS == result)
					{
						//启用源成功
					CONTINUE_DoTransfe:
						result = DoTransfer();
						if (TWRC_SUCCESS == result)
						{
							DoEndXfer();
							TW_UINT16 u16Count = GetPendingXfers()->Count;
							if (u16Count >= 0)
							{
								goto CONTINUE_DoTransfe;
							}
							//扫描成功
						}

						DoEndXfer();
						DisableDS();
						CloseDS();
						CloseDSM();

					}
					else
					{

					}
				}
				else
				{
					//数据源打开失败
				}
			}
			else
			{
				//没有获得数据源
			}
		}
	}
	else
	{
		//没有打开数据源
	}

	SaveConfig();
}

void ScanApp::DataSourceSignal()
{
	//it is absolutely critical that we never process signals while the DSM is on the stack
	if (!m_bInTWAIN)
	{
		//figure out what signal was requested
		switch (GetDSRequest())
		{
		case MSG_NULL:
			break;
		case MSG_XFERREADY:
			if (false)
			{
				//Automatically transfer all the images
				while ((TWRC_SUCCESS == DoTransfer()) && (TWRC_SUCCESS == DoEndXfer()) && (6 == m_uiState));
			}
			//Remove this signal from the queue
			ClearDSRequest();
			break;
		case MSG_CLOSEDSREQ:
			if (true)
			{
				DisableDS();
			}
			//Remove this signal from the queue
			ClearDSRequest();
			break;
		case MSG_CLOSEDSOK:
			DisableDS();
			//Remove this signal from the queue
			ClearDSRequest();
			break;
		case MSG_DEVICEEVENT:
			//Remove this signal from the queue
			ClearDSRequest();
			break;
		}
	}
	else
	{
		
	}
	
	return;
}

void ScanApp::writeScanConfigure(const QString & strFile)
{

}

bool ScanApp::DeleteDirectory(const QString &path)
{
	if (path.isEmpty())
	{
		return false;
	}

	QDir dir(path);
	if (!dir.exists())
	{
		return true;
	}

	dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
	QFileInfoList fileList = dir.entryInfoList();
	foreach(QFileInfo fi, fileList)
	{
		if (fi.isFile())
		{
			fi.dir().remove(fi.fileName());
		}
		else
		{
			DeleteDirectory(fi.absoluteFilePath());
		}
	}
	return dir.rmpath(dir.absolutePath());
}

int ScanApp::newScan(TW_HANDLE refMem)
{
	m_iCurrentIndex = 0;
	m_bFirstNext = true;
	doScan(refMem);

	if (getFileCount() > 0)
	{
		return 1;
	}
	else
		return 0;
}

QStringList& ScanApp::replaceScan()
{
	m_scanResult.clear();

	int iBackupCurrentIndex = m_iCurrentIndex;
	QStringList backConfig = m_FileConfig;
	m_FileConfig.clear();
	doScan(m_parentHandle);
	int delIndex = iBackupCurrentIndex;
	m_scanResult = m_FileConfig;

	if (m_FileConfig.size() >= backConfig.size() - iBackupCurrentIndex + 1)
	{
		QVector<int> m_deleIndex;
		for (; delIndex < backConfig.size(); delIndex++)
		{
			m_deleIndex.push_back(delIndex);
			QString strdelFileName = backConfig[delIndex];
			QFile delFile(strdelFileName);
			delFile.remove();
		}

		delIndex = iBackupCurrentIndex;
		for (int i = 0; i < m_deleIndex.size(); i++)
		{
			backConfig.removeAt(delIndex);
		}
		
		m_FileConfig = backConfig + m_FileConfig;
		m_iCurrentIndex = m_FileConfig.size() - 1;
		m_bFirstNext = true;
	}
	else
	{
		for (; delIndex < m_FileConfig.size() + iBackupCurrentIndex; delIndex++)
		{
			QString strdelFileName = backConfig[delIndex];
			QFile delFile(strdelFileName);
			delFile.remove();
		}
		delIndex = iBackupCurrentIndex;
		
		for (int iNewIndex = 0; iNewIndex < m_FileConfig.size(); delIndex++, iNewIndex++)
		{
			backConfig[delIndex] = m_FileConfig.at(iNewIndex);
		}
		
		m_iCurrentIndex = iBackupCurrentIndex + m_FileConfig.size() - 1;
		m_FileConfig = backConfig;
		m_bFirstNext = true;
	}

	return m_scanResult;
}

QStringList& ScanApp::insertScan(bool isBehind)
{
	m_scanResult.clear();

	QStringList head, tail;
	int countIndex = m_iCurrentIndex;
	if (isBehind)
		countIndex += 1;

	tail = m_FileConfig.mid(countIndex);
	head = m_FileConfig.mid(0, countIndex);
	m_FileConfig.clear();
	doScan(m_parentHandle);
	m_scanResult = m_FileConfig;

	m_FileConfig = head + m_FileConfig + tail;

	return m_scanResult;
}

void ScanApp::packageImages()
{
}

QString& ScanApp::perviousImage()
{
	if (m_FileConfig.size() < 1) return m_NullFile;

	m_iCurrentIndex--;
	if (m_iCurrentIndex < 0) m_iCurrentIndex = m_FileConfig.size() - 1;
	m_NullFile = m_FileConfig[m_iCurrentIndex];

	return m_NullFile;
}

QString& ScanApp::nextImage()
{
	if (m_FileConfig.size() < 1) return m_NullFile; 
	if (m_bFirstNext)
	{
		m_NullFile = m_FileConfig[m_iCurrentIndex];
		m_bFirstNext = false;
	}
	else
	{
		m_iCurrentIndex = ++m_iCurrentIndex % m_FileConfig.size();
		m_NullFile = m_FileConfig[m_iCurrentIndex];
	}
	
	return m_NullFile;
}

void ScanApp::SaveConfig()
{
	QSettings saveConfigure(m_FilePath + QDir::separator() + m_ScanConfigFileName, QSettings::IniFormat);
	saveConfigure.clear();

	saveConfigure.beginGroup("DefaultDevice");
	saveConfigure.setValue(QString::fromLocal8Bit("Dev"), m_currentDSName);
	saveConfigure.setValue(QString::fromLocal8Bit("Count"), m_FileConfig.count());
	saveConfigure.endGroup();

	for (int i = 0; i < m_FileConfig.length(); i++)
	{
		saveConfigure.beginGroup("Default");
		saveConfigure.setValue(QString("%1").arg(i), m_FileConfig.at(i));
		saveConfigure.endGroup();
	}
}

void ScanApp::LoadConfig()
{
	int fileCount = 0;
	m_FileConfig.clear();
	QSettings loadConfigure(m_FilePath + QDir::separator() + m_ScanConfigFileName, QSettings::IniFormat);
	loadConfigure.beginGroup("DefaultDevice");
	m_currentDSName = loadConfigure.value("Dev", "").toString();
	fileCount = loadConfigure.value("Count", "").toInt();
	loadConfigure.endGroup();

	loadConfigure.beginGroup("Default");
	//QStringList keys = loadConfigure.childKeys();
	//QString strKey = "";
	for(int i = 0; i < fileCount; i++)
	{
		QString strKey = QString("%1").arg(i);
		QString strValue = loadConfigure.value(strKey, "").toString();
		m_FileConfig.push_back(strValue);
	}

	loadConfigure.endGroup();
	m_iCurrentIndex = 0;
	m_bFirstNext = true;
}

int ScanApp::clearScan(bool delDir)
{
	m_FileConfig.clear();

	bool bResult = DeleteDirectory(m_FilePath);

	if (bResult)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	//QDir dir(m_FilePath);
	//QFileInfoList list = dir.entryInfoList();
	//if (list.length() != 0)
	//{

	//	for (int i = 0; i < list.size(); ++i)
	//	{
	//		QFileInfo *fileInfo = &list[i];
	//		QString strFile = fileInfo->absoluteFilePath();
	//		if (fileInfo->isFile())
	//		{
	//			QFile file(fileInfo->absoluteFilePath());
	//			file.remove();
	//		}
	//		else if (fileInfo->isDir())
	//		{
	//			//QDir subDir(fileInfo->absoluteFilePath());
	//			//subDir.removeRecursively();

	//		}
	//	}
	//}

	//if (delDir)
	//{
	//	dir.rmdir(m_FilePath);
	//	if (dir.exists())
	//	{
	//		dir.rmdir(m_FilePath);
	//	}
	//}
}

int ScanApp::deleteImage(const QString& strIndexes)
{
	QStringList newFileConfig;
	int iR = 0;
	QStringList strList = strIndexes.split("|");
	QVector<bool> removeList(m_FileConfig.count());
	removeList.fill(false);
	for (int i = 0; i < strList.size(); ++i)
	{
		bool bR;
		int index = strList[i].toInt(&bR);
		if (bR)
		{
			if (index >= 0 && index < m_FileConfig.count())
			{
				QString strFile = m_FileConfig[index];
				QFile file(strFile);
				iR = file.remove();

				removeList[index] = true;
			}
		}
	}

	for (int i = 0; i < removeList.count(); i++)
	{
		if (!removeList[i])
		{
			newFileConfig.append(m_FileConfig[i]);
		}
	}

	m_FileConfig = newFileConfig;

	return iR;
}

int ScanApp::deleteImage(int startIndex, int iCount)
{
	int iR = 0;
	if ((0 > startIndex || startIndex >= m_FileConfig.size()) || iCount > (m_FileConfig.size() - startIndex + 1) || iCount < 1)
	{
		return 0;
	}

	m_iCurrentIndex = startIndex - 1;

	for (int i = 0; i < iCount; i++)
	{
		QString strFile = m_FileConfig[startIndex + i];
		QFile file(strFile);
		iR = file.remove();
	}

	for (int i = 0; i < iCount; i++)
	{
		m_FileConfig.removeAt(startIndex);
	}

	return iR;
}

#define QStringToCharPtr(qs) qs.toLocal8Bit().data()

int ScanApp::UploadFile(const QString & strFile)
{
	QSettings ftpSettings("ftp.ini", QSettings::IniFormat);
	QString strHost = ftpSettings.value("default/host", "").toString();
	QString strPassword = ftpSettings.value("default/password", "").toString();
	QString strUser = ftpSettings.value("default/user", "").toString();
	int port1 = ftpSettings.value("default/port1", "").toInt();
	int port2 = ftpSettings.value("default/port2", "").toInt();

	QFileInfo fileInfo(strFile);
	QString strFileFullName = fileInfo.fileName();
	QString strUrlForcurl = QString::fromLocal8Bit("ftp://") + strHost + ":" + QString("%1").arg(port1) + "/" + strFileFullName;
	QString strUserpwd = strUser + QString::fromLocal8Bit(":") + strPassword;

	QString strZipFileFullName = m_FilePath + QDir::separator() + strFileFullName;
	QFile deleteTempFile(strZipFileFullName);
	if (deleteTempFile.exists())
	{
		deleteTempFile.remove();
	}
	QZipWriter zipWriter(strZipFileFullName);
	//zipWriter.addDirectory("./");
	for (int i = 0; i < m_FileConfig.size(); i++)
	{
		QString strFile = m_FileConfig[i];
		QFile addedFile(strFile);
		QFileInfo fileInfo(strFile);
		addedFile.open(QIODevice::ReadOnly);
		zipWriter.addFile(fileInfo.fileName(), addedFile.readAll());//添加文件
		addedFile.close();
	}
	zipWriter.close();

	int curlCode = FtpUpLoad(QStringToCharPtr(strUrlForcurl), QStringToCharPtr(strZipFileFullName), QStringToCharPtr(strUserpwd));

	deleteTempFile.remove();

	return curlCode;
}

int ScanApp::getFileIndex(const QString & strFile)
{
	return m_FileConfig.indexOf(strFile);
}

void ScanApp::adjustFile(int front, int back)
{
	QString strMe = m_FileConfig.at(front);
	m_FileConfig.removeAt(front);
	m_FileConfig.insert(back, strMe);
}

int ScanApp::getCurrentIndex()
{
	return m_iCurrentIndex;
}

int ScanApp::setCurrentIndex(int iIndex)
{
	if (0 > iIndex || iIndex >= m_FileConfig.size())
	{
		return 0;
	}

	m_iCurrentIndex = iIndex;
	return 1;
}

int ScanApp::getFileCount()
{
	return m_FileConfig.size();
}

int ScanApp::setScanFolder(QString strPath)
{
	if(strPath.length() != 0)
		m_FilePath = strPath;

	if (m_FilePath.length() != 0)
	{
		QDir dir(m_FilePath);
		if (!dir.exists())
		{
			if (dir.mkpath(m_FilePath))
			{
				return 1;
			}
			else
				return -1;//文件路劲设置失败
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return -1;//文件路劲设置失败
	}
}

QString ScanApp::getFileInfo(int iIndex)
{
	if (m_FileConfig.size() == 0)
	{
		return "";
	}

	if (iIndex >= m_FileConfig.size() || iIndex < 0)
		return "";

	QString strFile = m_FileConfig[iIndex];
	QFileInfo fi(strFile);
	qint64 int64 = fi.size();
	QDateTime lastDT = fi.lastModified();
	QPixmap image(strFile);
	
	QString resolution = QString("%1X%2").arg(image.width()).arg(image.height());
	QString dpi = QString("%1").arg(image.toImage().physicalDpiX());// .arg(image.toImage().logicalDpiY());

	QString result;
	result += QString("%1").arg(int64);
	result += "|";
	result += lastDT.toString("yyyy-MM-dd HH:mm:ss");
	result += "|";
	result += resolution;
	result += "|";
	result += dpi;

	return result;
}

QString ScanApp::getFileInfos(int start, int count)
{
	int locCount = count;
	
	if (m_FileConfig.size() == 0)
	{
		return "";
	}

	if (start + count >= m_FileConfig.size())
		return "";


	if (start < 0 && count < 0)
	{
		start = 0;
		count = m_FileConfig.size();
	}
	
	for (int i = 0; i < count; i++)
	{
		QString strFile = m_FileConfig[start + i];
		QFileInfo fi(strFile);
		qint64 int64 = fi.size();
		QDateTime lastDT = fi.lastModified();
		QPixmap image(strFile);

		QString resolution = QString("%1X%2").arg(image.width()).arg(image.height());
		QString dpi = QString("%1").arg(image.toImage().physicalDpiX());// .arg(image.toImage().logicalDpiY());

		QString result;
		result += QString("%1").arg(int64);
		result += "|";
		result += lastDT.toString("yyyy-MM-dd HH:mm:ss");
		result += "|";
		result += resolution;
		result += "|";
		result += dpi;


		return QString();
	}

	return "";
}

void ScanApp::getFileList(QStringList & stringList)
{
	stringList = m_FileConfig;
}

int ScanApp::insertExternalFile(const QString& strPath, bool isBehind, bool isCopy)
{
	QFileInfo fi(strPath);
	QUuid uuidCreater = QUuid::createUuid();
	QString strUUIDName = uuidCreater.toString();

	QString newFile = m_FilePath + QDir::separator() + strUUIDName + "." + fi.suffix();

	if (isCopy)
	{
		if (!QFile::copy(strPath, newFile))
		{
			return 0;
		}

		QFile copyFile(newFile);
		QFileDevice::Permissions permissions;
		permissions.setFlag(QFileDevice::WriteOwner);
		copyFile.setPermissions(permissions);
	}
	else
	{
		newFile = strPath;
	}
	
	QStringList tail;
	int countIndex = m_iCurrentIndex;
	if (isBehind)
		countIndex += 1;

	tail = m_FileConfig.mid(countIndex);
	m_FileConfig = m_FileConfig.mid(0, countIndex);
	//////////////////////////////////////////////////////
	m_FileConfig.push_back(newFile);
	//////////////////////////////////////////////////////
	m_FileConfig += tail;
	return 1;
}

int ScanApp::insertExternalFiles(const QStringList & strFilesList, bool isBehind, bool isCopy)
{
	QStringList tail;
	int countIndex = m_iCurrentIndex;
	if (isBehind)
		countIndex += 1;

	tail = m_FileConfig.mid(countIndex);
	m_FileConfig = m_FileConfig.mid(0, countIndex);
	for (int i = 0; i < strFilesList.size(); i++)
	{
		QString strPath = strFilesList[i];
		

		QString newFile;

		if (isCopy)
		{
			QFileInfo fi(strPath);
			QUuid uuidCreater = QUuid::createUuid();
			QString strUUIDName = uuidCreater.toString();
			newFile = m_FilePath + QDir::separator() + strUUIDName + "." + fi.suffix();
			if (!QFile::copy(strPath, newFile))
			{
				return 0;
			}

			QFile copyFile(newFile);
			QFileDevice::Permissions permissions;
			permissions.setFlag(QFileDevice::WriteOwner);
			copyFile.setPermissions(permissions);
		}
		else
		{
			newFile = strPath;
		}

		m_FileConfig.push_back(newFile);
	}

	m_FileConfig += tail;
	return 1;
}

int ScanApp::replaceCurrentFile(const QString& strPath, bool isCopy)
{
	int iResult = 0;
	QString strCurrentFile = m_FileConfig[m_iCurrentIndex];
	QFile curFile(strCurrentFile);
	QString newFile = strPath;
	if (isCopy)
	{
		newFile = getUUIDFileName(strPath);
		if (!QFile::copy(strPath, newFile))
		{
			return 0;
		}
		else
		{
			QFile copyFile(newFile);
			QFileDevice::Permissions permissions;
			permissions.setFlag(QFileDevice::WriteOwner);
			copyFile.setPermissions(permissions);
		}
	}

	if (curFile.exists())
	{
		//iResult = curFile.remove();
		m_FileConfig[m_iCurrentIndex] = newFile;
	}
	else
	{
		m_FileConfig[m_iCurrentIndex] = newFile;
	}
	return 1;
}

QString ScanApp::getUUIDFileName(const QString & strExt)
{
	QFileInfo fi(strExt);
	QUuid uuidCreater = QUuid::createUuid();
	QString strUUIDName = uuidCreater.toString();

	QString newFile = m_FilePath + QDir::separator() + strUUIDName + "." + fi.suffix();
	return newFile;
}

QString ScanApp::getFileFullName(int iIndex)
{
	if (iIndex >= m_FileConfig.size() || iIndex < 0)
		return "";

	QString strSrcImage = m_FileConfig.at(iIndex);
	return strSrcImage;
}

ScanApp::~ScanApp()
{
}


#define TEST_ERR_MAJOR_BAD     126
#define TEST_ERR_RUNS_FOREVER  125
#define TEST_ERR_EASY_INIT     124
#define TEST_ERR_MULTI_INIT    123
#define TEST_ERR_NUM_HANDLES   122
#define TEST_ERR_SELECT        121
#define TEST_ERR_SUCCESS       120
#define TEST_ERR_FAILURE       119
#define TEST_ERR_USAGE         118
#define TEST_ERR_FOPEN         117
#define TEST_ERR_FSTAT         116
#define TEST_ERR_BAD_TIMEOUT   115
#define test_setopt(A,B,C) \
  if((res = curl_easy_setopt((A), (B), (C))) != CURLE_OK) goto test_cleanup

#ifdef WIN64
#  include <io.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  undef  lseek
#  define lseek(fdes,offset,whence)  _lseeki64(fdes, offset, whence)
#  undef  fstat
#  define fstat(fdes,stp)            _fstati64(fdes, stp)
#  undef  stat
#  define stat(fname,stp)            _stati64(fname, stp)
#  define struct_stat                struct _stati64
#  define LSEEK_ERROR                (__int64)-1
#endif

/*
 * Small file (<2Gb) support using WIN32 functions.
 */

#ifdef USE_WIN32_SMALL_FILES
#  include <io.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  ifndef _WIN32_WCE
#    undef  lseek
#    define lseek(fdes,offset,whence)  _lseek(fdes, (long)offset, whence)
#    define fstat(fdes,stp)            _fstat(fdes, stp)
#    define stat(fname,stp)            _stat(fname, stp)
#    define struct_stat                struct _stat
#  endif
#  define LSEEK_ERROR                (long)-1
#endif
static int progress_callback(void *clientp, double dltotal, double dlnow,
	double ultotal, double ulnow)
{
	if (0 == ultotal) {
		return FALSE;
	}

	int totaldotz = 40;
	double fractiondownloaded = ulnow / ultotal;

	int ii = 0;
	qDebug()<<QString::asprintf("%3.0f%% \r\n", fractiondownloaded * 100);

	return FALSE;
}

int ScanApp::FtpUpLoad(const char * URL, const char *strSrcFileName, const char* userpwd)
{
#if 1
	
	CURL *curl;
	CURLcode res = CURLE_OK;
	FILE *hd_src;
	int hd;
	struct_stat file_info;
	struct curl_slist *hl;

	struct curl_slist *headerlist = NULL;
	const char *buf_1 = "RNFR 505";
	const char *buf_2 = "RNTO 505-forreal";

	if (!strSrcFileName) {
		fprintf(stderr, "Usage: <url> <file-to-upload>\n");
		return TEST_ERR_USAGE;
	}

	hd_src = fopen(strSrcFileName, "rb");
	if (NULL == hd_src) {
		fprintf(stderr, "fopen failed with error: %d %s\n",
			errno, strerror(errno));
		fprintf(stderr, "Error opening file: %s\n", strSrcFileName);
		return TEST_ERR_MAJOR_BAD; /* if this happens things are major weird */
	}

	/* get the file size of the local file */
	hd = fstat(fileno(hd_src), &file_info);
	if (hd == -1) {
		/* can't open file, bail out */
		fprintf(stderr, "fstat() failed with error: %d %s\n",
			errno, strerror(errno));
		fprintf(stderr, "ERROR: cannot open file %s\n", strSrcFileName);
		fclose(hd_src);
		return TEST_ERR_MAJOR_BAD;
	}

	if (!file_info.st_size) {
		fprintf(stderr, "ERROR: file %s has zero size!\n", strSrcFileName);
		fclose(hd_src);
		return TEST_ERR_MAJOR_BAD;
	}

	if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
		fprintf(stderr, "curl_global_init() failed\n");
		fclose(hd_src);
		return TEST_ERR_MAJOR_BAD;
	}

	/* get a curl handle */
	curl = curl_easy_init();
	if (!curl) {
		fprintf(stderr, "curl_easy_init() failed\n");
		curl_global_cleanup();
		fclose(hd_src);
		return TEST_ERR_MAJOR_BAD;
	}

	/* build a list of commands to pass to libcurl */

	hl = curl_slist_append(headerlist, buf_1);
	if (!hl) {
		fprintf(stderr, "curl_slist_append() failed\n");
		curl_easy_cleanup(curl);
		curl_global_cleanup();
		fclose(hd_src);
		return TEST_ERR_MAJOR_BAD;
	}
	headerlist = curl_slist_append(hl, buf_2);
	if (!headerlist) {
		fprintf(stderr, "curl_slist_append() failed\n");
		curl_slist_free_all(hl);
		curl_easy_cleanup(curl);
		curl_global_cleanup();
		fclose(hd_src);
		return TEST_ERR_MAJOR_BAD;
	}
	headerlist = hl;

	/* enable uploading */
	test_setopt(curl, CURLOPT_UPLOAD, 1L);

	/* enable verbose */
	test_setopt(curl, CURLOPT_VERBOSE, 1L);

	/* specify target */
	test_setopt(curl, CURLOPT_URL, URL);

	/* pass in that last of FTP commands to run after the transfer */
	test_setopt(curl, CURLOPT_POSTQUOTE, headerlist);

	/* now specify which file to upload */
	test_setopt(curl, CURLOPT_READDATA, hd_src);

	test_setopt(curl, CURLOPT_USERPWD, userpwd);

	/* and give the size of the upload (optional) */
	test_setopt(curl, CURLOPT_INFILESIZE_LARGE,
		(curl_off_t)file_info.st_size);

	//curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
	//test_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_callback);
	/* Now run off and do what you've been told! */
	res = curl_easy_perform(curl);

test_cleanup:

	/* clean up the FTP commands list */
	curl_slist_free_all(headerlist);

	/* close the local file */
	fclose(hd_src);

	curl_easy_cleanup(curl);
	curl_global_cleanup();

	return res;
#else
	return 0;
#endif
}

#define CHECK_FORM_ERROR(x) if ((formCode = (x)) != CURL_FORMADD_OK) {fprintf(stderr, "curl_formadd[%d] error./n", formCode);goto out;}

#define CHECK_SETOPT_ERROR(x) if ((code = (x)) != CURLE_OK){fprintf(stderr, "curl_easy_setopt[%d] error./n", code);goto all;}

int ScanApp::HttpUpload(const char * URL, const char * fileName)
{
	int ret = -1;
	CURL *curl = NULL;
	CURLcode code;
	CURLFORMcode formCode;
	int timeout = 15;



	struct curl_httppost *post = NULL;
	struct curl_httppost *last = NULL;
	struct curl_slist *headerlist = NULL;

	/*CHECK_FORM_ERROR(curl_formadd(&post, &last, CURLFORM_COPYNAME, "user",
		CURLFORM_COPYCONTENTS, user,
		CURLFORM_END));

	CHECK_FORM_ERROR(curl_formadd(&post, &last, CURLFORM_COPYNAME, "password",
		CURLFORM_COPYCONTENTS, pwd,
		CURLFORM_END));*/

	CHECK_FORM_ERROR(curl_formadd(&post, &last, CURLFORM_COPYNAME, "file",
		CURLFORM_FILE, fileName,
		CURLFORM_END));

	CHECK_FORM_ERROR(curl_formadd(&post, &last,
		CURLFORM_COPYNAME, "submit",
		CURLFORM_COPYCONTENTS, "upload",
		CURLFORM_END));

	curl = curl_easy_init();
	if (curl == NULL)
	{
		fprintf(stderr, "curl_easy_init() error./n");
		goto out;
	}

	CHECK_SETOPT_ERROR(curl_easy_setopt(curl, CURLOPT_HEADER, 0));
	CHECK_SETOPT_ERROR(curl_easy_setopt(curl, CURLOPT_URL, URL));
	CHECK_SETOPT_ERROR(curl_easy_setopt(curl, CURLOPT_HTTPPOST, post));
	CHECK_SETOPT_ERROR(curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout));

	code = curl_easy_perform(curl);
	if (code != CURLE_OK)
	{
		fprintf(stderr, "curl_easy_perform[%d] error./n", code);
		goto all;
	}

	ret = 0;

all:
	curl_easy_cleanup(curl);
out:
	curl_formfree(post);

	return ret;
}
