#include "InvokeDefine.h"
#include "ServerThread.h"

char * InvokeToBuffer(char * name, char * param)
{
	P_INVOKE_DATA pInvokeData = new INVOKE_DATA;
	MAKE_CALL(pInvokeData, 0, name, param)
	return (char*)pInvokeData;
}

char * GetInvokeReturn(char * data)
{
	P_INVOKE_DATA pInvokeData = (P_INVOKE_DATA)data;

	return pInvokeData->result;
}
