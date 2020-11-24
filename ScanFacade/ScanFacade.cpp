#include "ScanFacade.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardItemModel>
#include "ScanApp.h"
#include "InvokeDefine.h"
#include "ServerThread.h"
#include <LogWriter.h>
#include "MatchersDialog.h"

extern LogWriter* g_logWriter;
extern bool g_bRemoteEnable;

ScanFacade::ScanFacade(QWidget *parent)
	: QMainWindow(parent), m_actionGroup(this), m_serverThread(new ServerThread)
{
	ui.setupUi(this);
	ui.plainTextEdit->setReadOnly(true);
	//bool conn = QObject::connect(this->m_serverThread, SIGNAL(invokeFuntion(char*, char*)), this, SLOT(slot_remoteInvoke(char*, char*)));
	QObject::connect(this->m_serverThread, &ServerThread::invokeFuntion, this, &ScanFacade::slot_remoteInvoke);
	ui.graphicsView->setScene(&m_graphicsScene);
}

void ScanFacade::slot_loadDSM(bool bb)
{
	if (!m_App->LoadDSM())
	{
		//QMessage
	}
}

void ScanFacade::slot_unloadDSM(bool bb)
{
	m_App->FreeDSM();
}

void ScanFacade::slot_openDSM(bool bb)
{
	WId hWnd = this->winId();
	TW_UINT16 result = m_App->OpenDSM((TW_MEMREF)hWnd);
	
	QStringList list;
	m_App->aquireDSMList(&list);

	for (int i = 0; i < list.count(); i++)
	{
		QAction* pAction = new QAction(list.at(i));
		m_actionGroup.addAction(pAction);
		ui.menuDS->addAction(pAction);
	}
}

void ScanFacade::slot_closeDSM(bool bb)
{
	m_App->CloseDSM();
}

void ScanFacade::slot_closeAquireDSMList(bool bb)
{
	
}

void ScanFacade::slot_openDS(bool bb)
{
	if (TWRC_SUCCESS == m_App->OpenDS())
	{

	}
}

void ScanFacade::slot_closeDS(bool bb)
{
	if (TWRC_SUCCESS == m_App->CloseDS())
	{

	}
}

void ScanFacade::slot_ActionTriggered(QAction* action)
{
	QString text = action->text();
	if (!m_App->selectCurrentDS(text))
	{
		QMessageBox::information(NULL, "Title", "To select DS \"" + text + "\" failure.", QMessageBox::Yes);
	}
}

void ScanFacade::slot_action_show_setup_dialog(bool bb)
{
	m_App->showSetupDialog((TW_HANDLE)this->winId());
}

void ScanFacade::slot_actionStartScan(bool bb)
{
	WId hWnd = this->winId();

	QString strDeviceName;

	getSelectDeviceName(&strDeviceName);
	if (strDeviceName.length() == 0) return;
	m_App->modifyCurrentDSName(strDeviceName);

	m_App->clearScan();
	m_App->newScan((TW_HANDLE)hWnd);
	updatePageList();
	slot_nextImage();
}

void ScanFacade::slot_upload(bool bb)
{
	//m_App->UploadFile(0, "http://192.168.1.111:8081/img-toogle", 0, 0, 0);
	//m_App->uploadFile("D:/cmake-3.12.0-rc2-win64-x64.zip");
}

void ScanFacade::slot_insertExtenalFile(bool bb)
{
	QStringList files = QFileDialog::getOpenFileNames(
		this,
		"Select one or more files to open",
		"D:\\temp\\images",
		"Images (*.png *.xpm *.jpg)");

	for (int i = 0; i < files.size(); i++)
	{
		m_App->insertExternalFile(files[i]);
	}

	m_App->SaveConfig();
}

void ScanFacade::slot_perviousImage(bool bb)
{
	QString img = m_App->perviousImage();
	QPixmap pixmap(img);
	m_graphicsScene.clear();
	m_graphicsScene.addPixmap(pixmap);
}

void ScanFacade::slot_nextImage(bool bb)
{
	QString img = m_App->nextImage();
	QPixmap pixmap(img);
	m_graphicsScene.clear();
	m_graphicsScene.addPixmap(pixmap);
}

void ScanFacade::slot_clearScan(bool bb)
{
	m_App->clearScan();
	m_graphicsScene.clear();
}

void ScanFacade::slot_insertScan(bool bb)
{
	//QString strInfo = m_App->getFileInfo(1);
	//m_App->insertExternalFile("D:\\temp\\del\\1.jpg");
	//m_App->DrawImageToImage(1, 200, 200, "D:\\temp\\del\\watermark.bmp");
	//m_App->writeTextToImage(1, 200, 200, 0xFF000000, 20, true, "", QString::fromLocal8Bit("我们是人类"));
	m_App->insertScan();
	slot_nextImage();
	m_App->SaveConfig();
}

void ScanFacade::slot_replaceScan(bool bb)
{
	m_App->replaceScan();
	slot_nextImage();
	m_App->SaveConfig();
}

void ScanFacade::slot_deleteImage(bool bb)
{
	m_App->deleteImage(m_App->getCurrentIndex(), 1);
	m_App->SaveConfig();
	slot_nextImage();
}

void ScanFacade::slot_startServer(bool bb)
{
	//ServerThread* st = new ServerThread();
	m_isListen = m_serverThread->startListen();
}

void ScanFacade::slot_enableWindowLogging(bool bb)
{
	if (bb)
	{
		ui.actionwindow_2->setChecked(bb);
		ui.actionfile->setChecked(!bb);
		ui.actiondisable->setChecked(!bb);
		g_bRemoteEnable = true;

		m_bOutLoggingToUI = true;
	}
}

void ScanFacade::slot_enableFileLogging(bool bb)
{
	if (bb)
	{
		ui.actionwindow_2->setChecked(!bb);
		ui.actiondisable->setChecked(!bb);
		ui.actionfile->setChecked(bb);
		g_bRemoteEnable = true;

		m_bOutLoggingToUI = false;
	}
}

void ScanFacade::slot_disableFileLogging(bool bb)
{
	if (bb)
	{
		ui.actionwindow_2->setChecked(!bb);
		ui.actiondisable->setChecked(bb);
		ui.actionfile->setChecked(!bb);
		g_bRemoteEnable = false;
	}
}

void ScanFacade::slot_cleanButton(bool)
{
	QTextDocument* pDoc = ui.plainTextEdit->document();
	pDoc->setPlainText("");
}

void ScanFacade::slot_configMatchers(bool)
{
	
	MatchersDialog md;
	int iR = md.exec();
	if (iR == QDialog::Accepted)
	{

	}
}

//QVariant ScanFacade::invokeFuntion(char * name, char * param)
//{
//	return slot_remoteInvoke(name, param);
//}

void ScanFacade::connectAllSignalsToSlots()
{
	connect(ui.actionload, SIGNAL(triggered(bool)), SLOT(slot_loadDSM(bool)));
	connect(ui.actionunload, SIGNAL(triggered(bool)), SLOT(slot_unloadDSM(bool)));
	connect(ui.actionopen, SIGNAL(triggered(bool)), SLOT(slot_openDSM(bool)));
	connect(ui.actionclose, SIGNAL(triggered(bool)), SLOT(slot_closeDSM(bool)));

	connect(ui.actionopen_2, SIGNAL(triggered(bool)), SLOT(slot_openDS(bool)));
	connect(ui.actionclose_2, SIGNAL(triggered(bool)), SLOT(slot_closeDS(bool)));
	connect(ui.action_show_setup_dialog, SIGNAL(triggered(bool)), SLOT(slot_action_show_setup_dialog(bool)));
	connect(ui.actionStartScan, SIGNAL(triggered(bool)), SLOT(slot_actionStartScan(bool)));
	connect(ui.actionUpload, SIGNAL(triggered(bool)), SLOT(slot_upload(bool)));

	connect(&m_actionGroup, SIGNAL(triggered(QAction*)), SLOT(slot_ActionTriggered(QAction*)));
	connect(ui.actionprevious, SIGNAL(triggered(bool)), SLOT(slot_perviousImage(bool)));
	connect(ui.actionnext, SIGNAL(triggered(bool)), SLOT(slot_nextImage(bool)));
	connect(ui.actionClear, SIGNAL(triggered(bool)), SLOT(slot_clearScan(bool)));
	connect(ui.actioninsert, SIGNAL(triggered(bool)), SLOT(slot_insertScan(bool)));
	connect(ui.actionReplace, SIGNAL(triggered(bool)), SLOT(slot_replaceScan(bool)));
	connect(ui.actionDelete, SIGNAL(triggered(bool)), SLOT(slot_deleteImage(bool)));
	connect(ui.actionInsert_Extenal_Image_I, SIGNAL(triggered(bool)), SLOT(slot_insertExtenalFile(bool)));

	connect(ui.actionStart, SIGNAL(triggered(bool)), SLOT(slot_startServer(bool)));
	connect(ui.actionwindow_2, SIGNAL(triggered(bool)), SLOT(slot_enableWindowLogging(bool)));
	connect(ui.actionfile, SIGNAL(triggered(bool)), SLOT(slot_enableFileLogging(bool)));
	connect(ui.actiondisable, SIGNAL(triggered(bool)), SLOT(slot_disableFileLogging(bool)));
	connect(ui.cleanButton, SIGNAL(clicked(bool)), SLOT(slot_cleanButton(bool)));

	connect(ui.actionMatchers, SIGNAL(triggered(bool)), SLOT(slot_configMatchers(bool)));
}

void ScanFacade::restoreConfig()
{
	m_App->LoadConfig();
	updatePageList();
	this->slot_nextImage();
}

int ScanFacade::newScan(QString strPath)
{
	int iResult = m_App->setScanFolder(strPath);
	WId hWnd = this->winId();
	m_App->clearScan();
	if (iResult)
	{
		iResult = m_App->newScan((TW_HANDLE)hWnd);
	}
	else
	{
		return iResult;
	}

	return iResult;
}

void ScanFacade::getSelectDeviceName(QString * outName)
{
	this->slot_loadDSM();
	this->slot_openDSM();
	QStringList deviceNames;
	m_App->aquireDSMList(&deviceNames);
	this->slot_closeDSM();
	this->slot_unloadDSM();
	if (deviceNames.size() != 0)
	{
		Ui::DeiveDialog DeviceDialogSetup;
		QDialog deviceDialog;
		DeviceDialogSetup.setupUi(&deviceDialog);
		QStringListModel listModel;
		listModel.setStringList(deviceNames);

		DeviceDialogSetup.listView->setModel(&listModel);

		deviceDialog.setModal(true);
		deviceDialog.exec();
		if (deviceDialog.result() == QDialog::Accepted)
		{
			QItemSelectionModel *selmodel = DeviceDialogSetup.listView->selectionModel();
			QModelIndexList indexlist = selmodel->selectedIndexes();
			int count1 = indexlist.size();
			if (count1 > 0)
			{
				QModelIndex modelIndex = indexlist.at(0);
				QString text = modelIndex.data().toString();
				*outName = text;
				//qDebug() << text;
			}
		}
	}
}

void ScanFacade::updatePageList()
{
	QStandardItemModel* aim = new QStandardItemModel(ui.listView);

	for (int i = 0; i < m_App->getFileCount(); i++)
	{
		QStandardItem* item = new QStandardItem;
		item->setText(QString::fromLocal8Bit("page(%1)").arg(i + 1));
		aim->appendRow(item);
	}

	ui.listView->setModel(aim);

}

QVariant ScanFacade::slot_remoteInvoke(char * name, char * param)
{
	QString strName = QString::fromLocal8Bit(name);
	QString params;
	QVariant var;
	if (strName == QString::fromLocal8Bit(NAME_newScan))
	{
		QString strPath = QString::fromLocal8Bit(param);
		params = strPath;
		var = this->newScan(strPath);
	}
	else if (strName == QString::fromLocal8Bit(NAME_replaceScan))
	{
		var = m_App->replaceScan();
	}
	else if (strName == QString::fromLocal8Bit(NAME_insertScan))
	{
		bool iParam = (bool)*param;
		params = QString("p1(bool):%1").arg(iParam);
		var = m_App->insertScan(iParam);
	}
	else if (strName == QString::fromLocal8Bit(NAME_packageImages))
	{
		m_App->packageImages();
		var = 1;
	}
	else if (strName == QString::fromLocal8Bit(NAME_perviousImage))
	{
		QString strImage = m_App->perviousImage();
		var = strImage;
	}
	else if (strName == QString::fromLocal8Bit(NAME_nextImage))
	{
		QString strImage = m_App->nextImage();
		var = strImage;
	}
	else if (strName == QString::fromLocal8Bit(NAME_SaveConifg))
	{
		m_App->SaveConfig();
		var = 1;
	}
	else if (strName == QString::fromLocal8Bit(NAME_LoadConfig))
	{
		m_App->LoadConfig();
		var = 1;
	}
	else if (strName == QString::fromLocal8Bit(NAME_clearScan))
	{
		m_App->clearScan();
		var = 1;
	}
	else if (strName == QString::fromLocal8Bit(NAME_deleteImage))
	{
		int* pIntArray = (int*)param;
		int iStart = pIntArray[0];
		int iCount = pIntArray[1];
		
		params = QString("p1(int):%1, p2(int):%2").arg(iStart).arg(iCount);

		var = m_App->deleteImage(iStart, iCount);
	}
	else if (strName == QString::fromLocal8Bit(NAME_deleteImage2))
	{
		QString strIndexes = QString::fromLocal8Bit(param);
		params = QString("p1(String):%1").arg(strIndexes);
		var = m_App->deleteImage(strIndexes);
	}
	else if (strName == QString::fromLocal8Bit(NAME_uploadFile))
	{
		QString strFile = QString::fromLocal8Bit(param);
		params = QString("p1(String):%1").arg(strFile);
		int curlCode = m_App->UploadFile(strFile);
		var = curlCode;
	}
	else if (strName == QString::fromLocal8Bit(NAME_getFileIndex))
	{
		QString strFile = QString::fromLocal8Bit(param);
		params = QString("p1(String):%1").arg(strFile);
		var = m_App->getFileIndex(strFile);
	}
	else if (strName == QString::fromLocal8Bit(NAME_adjustFile))
	{
		int* pIntArray = (int*)param;
		int iFont = pIntArray[0];
		int iBack = pIntArray[1];
		params = QString("p1(int):%1, p2(int):%2").arg(iFont).arg(iBack);
		m_App->adjustFile(iFont, iBack);
		var = 1;
	}
	else if (strName == QString::fromLocal8Bit(NAME_getCurrentIndex))
	{
		var = m_App->getCurrentIndex();
	}
	else if (strName == QString::fromLocal8Bit(NAME_getFileCount))
	{
		var = m_App->getFileCount();
	}
	else if (strName == QString::fromLocal8Bit(NAME_setScanFolder))
	{
		QString strFile = QString::fromLocal8Bit(param);
		params = QString("p1(String):%1").arg(strFile);
		var = m_App->setScanFolder(strFile);
		
	}
	else if (strName == QString::fromLocal8Bit(NAME_getFileList))
	{
		QStringList FileName;
		m_App->getFileList(FileName);

		var = FileName;
	}
	else if (strName == QString::fromLocal8Bit(NAME_setCurrentIndex))
	{
		int iParam = *(int*)param;
		params = QString("p1(int):%1").arg(iParam);
		var = m_App->setCurrentIndex(iParam);
	}
	else if (strName == QString::fromLocal8Bit(NAME_insertExternalFile))
	{
		char* pTemp = param;
		bool iParam = *(bool*)param;
		pTemp += sizeof(iParam);
		bool iParam2 = *(bool*)pTemp;
		pTemp += sizeof(iParam2);
		char* pBuffer = pTemp;
		QString strFile = QString::fromLocal8Bit(pBuffer);
		params = QString("p1(bool):%1, p2(bool):%2, p3(char*):%2").arg(strFile).arg(iParam).arg(iParam2);
		var = m_App->insertExternalFile(strFile, iParam, iParam2);
	}
	else if (strName == QString::fromLocal8Bit(NAME_insertExternalFiles))
	{
		char* pTemp = param;
		bool iParam = *(bool*)param;
		pTemp += sizeof(iParam);
		bool iParam2 = *(bool*)pTemp;
		pTemp += sizeof(iParam2);
		char* pBuffer = pTemp;
		QString strFile = QString::fromLocal8Bit(pBuffer);

		QStringList strFiles = strFile.split("|");
		params = QString("p1(bool):%1, p2(bool):%2, p3(char*):%2").arg(strFile).arg(iParam).arg(iParam2);
		var = m_App->insertExternalFiles(strFiles, iParam, iParam2);
	}
	else if (strName == QString::fromLocal8Bit(NAME_getFileInfo))
	{
		int iParam = *(int*)param;
		params = QString("p1(int):%1").arg(iParam);
		var = m_App->getFileInfo(iParam);
	}
	else if (strName == QString::fromLocal8Bit(NAME_getFileFullName))
	{
		int iParam = *(int*)param;
		params = QString("p1(int):%1").arg(iParam);
		var = m_App->getFileFullName(iParam);
	}
	else if (strName == QString::fromLocal8Bit(NAME_getDeviceList))
	{
		this->slot_loadDSM();
		this->slot_openDSM();
		QStringList deviceNames;
		m_App->aquireDSMList(&deviceNames);
		this->slot_closeDSM();
		this->slot_unloadDSM();
		var = deviceNames;
	}
	else if (strName == QString::fromLocal8Bit(NAME_setDefaultDevice))
	{
		char* pBuffer = param;
		QString deviceName = QString::fromLocal8Bit(pBuffer);
		params = QString("p1(String):%1").arg(deviceName);
		m_App->modifyCurrentDSName(deviceName);
		var = 1;
	}
	else if (strName == QString::fromLocal8Bit(NAME_getDefaultDevice))
	{
		var = m_App->getCurrentDSName();
	}
	else if (strName == QString::fromLocal8Bit(NAME_replaceCurrentFile))
	{
		bool iParam = *(bool*)param;
		
		char* pBuffer = param + sizeof(iParam);
		QString strFile = QString::fromLocal8Bit(pBuffer);
		params = QString("p1(String):%1, p2(bool):%2").arg(strFile).arg(iParam);
		var = m_App->replaceCurrentFile(strFile, iParam);
	}
#if 0
	else if (strName == QString::fromLocal8Bit(NAME_writeTextToImage))
	{
		size_t l0(sizeof(param)), l1(sizeof(int)), l2(sizeof(int)), l3(sizeof(int)), l4(sizeof(uint)), l5(sizeof(uint)), l6(sizeof(bool)), l7(sizeof(uint));
		char* pCurrentAddress = param;
		int iIndex = *(int*)pCurrentAddress;

		pCurrentAddress += l1;
		int x = *(int*)pCurrentAddress;

		pCurrentAddress += l2;
		int y = *(int*)pCurrentAddress;

		pCurrentAddress += l3;
		uint cololr = *(uint*)pCurrentAddress;

		pCurrentAddress += l4;
		uint size = *(uint*)pCurrentAddress;

		pCurrentAddress += l5;
		bool isBold = *(bool*)pCurrentAddress;

		pCurrentAddress += l6;
		uint stringLen = *(uint*)pCurrentAddress;

		pCurrentAddress += l7;

		QString font = "";
		if (stringLen)
			font = QString::fromLocal8Bit(pCurrentAddress);

		pCurrentAddress += (stringLen + 1);
		QString text = QString::fromLocal8Bit(pCurrentAddress);

		var = m_App->writeTextToImage(iIndex, x, y, cololr, size, 0, isBold, font, text);
	}
	else if (strName == QString::fromLocal8Bit(NAME_drawImageToImage))
	{
		size_t l0(sizeof(param)), l1(sizeof(int)), l2(sizeof(int)), l3(sizeof(int));

		char* pCurrentAddress = param;
		int iIndex = *(int*)pCurrentAddress;

		pCurrentAddress += l1;
		int x = *(int*)pCurrentAddress;

		pCurrentAddress += l2;
		int y = *(int*)pCurrentAddress;

		pCurrentAddress += l3;
		QString strText = QString::fromLocal8Bit(pCurrentAddress);

		var = m_App->drawImageToImage(iIndex, x, y, strText);
	}
#endif
	else
	{
		QVariant var = 0;
	}

	if (g_bRemoteEnable)
	{
		QDateTime current_time = QDateTime::currentDateTime();
		QString strTime = current_time.toString("yyyy-MM-dd hh:mm:ss.zzz");
		QString loggingText = QString::fromLocal8Bit("[%1] {To invoke slot_remoteInvoke (name : %2, parameters : %3)} -> {function return (%4)}");

		QString strReturn = var.toString();
		if (strReturn.length() > 120)
		{
			strReturn = strReturn.left(120);
		}

		loggingText = loggingText.arg(strTime).arg(strName).arg(params).arg(strReturn);
		if (m_bOutLoggingToUI)
		{
			QTextDocument* pDoc = ui.plainTextEdit->document();
			QString oldText = pDoc->toPlainText();
			QString newText = oldText + loggingText;
			pDoc->setPlainText(newText + QString::fromLocal8Bit("\r\n"));
		}
		else
			g_logWriter->WrtieLogging(QString::fromLocal8Bit("C:\\windows\\temp\\ScanBox_logging.txt"), loggingText, LogWriter::FILELOGGING);
	}
	
	return var;
}
