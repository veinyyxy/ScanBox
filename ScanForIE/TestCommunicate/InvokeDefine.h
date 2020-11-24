#pragma once

#define MAKE_CALL(data, dataType, funName, funParam) \
	data->type = dataType;\
	strcpy_s(data->name, funName);\
	if(funParam != nullptr)\
	{\
		strcpy_s(data->param, funParam);\
	}

#define NAME_newScan "newScan"
#define NAME_replaceScan "replaceScan"
#define NAME_insertScan "insertScan"
#define NAME_packageImages "packageImages"
#define NAME_perviousImage "perviousImage"
#define NAME_nextImage "nextImage"
#define NAME_SaveConifg "SaveConifg"
#define NAME_LoadConfig "LoadConfig"
#define NAME_clearScan "LoadConfig"
#define NAME_deleteImage "deleteImage"
#define NAME_uploadFile "uploadFile"
#define NAME_getFileIndex "getFileIndex"
#define NAME_adjustFile "adjustFile"
#define NAME_getCurrentIndex "getCurrentIndex"
#define NAME_getFileCount "getFileCount"
#define NAME_setScanFolder "setScanFolder"

char* InvokeToBuffer(char* name, char* parm);
char* GetInvokeReturn(char* data);