#include "TestCommunicate.h"
#include "ClientThread.h"
#include "InvokeDefine.h"

TestCommunicate::TestCommunicate(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	m_pClientThread = new ClientThread;
	connect(ui.connectButton, SIGNAL(clicked(bool)), SLOT(slot_connectServer(bool)));
	connect(ui.disconnectButton, SIGNAL(clicked(bool)), SLOT(slot_disConnectServer(bool)));
	connect(ui.invokeButton, SIGNAL(clicked(bool)), SLOT(slot_invokeFunction(bool)));
}

void TestCommunicate::slot_connectServer(bool b)
{
	m_pClientThread->connectServer();
}

void TestCommunicate::slot_invokeFunction(bool b)
{
	QString strName = ui.nameEdit->text();
	QString strFuntion = ui.paramEdit->text();

	QVariant result = m_pClientThread->invokeFuntion(strName.toLocal8Bit().data(), strFuntion.toLocal8Bit().data());

	ui.textEdit->setText(result.toString());
}

void TestCommunicate::slot_disConnectServer(bool b)
{

}
