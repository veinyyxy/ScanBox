#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TestCommunicate.h"

class ClientThread;

class TestCommunicate : public QMainWindow
{
	Q_OBJECT

public:
	TestCommunicate(QWidget *parent = Q_NULLPTR);

private:
	Ui::TestCommunicateClass ui;
	ClientThread* m_pClientThread;

public slots:
	void slot_connectServer(bool b = false);
	void slot_invokeFunction(bool b = false);
	void slot_disConnectServer(bool b = false);
};
