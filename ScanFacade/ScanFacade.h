#pragma once

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "ui_ScanFacade.h"
#include "ui_DeviceDialog.h"
#include "InvokeInterface.h"
//#include "ServerThread.h"

class ServerThread;
class ScanApp;

class ScanFacade : public QMainWindow//, InvokeInterface
{
	Q_OBJECT

public:
	ScanFacade(QWidget *parent = Q_NULLPTR);
	void connectAllSignalsToSlots();
	inline void SetApp(ScanApp* pApp) { m_App = pApp; }
	void restoreConfig();
	int newScan(QString strPath);
	inline bool isListen() { return m_isListen; }
	void getSelectDeviceName(QString* outName);
	void updatePageList();
public slots:
	QVariant slot_remoteInvoke(char* name, char* param);
	void slot_loadDSM(bool bb = false);
	void slot_unloadDSM(bool bb = false);
	void slot_openDSM(bool bb = false);
	void slot_closeDSM(bool bb = false);
	void slot_closeAquireDSMList(bool bb = false);
	void slot_openDS(bool bb = false);
	void slot_closeDS(bool bb = false);
	void slot_ActionTriggered(QAction* action);
	void slot_action_show_setup_dialog(bool bb = false);
	void slot_actionStartScan(bool bb = false);
	void slot_upload(bool bb = false);
	void slot_insertExtenalFile(bool bb = false);
	void slot_perviousImage(bool bb = false);
	void slot_nextImage(bool bb = false);
	void slot_clearScan(bool bb = false);
	void slot_insertScan(bool bb = false);
	void slot_replaceScan(bool bb = false);
	void slot_deleteImage(bool bb = false);
	void slot_startServer(bool bb = false);
	void slot_enableWindowLogging(bool bb);
	void slot_enableFileLogging(bool bb);
	void slot_disableFileLogging(bool bb);
	void slot_cleanButton(bool);
	void slot_configMatchers(bool);
private:
	Ui::ScanFacadeClass ui;
	QString currentDS = "";
	QActionGroup m_actionGroup;
	// Inherited via CTWAINSession
	ServerThread* m_serverThread;
	ScanApp *m_App = 0;
	QGraphicsScene m_graphicsScene;
	bool m_isListen = false;
	bool m_bOutLoggingToUI = true;
	// Inherited via InvokeInterface
	//virtual QVariant invokeFuntion(char * name, char * param) override;
};