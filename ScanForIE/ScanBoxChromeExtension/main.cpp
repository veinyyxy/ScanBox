// ScanBoxChromeExtension.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "NativeCommThread.h"
#include "ScanBoxChromeExtension.h"
#include "LogWriter.h"

LogWriter* g_logWriter = new LogWriter;
bool g_bRemoteEnable = false;
QString g_loggingFile = QString::fromLocal8Bit("C:\\windows\\temp\\ScanForChrome_logging.txt");

#define OUT_BUFFER_SIZE (1024 * 1024 * 40)
#define ONE_MILLION (1024 * 1000)
char* outBuf = new char[OUT_BUFFER_SIZE];
char* buffer = new char[OUT_BUFFER_SIZE];
void GetImageBase64(const QString& fileName, QString& base64String)
{
	const uint bufferSize = 1024 * 1024 * 30;
	
	QFile imageFile(fileName);
	qint64 filesize = imageFile.size();
	if (imageFile.open(QIODevice::ReadOnly))
	{
		QDataStream ds(&imageFile);
		char* buffer = new char[bufferSize];
		memset(buffer, 0, bufferSize);
		if (filesize < bufferSize)
			ds.readRawData(buffer, filesize);

		QByteArray* ba = new QByteArray(buffer, filesize);

		QByteArray base64Code = ba->toBase64();


		base64String = QString::fromLocal8Bit(base64Code);
		

		delete ba;
		delete[] buffer;
	}
}
void ParseInvoke(ScanBoxChromeExtension* pSCE, const char* pJsonString, QJsonObject& returnJson)
{
	//QString strJ = QString::fromLocal8Bit("{\"invoke\":\"newScan\",\"paramenters\":{\"nothing\":\"D:\\\\temp\\\\ScanBox\"}}");
	//QByteArray ba = strJ.toLocal8Bit();
	//QString strJ = QString::fromUtf8(pJsonString);
	//strJ = strJ.mid(1, strJ.length() - 2);
	//strJ = strJ.replace("\\\"", "\"");
	//strJ = strJ.replace("\\\\", "\\");
	//QByteArray ba = strJ.toLocal8Bit();
	//char* pData = ba.data();
	QString strLogedParams;
	QString strLogedReturn;

	QJsonParseError jsonerror;
	QJsonDocument doc = QJsonDocument::fromJson(pJsonString, &jsonerror);
	QJsonDocument returnDoc;
	
	if (!doc.isNull() && jsonerror.error == QJsonParseError::NoError)
	{
		if (doc.isObject())
		{
			QJsonObject object = doc.object();
			/*QStringList list = object.keys();
			for (int i = 0; i < list.count(); i++)
			{
				qDebug() << list.at(i);
			}*/
			QJsonValue jv = object.value("invoke");
			QString funName = jv.toString();

			QJsonObject params = object.value("paramenters").toObject();


			if (funName == QString::fromLocal8Bit("newScan"))
			{
				QString param = params.value("nothing").toString();
				int iR = pSCE->newScan(param);

				strLogedParams = param;
				strLogedReturn = QString("%1").arg(iR);

				returnJson.insert(QString::fromLocal8Bit("reply"), iR);
			}
			else if (funName == QString::fromLocal8Bit("replaceScan"))
			{
				QString strR = pSCE->replaceScan();

				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("insertScan"))
			{
				bool param = params.value("isBehind").toBool();
				QString strR = pSCE->insertScan(param);

				strLogedParams = param;
				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("packageImages"))
			{
				pSCE->packageImages();
			}
			else if (funName == QString::fromLocal8Bit("perviousImage"))
			{
				QString strR = pSCE->perviousImage();

				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("nextImage"))
			{
				QString strR = pSCE->nextImage();

				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("SaveConfig"))
			{
				pSCE->SaveConfig();
				returnJson.insert(QString::fromLocal8Bit("reply"), QJsonValue::Null);
			}
			else if (funName == QString::fromLocal8Bit("LoadConfig"))
			{
				pSCE->LoadConfig();
				returnJson.insert(QString::fromLocal8Bit("reply"), QJsonValue::Null);
			}
			else if (funName == QString::fromLocal8Bit("clearScan"))
			{
				pSCE->clearScan();
				returnJson.insert(QString::fromLocal8Bit("reply"), QJsonValue::Null);
			}
			else if (funName == QString::fromLocal8Bit("deleteImage"))
			{
				int param1 = params.value("startIndex").toInt();
				int param2 = params.value("iCount").toInt();
				int iR = pSCE->deleteImage(param1, param2);

				strLogedParams = QString("%1, %2").arg(param1).arg(param2);
				strLogedReturn = QString("%1").arg(iR);

				returnJson.insert(QString::fromLocal8Bit("reply"), iR);
			}
			else if (funName == QString::fromLocal8Bit("deleteImage2"))
			{
				QString param1 = params.value("indexes").toString();
				int iR = pSCE->deleteImage2(param1);

				strLogedParams = QString("%1").arg(param1);
				strLogedReturn = QString("%1").arg(iR);

				returnJson.insert(QString::fromLocal8Bit("reply"), iR);
			}
			else if (funName == QString::fromLocal8Bit("uploadFile"))
			{
				QString param = params.value("strFile").toString();
				int iR = pSCE->uploadFile(param);

				strLogedParams = QString("%1").arg(param);
				strLogedReturn = QString("%1").arg(iR);

				returnJson.insert(QString::fromLocal8Bit("reply"), iR);
			}
			else if (funName == QString::fromLocal8Bit("uploadAnyFile"))
			{
				QString param1 = params.value("strURL").toString();
				QString param2 = params.value("strLocalFile").toString();
				QString strR = pSCE->uploadAnyFile(param1, param2);

				strLogedParams = QString("%1, %2").arg(param1).arg(param2);
				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("getFileIndex"))
			{
				QString param = params.value("strFile").toString();
				int iR = pSCE->getFileIndex(param);

				strLogedParams = QString("%1").arg(param);
				strLogedReturn = QString("%1").arg(iR);

				returnJson.insert(QString::fromLocal8Bit("reply"), iR);
			}
			else if (funName == QString::fromLocal8Bit("adjustFile"))
			{
				int param1 = params.value("front").toInt();
				int param2 = params.value("back").toInt();
				int iR = pSCE->adjustFile(param1, param2);

				strLogedParams = QString("%1, %2").arg(param1).arg(param2);
				strLogedReturn = QString("%1").arg(iR);

				returnJson.insert(QString::fromLocal8Bit("reply"), iR);
			}
			else if (funName == QString::fromLocal8Bit("getCurrentIndex"))
			{
				int iR = pSCE->getCurrentIndex();

				strLogedReturn = QString("%1").arg(iR);

				returnJson.insert(QString::fromLocal8Bit("reply"), iR);
			}
			else if (funName == QString::fromLocal8Bit("setCurrentIndex"))
			{
				int param = params.value("iIndex").toInt();
				int iR = pSCE->setCurrentIndex(param);

				strLogedParams = QString("%1").arg(param);
				strLogedReturn = QString("%1").arg(iR);

				returnJson.insert(QString::fromLocal8Bit("reply"), iR);
			}
			else if (funName == QString::fromLocal8Bit("getFileCount"))
			{
				int iR = pSCE->getFileCount();

				strLogedReturn = QString("%1").arg(iR);

				returnJson.insert(QString::fromLocal8Bit("reply"), iR);
			}
			else if (funName == QString::fromLocal8Bit("setScanFolder"))
			{
				QString param = params.value("strPath").toString();
				int iR = pSCE->setScanFolder(param);

				strLogedParams = QString("%1").arg(param);
				strLogedReturn = QString("%1").arg(iR);

				returnJson.insert(QString::fromLocal8Bit("reply"), iR);
			}
			else if (funName == QString::fromLocal8Bit("getFileInfo"))
			{
				int param = params.value("iIndex").toInt();
				QString strR = pSCE->getFileInfo(param);

				strLogedParams = QString("%1").arg(param);
				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("getFileInfoFromNames"))
			{
				QString param = params.value("strFileFullName").toString();
				QString strR = pSCE->getFileInfoFromNames(param);

				strLogedParams = QString("%1").arg(param);
				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("getFileList"))
			{
				QString strR = pSCE->getFileList();

				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("insertExternalFile"))
			{
				QString param1 = params.value("strPath").toString();
				bool param2 = params.value("isBehind").toBool();
				bool param3 = params.value("isCopy").toBool();
				int iR = pSCE->insertExternalFile(param1, param2, param3);

				strLogedParams = QString("%1, %2, %3").arg(param1).arg(param2).arg(param3);
				strLogedReturn = QString("%1").arg(iR);

				returnJson.insert(QString::fromLocal8Bit("reply"), iR);
			}
			else if (funName == QString::fromLocal8Bit("insertExternalFiles"))
			{
				QString param1 = params.value("strFilesList").toString();
				bool param2 = params.value("isBehind").toBool();
				bool param3 = params.value("isCopy").toBool();
				int iR = pSCE->insertExternalFiles(param1, param2, param3);

				strLogedParams = QString("%1, %2, %3").arg(param1).arg(param2).arg(param3);
				strLogedReturn = QString("%1").arg(iR);

				returnJson.insert(QString::fromLocal8Bit("reply"), iR);
			}
			else if (funName == QString::fromLocal8Bit("replaceCurrentFile"))
			{
				QString param1 = params.value("strPath").toString();
				bool param2 = params.value("isCopy").toBool();
				int iR = pSCE->replaceCurrentFile(param1, param2);

				strLogedParams = QString("%1, %2").arg(param1).arg(param2);
				strLogedReturn = QString("%1").arg(iR);

				returnJson.insert(QString::fromLocal8Bit("reply"), iR);
			}
			else if (funName == QString::fromLocal8Bit("getDiskFile"))
			{
				QString param1 = params.value("caption").toString();
				QString param2 = params.value("strDir").toString();
				QString param3 = params.value("filter").toString();
				QString strR = pSCE->getDiskFile(param1, param2, param3);

				strLogedParams = QString("%1, %2, %3").arg(param1).arg(param2).arg(param3);
				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("writeTextToImage"))
			{
				int iIndex = params.value("iIndex").toInt();
				int x = params.value("x").toInt();
				int y = params.value("y").toInt();
				uint cololr = params.value("cololr").toInt();
				uint size = params.value("size").toInt();
				double angle = params.value("angle").toDouble();
				bool isBold = params.value("isBold").toBool();
				bool isUnderline = params.value("isUnderline").toBool();
				bool isOverline = params.value("isOverline").toBool();
				QString font = params.value("font").toString();
				QString text = params.value("text").toString();

				QString strR = pSCE->writeTextToImage(iIndex, x, y, cololr, size, angle, isBold, isUnderline, isOverline, font, text);

				strLogedParams = QString("%1, %2, %3, %4, %5, %6, %7, %8, %9, %10").arg(iIndex).arg(x).arg(y).arg(cololr)
					.arg(size).arg(angle).arg(isBold).arg(isUnderline).arg(isOverline).arg(font).arg(text);
				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("drawImageToImage"))
			{
				int iIndex = params.value("iIndex").toInt();
				int x = params.value("x").toInt();
				int y = params.value("y").toInt();
				bool rotCenter = params.value("rotCenter").toBool();
				double rot = params.value("rot").toDouble();
				double scale = params.value("scale").toDouble();
				QString strWatermark = params.value("strWatermark").toString();

				QString strR = pSCE->drawImageToImage(iIndex, x, y, rotCenter, rot, scale, strWatermark);

				strLogedParams = QString("%1, %2, %3, %4, %5, %6, %7").arg(iIndex).arg(x).arg(y).arg(rotCenter)
					.arg(rot).arg(scale).arg(strWatermark);
				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("rotateImage"))
			{
				int iIndex = params.value("iIndex").toInt();
				double rot = params.value("rot").toDouble();
				QString strR = pSCE->rotateImage(iIndex, rot);

				strLogedParams = QString("%1, %2").arg(iIndex).arg(rot);
				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("sharpenImage"))
			{
				int iIndex = params.value("iIndex").toInt();
				QString strR = pSCE->sharpenImage(iIndex);

				strLogedParams = QString("%1").arg(iIndex);
				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("rectifyImage"))
			{
				int iIndex = params.value("iIndex").toInt();
				double param = params.value("param").toDouble();
				int iR = pSCE->rectifyImage(iIndex, param);

				strLogedParams = QString("%1, %2").arg(iIndex).arg(param);
				strLogedReturn = QString("%1").arg(iR);

				returnJson.insert(QString::fromLocal8Bit("reply"), iR);
			}
			else if (funName == QString::fromLocal8Bit("clipImage"))
			{
				int iIndex = params.value("iIndex").toInt();
				int x = params.value("x").toInt();
				int y = params.value("y").toInt();
				int width = params.value("width").toInt();
				int height = params.value("height").toInt();
				QString strR = pSCE->clipImage(iIndex, x, y, width, height);

				strLogedParams = QString("%1, %2, %3, %4, %5").arg(iIndex).arg(x).arg(y).arg(width)
					.arg(height);
				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("mosaicImage"))
			{
				int iIndex = params.value("iIndex").toInt();
				int x = params.value("x").toInt();
				int y = params.value("y").toInt();
				int width = params.value("width").toInt();
				int height = params.value("height").toInt();
				double scale = params.value("scale").toDouble();
				QString strR = pSCE->mosaicImage(iIndex, x, y, width, height, scale);

				strLogedParams = QString("%1, %2, %3, %4, %5, %6").arg(iIndex).arg(x).arg(y).arg(width)
					.arg(height).arg(scale);
				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			/*else if (funName == QString::fromLocal8Bit("getColor"))
			{
				uint color = params.value("cololr").toInt();
				uint colorReuslt = pSCE->getColor(color);

				returnJson.insert(QString::fromLocal8Bit("reply"), (qint64)colorReuslt);
			}*/
			/*else if (funName == QString::fromLocal8Bit("getFont"))
			{

			}*/
			else if (funName == QString::fromLocal8Bit("getColor"))
			{
				uint colorReuslt = pSCE->getColor();

				strLogedReturn = QString("%1").arg(colorReuslt);

				returnJson.insert(QString::fromLocal8Bit("reply"), (qint64)colorReuslt);
			}
			else if (funName == QString::fromLocal8Bit("getFont"))
			{
				uint size = params.value("width").toInt();
				bool isBold = params.value("isBold").toBool();
				bool isUnderline = params.value("isUnderline").toBool();
				bool StrikeOut = params.value("StrikeOut").toBool();
				QString font = params.value("font").toString();
				QString strR = pSCE->getFont(size, isBold, isUnderline, StrikeOut, font);

				strLogedParams = QString("%1, %2, %3, %4, %5").arg(size).arg(isBold).arg(isUnderline).arg(StrikeOut)
					.arg(font);
				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("getDeviceList"))
			{
				QString strR = pSCE->getDeviceList();

				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("setDefaultDevice"))
			{
				QString strName = params.value("strName").toString();
				int iR = pSCE->setDefaultDevice(strName);

				strLogedParams = QString("%1").arg(strName);
				strLogedReturn = QString("%1").arg(iR);

				returnJson.insert(QString::fromLocal8Bit("reply"), iR);
			}
			else if (funName == QString::fromLocal8Bit("getDefaultDevice"))
			{
				QString strR = pSCE->getDefaultDevice();

				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("getRemoveInvokeError"))
			{
				QString strR = pSCE->getRemoveInvokeError();
				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("setConnectTimeout"))
			{
				int iT = params.value("iT").toInt();
				pSCE->setConnectTimeout(iT);

				strLogedParams = QString("%1").arg(iT);

				returnJson.insert(QString::fromLocal8Bit("reply"), QJsonValue::Null);
			}
			else if (funName == QString::fromLocal8Bit("setWriteTimeout"))
			{
				int iT = params.value("iT").toInt();
				pSCE->setWriteTimeout(iT);

				strLogedParams = QString("%1").arg(iT);

				returnJson.insert(QString::fromLocal8Bit("reply"), QJsonValue::Null);
			}
			else if (funName == QString::fromLocal8Bit("setReadTimeout"))
			{
				int iT = params.value("iT").toInt();
				pSCE->setReadTimeout(iT);

				strLogedParams = QString("%1").arg(iT);

				returnJson.insert(QString::fromLocal8Bit("reply"), QJsonValue::Null);
			}
			else if (funName == QString::fromLocal8Bit("getFileFullName"))
			{
				int iIndex = params.value("iIndex").toInt();
				QString strR = pSCE->getFileFullName(iIndex);

				strLogedParams = QString("%1").arg(iIndex);
				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("GenerateMicrograph"))
			{
				QString fileName = params.value("fileName").toString();
				double scale = params.value("scale").toDouble();
				QString strR = pSCE->GenerateMicrograph(fileName, scale);

				strLogedParams = QString("%1, %2").arg(fileName).arg(scale);
				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("ScalaImage"))
			{
				QString fileName = params.value("sourceFile").toString();
				double scale = params.value("scale").toDouble();
				QString strR = pSCE->ScalaImage(fileName, scale);

				strLogedParams = QString("%1, %2").arg(fileName).arg(scale);
				strLogedReturn = QString("%1").arg(strR);

				returnJson.insert(QString::fromLocal8Bit("reply"), strR);
			}
			else if (funName == QString::fromLocal8Bit("DeleteMicroGraph"))
			{
				QString fileName = params.value("fileName").toString();
				bool bR = pSCE->DeleteMicroGraph(fileName);

				strLogedParams = QString("%1").arg(fileName);
				strLogedReturn = QString("%1").arg(bR);

				returnJson.insert(QString::fromLocal8Bit("reply"), bR);
			}
			else if (funName == QString::fromLocal8Bit("ShowColorDialog"))
			{
				uint color = params.value("color").toInt();
				pSCE->ShowColorDialog(color);

				strLogedParams = QString("%1").arg(color);

				returnJson.insert(QString::fromLocal8Bit("reply"), QJsonValue::Null);
			}
			else if (funName == QString::fromLocal8Bit("GetSelectColor"))
			{

			}
			else if (funName == QString::fromLocal8Bit("ImageToBase64"))
			{
				QString fileName = params.value("fileName").toString();
				QString base64String;
				GetImageBase64(fileName, base64String);
				returnJson.insert(QString::fromLocal8Bit("reply"), base64String);
			}
			else if (funName == QString::fromLocal8Bit("MultipleImageToBase64"))
			{
				QString fileName = params.value("fileName").toString();
				QStringList fileNameList = fileName.split("|");
				QString base64Assemble;
				for (int i = 0; i < fileNameList.length(); i++)
				{
					QString base64String;
					GetImageBase64(fileNameList[i], base64String);

					base64Assemble += base64String;
					base64Assemble += "|";
				}

				base64Assemble.remove(base64Assemble.length() - 1, 1);

				returnJson.insert(QString::fromLocal8Bit("reply"), base64Assemble);
			}
			else if (funName == QString::fromLocal8Bit("EnableLoggingOutput"))
			{
				bool bEnable = params.value("isEnable").toBool();

				strLogedParams = QString("%1").arg(bEnable);

				g_bRemoteEnable = bEnable;
			}

			if (g_bRemoteEnable)
			{
				QDateTime current_time = QDateTime::currentDateTime();
				QString strTime = current_time.toString("yyyy-MM-dd hh:mm:ss.zzz");
				QString loggingText = QString::fromLocal8Bit("[%1] {To invoke ParseInvoke (name : %2, parameters : %3)} -> {function return (%4)}");


				loggingText = loggingText.arg(strTime).arg(funName).arg(strLogedParams).arg(strLogedReturn);

				g_logWriter->WrtieLogging(g_loggingFile, loggingText, LogWriter::FILELOGGING);
			}
			
		}


		
		//QJsonDocument outjd(returnJson);

		//outText = outjd.toJson();
	}
}

int main(int argc, char** argv)
{
	/*const uint bufferSize = 1024 * 1024 * 30;
	QString fileName = "D:\\fff.jpg";
	
	QFile imageFile(fileName);
	qint64 filesize = imageFile.size();
	if (imageFile.open(QIODevice::ReadOnly))
	{
		QDataStream ds(&imageFile);
		char* buffer = new char[bufferSize];
		memset(buffer, 0, bufferSize);
		if(filesize < bufferSize)
			ds.readRawData(buffer, filesize);

		QByteArray* ba = new QByteArray(buffer, filesize);

		QByteArray base64Code = ba->toBase64();

		QFile outfile("D:\\outfile.jpg");
		outfile.open(QIODevice::WriteOnly);
		QDataStream ts(&outfile);

		QByteArray decodeBase64 = QByteArray::fromBase64(base64Code);
		char* datas = decodeBase64.data();
		
		ts.writeRawData(datas, decodeBase64.size());

		outfile.close();
	}
	return 0;*/

	QApplication app(argc, argv);
	ScanBoxChromeExtension sce;

	g_logWriter->start();

	while (1)
	{
		memset(buffer, 0, OUT_BUFFER_SIZE);
		//FILE* stdinHandle = stdin;
		_setmode(_fileno(stdin), O_BINARY);

#if 0
		int  ch = fgetc(stdin);
		if (ch == EOF) continue;

		char len[4] = { 0 };
		len[0] = ch;
		for (int i = 1; i < 4; i++)
		{
			len[i] = fgetc(stdin);
		}

		int textLen = *(int*)len;

		for (size_t i = 0; i < textLen; i++)
		{
			ch = fgetc(stdin);
			buffer[i] = ch;
		}
#else
		char len[4] = { 0 };
		INT32 textLen = 0;
		if (fread(len, sizeof(char), 4, stdin) == 0)
		{
			break;
		}
		else
		{
			textLen = *(INT32*)len;
			if (fread(buffer, sizeof(char), textLen, stdin) == 0)
			{
				break;
			}
		}
#endif
		//writeLogingA("D:\\logging.txt", "%s", buffer);
		
		QJsonObject returnJson;
		ParseInvoke(&sce, buffer, returnJson);
		
		QByteArray byteResult;
		QJsonDocument outjd(returnJson);

		byteResult = outjd.toJson();
		memset(outBuf, 0, OUT_BUFFER_SIZE);

		if (byteResult.size() <= ONE_MILLION)
		{
			INT32 outlen = byteResult.size();
			size_t sumLen = outlen + 4;
			memcpy_s(outBuf, OUT_BUFFER_SIZE, &outlen, sizeof(outlen));
			memcpy_s(outBuf + sizeof(outlen), OUT_BUFFER_SIZE - sizeof(outlen), byteResult.constData(), byteResult.size());

			_setmode(_fileno(stdout), O_BINARY);
			size_t iwr = fwrite(outBuf, sizeof(char), sumLen, stdout);
			fflush(stdout);
		}
		else
		{
			INT32 outlen = byteResult.size();
			INT32 batchNum = outlen / ONE_MILLION;
			if (outlen % ONE_MILLION > 0)
			{
				batchNum += 1;
			}

			for (int i = 0; i < batchNum; i++)
			{
				INT32 outlen = byteResult.size();

				int midLen = ONE_MILLION;
				if (i + 1 == batchNum)
					midLen = batchNum - i * ONE_MILLION;
				QByteArray midArray = byteResult.mid(i * ONE_MILLION, midLen);

				QJsonObject partJson;
				partJson.insert(QString::fromLocal8Bit("block_count"), batchNum);
				partJson.insert(QString::fromLocal8Bit("block_num"), i);
				QString newJsonString = QString::fromLocal8Bit(midArray);
				partJson.insert(QString::fromLocal8Bit("block_content"), newJsonString);

				QJsonDocument partjd(partJson);

				QByteArray midPartJson = partjd.toJson();

				outlen = midPartJson.size();
				size_t sumLen = outlen + 4;
				memcpy_s(outBuf, OUT_BUFFER_SIZE, &outlen, sizeof(outlen));
				memcpy_s(outBuf + sizeof(outlen), OUT_BUFFER_SIZE - sizeof(outlen), midPartJson.constData(), midPartJson.size());

				_setmode(_fileno(stdout), O_BINARY);
				size_t iwr = fwrite(outBuf, sizeof(char), sumLen, stdout);
				fflush(stdout);
			}
		}
		
	}

	g_logWriter->Active(true);

	return 0;
}
