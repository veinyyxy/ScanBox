#pragma once
#include <QVariant>

class InvokeInterface
{
public:
	virtual QVariant invokeFuntion(char* name, char* param) = 0;
	virtual ~InvokeInterface();
};

