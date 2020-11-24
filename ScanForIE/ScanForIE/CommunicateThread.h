#pragma once
#include <QThread>

class CommunicateThread :
	public QThread
{
public:
	CommunicateThread();
	~CommunicateThread();
	virtual void run();
};

