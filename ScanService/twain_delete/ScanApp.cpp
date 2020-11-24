#include "pch.h"
#include "ScanApp.h"
#include <QUuid>
#include <QSettings>
#include "ScanService.h"

ScanApp::ScanApp(int & argc, char ** argv, int nVer)
	: QApplication(argc, argv, nVer), m_loggingOut(m_FilePath)
{
	this->installNativeEventFilter(new NativeEventFilterZ());

	if (argc > 1)
	{
		QString tempPath = argv[1];
		if (tempPath.length() != 0)
		{
			QDir dir(tempPath);
			if (!dir.exists())
			{
				if (!dir.mkpath(tempPath))
				{
					return;
				}
			}

			m_FilePath = tempPath;
		}
	}

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

bool ScanApp::selectCurrentDS(QString strName)
{
	TW_IDENTITY twDataSource;
	//Update with the current default data source
	if (TWRC_SUCCESS == DSM_Entry(DG_CONTROL, DAT_IDENTITY, MSG_GETDEFAULT, &twDataSource))
	{
		m_currentDSName = strName;
		return true;
	}
	else
		return false;
}

bool ScanApp::showSetupDialog()
{
	EnableDS((TW_HANDLE)0/*this->winid()*/, true);
	return false;
}

void ScanApp::OnSignalDSRequest()
{

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
			
			QString strDestName = m_FilePath + QDir::separator() + strUUIDName + m_FileExtendName;
			if (strDestName.length())
			{
				if (QFile::exists(strDestName))
				{
					//remove the original file
					QFile::remove(strDestName);
				}
				//attempt to re-create the file
				QFile img(strDestName);
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
					emit signal_saveFileFinish(strDestName);
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
		//This sample Application only supports Memory Transfer of BMP/JPG/TIF (Uncomp/Jpg/G4)
		switch (twInfo.Compression)
		{
		case TWCP_NONE:
			//uncompressed images will be stored as .bmp
			m_twMemoryFileFormat = TWFF_BMP;
			m_FileExtendName = ".bmp";
			break;
		case TWCP_JPEG:
			//jpeg compressed images will be stored as a .jpg
			m_twMemoryFileFormat = TWFF_JFIF;
			m_FileExtendName = ".jpg";
			break;
		case TWCP_GROUP4:
			//Group4 compressed images will be stored as .tif
			m_twMemoryFileFormat = TWFF_TIFF;
			m_FileExtendName = ".tif";
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
				selectCurrentDS(list[0]);

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
}



ScanApp::~ScanApp()
{
}

void ScanApp::writeLogging(LPSTR text)
{
	
}

void ScanApp::closeThread()
{
	m_loggingOut.closeLoggingThread();
}

void ScanApp::CreateScanConfig()
{
	QSettings setting("", QSettings::IniFormat);
	
}
