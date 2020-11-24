#include "stdafx.h"
#include "InvokeDefine.h"
#include "ClientThread.h"

char * InvokeToBuffer(char * name, char * param)
{
	P_INVOKE_DATA pInvokeData = new INVOKE_DATA;
	memset(pInvokeData, 0, sizeof(INVOKE_DATA));
	MAKE_CALL(pInvokeData, 0, name, param);
	return (char*)pInvokeData;
}

char * GetInvokeReturn(char * data)
{
	P_INVOKE_DATA pInvokeData = (P_INVOKE_DATA)data;

	return pInvokeData->result;
}
