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
#define NAME_clearScan "clearScan"
#define NAME_deleteImage "deleteImage"
#define NAME_deleteImage2 "deleteImage2"
#define NAME_uploadFile "uploadFile"
#define NAME_getFileIndex "getFileIndex"
#define NAME_adjustFile "adjustFile"
#define NAME_getCurrentIndex "getCurrentIndex"
#define NAME_getFileCount "getFileCount"
#define NAME_setScanFolder "setScanFolder"
#define NAME_getFileList "getFileList"
#define NAME_setCurrentIndex "setCurrentIndex"
#define NAME_getFileInfo "getFileInfo"
#define NAME_insertExternalFile  "insertExternalFile"
//#define NAME_writeTextToImage "writeTextToImage"
//#define NAME_drawImageToImage "drawImageToImage"
#define NAME_getFileFullName "getFileFullName"
#define NAME_getDeviceList "getDeviceList"
#define NAME_setDefaultDevice "setDefaultDevice"
#define NAME_getDefaultDevice "getDefaultDevice"
#define NAME_replaceCurrentFile "replaceCurrentFile"
#define NAME_insertExternalFiles "insertExternalFiles"

char* InvokeToBuffer(char* name, char* parm);
char* GetInvokeReturn(char* data);