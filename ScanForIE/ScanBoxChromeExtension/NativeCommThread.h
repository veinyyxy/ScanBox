#pragma once
class NativeCommThread : public QThread
{
public:
	NativeCommThread();
	~NativeCommThread();

	virtual void run();
};

