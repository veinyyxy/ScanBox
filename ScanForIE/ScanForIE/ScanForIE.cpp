#include "ScanForIE.h"
#include <ActiveQt/QAxFactory>
#include <QApplication>
#include <QFileDialog>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QColorDialog>
#include <QFontDialog>
#include <QtMath>
#include <QTranslator>
#include <QSettings>
#include <QMessageBox>
#include <QDateTime>
#include <qmath.h>
#include <iostream>

#include "ClientThread.h"
#include "InvokeDefine.h"

#define CHECK_FORM_ERROR(x) if ((formCode = (x)) != CURL_FORMADD_OK) {fprintf(stderr, "curl_formadd[%d] error./n", formCode);goto out;}
#define CHECK_SETOPT_ERROR(x) if ((code = (x)) != CURLE_OK){fprintf(stderr, "curl_easy_setopt[%d] error./n", code);goto all;}

QAXFACTORY_DEFAULT(ScanForIE,
	"{a65a691d-d673-41b1-ba19-62123e7c4d72}",
	"{e737ac44-8b43-4c8a-a956-6ec80d35188a}",
	"{8065a81b-8251-4f0c-bc88-1abde1a3b492}",
	"{80cd5c4c-fea9-41c7-a447-54728a7b6a3e}",
	"{b5508ffa-a96a-4c48-a52c-5245bec80310}"
)


ScanForIE::ScanForIE(QWidget *parent)
	: QWidget(parent), m_pClientThread(new ClientThread)
{
	ui.setupUi(this);

	QTranslator tran;
	bool ok = tran.load(":/qt_zh_CN.qm");
	//QMessageBox::warning(this, "hehe", QString("%1").arg(ok));

	QCoreApplication* pApp = QApplication::instance();
	ok = pApp->installTranslator(&tran);
	//QMessageBox::warning(this, "hehe", QString("%1").arg(ok));

	QString exeImagePath = QCoreApplication::applicationDirPath();
	QSettings loadConfigure(exeImagePath + QDir::separator() + "Plugin.ini", QSettings::IniFormat);
	loadConfigure.beginGroup("Config");
	int iTimeout = loadConfigure.value("timeout").toInt();
	loadConfigure.endGroup();

	QObject::connect(m_pColorDialog, SIGNAL(colorSelected(const QColor &)), this, SLOT(slot_colorSelected(const QColor &)));

}

void ScanForIE::slot_colorSelected(const QColor &color)
{
	emit ColorValue(color.rgba());
}

int ScanForIE::newScan(const QString& nothing)
{
	QByteArray byteArray = nothing.toLocal8Bit();
	char* pBuffer = byteArray.data();
	QVariant var = m_pClientThread->invokeFuntion(NAME_newScan, pBuffer, nothing.size() + 1, m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);

	return var.toInt();
}

QString ScanForIE::replaceScan()
{
	QVariant var = m_pClientThread->invokeFuntion(NAME_replaceScan, 0, 0, m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);
	return var.toString();
}

QString ScanForIE::insertScan(bool isBehind)
{
	bool bParam = isBehind;
	QVariant var = m_pClientThread->invokeFuntion(NAME_insertScan, (char*)&bParam, sizeof(bParam), m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);
	return var.toString();
}

void ScanForIE::packageImages()
{

}

QString ScanForIE::perviousImage()
{
	QVariant var = m_pClientThread->invokeFuntion(NAME_perviousImage, 0, 0, m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);
	return var.toString();
}

QString ScanForIE::nextImage()
{
	QVariant var = m_pClientThread->invokeFuntion(NAME_nextImage, 0, 0, m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);
	return var.toString();
}

void ScanForIE::SaveConfig()
{
	m_pClientThread->invokeFuntion(NAME_SaveConifg, 0, 0, m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);
}

void ScanForIE::LoadConfig()
{
	m_pClientThread->invokeFuntion(NAME_LoadConfig, 0, 0, m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);
}

void ScanForIE::clearScan()
{
	m_pClientThread->invokeFuntion(NAME_clearScan, 0, 0, m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);
}

int ScanForIE::deleteImage(int startIndex, int iCount)
{
	int param[2] = { 0 };
	param[0] = startIndex;
	param[1] = iCount;
	QVariant var = m_pClientThread->invokeFuntion(NAME_deleteImage, (char*)param, sizeof(param), m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);
	return var.toInt();
}

int ScanForIE::uploadFile(const QString & strFile)
{
	QByteArray byteArray = strFile.toLocal8Bit();
	char* pBuffer = byteArray.data();
	QVariant var = m_pClientThread->invokeFuntion(NAME_uploadFile, pBuffer, strFile.size() + 1, m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);
	return var.toInt();
}

int WriteDate(void *buffer, size_t sz, size_t nmemb, void *ResInfo)
{
	std::string* psResponse = (std::string*)ResInfo;//强制转换
	psResponse->append((char*)buffer, sz * nmemb); //sz*nmemb表示接受数据的多少
	return sz * nmemb;  //返回接受数据的多少
}

QString ScanForIE::uploadAnyFile(const QString & strURL, const QString & strLocalFile)
{
	std::string ResString;
	QByteArray URLbyteArray = strURL.toLocal8Bit();
	char* pURLBuf = URLbyteArray.data();
	QByteArray FileNameByteArray  = strLocalFile.toLocal8Bit();
	char* pFileNameBuf = FileNameByteArray.data();
	CURL *curl = NULL;
	CURLcode code;
	CURLFORMcode formCode;
	int timeout = 15;

	struct curl_httppost *post = NULL;
	struct curl_httppost *last = NULL;
	struct curl_slist *headerlist = NULL;
#if 0
	struct curl_forms array[3];
	array[0].option = CURLFORM_FILE;
	array[0].value = "curl.c";
	array[1].option = CURLFORM_FILE;
	array[1].value = "1.png";
	array[2].option = CURLFORM_END;
	curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, "sendfile",
		CURLFORM_ARRAY, array,
		CURLFORM_END);
#endif
	/*CHECK_FORM_ERROR(curl_formadd(&post, &last, CURLFORM_COPYNAME, "user",
		CURLFORM_COPYCONTENTS, user,
		CURLFORM_END));

	CHECK_FORM_ERROR(curl_formadd(&post, &last, CURLFORM_COPYNAME, "password",
		CURLFORM_COPYCONTENTS, pwd,
		CURLFORM_END));*/

	CHECK_FORM_ERROR(curl_formadd(&post, &last, CURLFORM_COPYNAME, "file",
		CURLFORM_FILE, pFileNameBuf,
		CURLFORM_END));

	/*CHECK_FORM_ERROR(curl_formadd(&post, &last,
		CURLFORM_COPYNAME, "submit",
		CURLFORM_COPYCONTENTS, "upload",
		CURLFORM_END));*/

	curl = curl_easy_init();
	if (curl == NULL)
	{
		fprintf(stderr, "curl_easy_init() error./n");
		goto out;
	}

	
	CHECK_SETOPT_ERROR(curl_easy_setopt(curl, CURLOPT_HEADER, 0));
	CHECK_SETOPT_ERROR(curl_easy_setopt(curl, CURLOPT_URL, pURLBuf));
	CHECK_SETOPT_ERROR(curl_easy_setopt(curl, CURLOPT_HTTPPOST, post));
	CHECK_SETOPT_ERROR(curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout));
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteDate);//WriteDate回调函数
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ResString);//ResString结果写入ResString中

	code = curl_easy_perform(curl);
	if (code != CURLE_OK)
	{
		const char* pErrorDisp = curl_easy_strerror(code);
		fprintf(stderr, "%s", pErrorDisp);
		fprintf(stderr, "curl_easy_perform[%d] error./n", code);
		goto all;
	}


all:
	curl_easy_cleanup(curl);
out:
	curl_formfree(post);

	QString response = QString::fromStdString(ResString);
	return response;
}

int ScanForIE::getFileIndex(const QString & strFile)
{
	QVariant var = -1;
	int iLen = strFile.length();
	if (0 < iLen && iLen < 256)
	{
		std::string strText = strFile.toStdString();
		const char* pBuffer = strText.c_str();
		var = m_pClientThread->invokeFuntion(NAME_getFileIndex, (char*)pBuffer, strFile.size() + 1, m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);
	}
	
	return var.toInt();
}

int ScanForIE::adjustFile(int front, int back)
{
	int param[2] = { 0 };
	param[0] = front;
	param[1] = back;
	QVariant var = m_pClientThread->invokeFuntion(NAME_adjustFile, (char*)param, sizeof(param), m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);

	return var.toInt();
}

int ScanForIE::getCurrentIndex()
{
	QVariant var = m_pClientThread->invokeFuntion(NAME_getCurrentIndex, 0, 0, m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);

	return var.toInt();
}

int ScanForIE::getFileCount()
{
	QVariant var = m_pClientThread->invokeFuntion(NAME_getFileCount, 0, 0, m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);

	return var.toInt();
}

int ScanForIE::setScanFolder(QString strPath)
{
	QVariant result = -1;
	int iLen = strPath.length();
	if (0 < iLen && iLen < 256)
	{
		QByteArray byteArray = strPath.toLocal8Bit();
		char* pBuffer = byteArray.data();
		result = m_pClientThread->invokeFuntion(NAME_setScanFolder, pBuffer, strPath.size() + 1, m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);
	}
	
	return result.toInt();
}

QString ScanForIE::getFileInfo(int iIndex)
{
	QVariant var = m_pClientThread->invokeFuntion(NAME_getFileInfo, (char*)&iIndex, sizeof(iIndex), m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);
	return var.toString();
}

QString ScanForIE::getFileInfoFromNames(QString strFileFullName)
{
	QStringList strFileNames = strFileFullName.split("|");

	int count = strFileNames.size();

	QString strAllInfo = QString::fromLocal8Bit("");

	for (int i = 0; i < count; i++)
	{
		QString strFile = strFileNames[i];
		QFileInfo fi(strFile);
		qint64 int64 = fi.size();
		QDateTime lastDT = fi.lastModified();
		QPixmap image(strFile);

		QString resolution = QString("%1X%2").arg(image.width()).arg(image.height());
		QString dpi = QString("%1").arg(image.toImage().physicalDpiX());// .arg(image.toImage().logicalDpiY());

		QString result = QString::fromLocal8Bit("");
		result += QString("%1").arg(int64);
		result += "|";
		result += lastDT.toString("yyyy-MM-dd HH:mm:ss");
		result += "|";
		result += resolution;
		result += "|";
		result += dpi;

		strAllInfo += result;

		if(i != count - 1)
			strAllInfo += "+";
	}
	return strAllInfo;
}

QString ScanForIE::getFileList()
{
	QVariant var = m_pClientThread->invokeFuntion(NAME_getFileList, 0, 0, m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);
	return var.toString();
}

int ScanForIE::setCurrentIndex(int iIndex)
{
	QVariant var = m_pClientThread->invokeFuntion(NAME_setCurrentIndex, (char*)&iIndex, sizeof(iIndex), m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);
	return var.toInt();
}

int ScanForIE::insertExternalFile(QString strPath, bool isBehind, bool isCopy)
{
	if (strPath.length() > 252)
	{
		return 0;
	}
	//char param[1024 * 1024 * 2] = { 0 };
	char* param = new char[BufferSize];
	memset(param, 0, BufferSize);
	memcpy_s(param, BufferSize/*sizeof(param)*/, &isBehind, sizeof(isBehind));
	memcpy_s(param + sizeof(isCopy), BufferSize/*sizeof(param)*/ - sizeof(isCopy), &isCopy, sizeof(isCopy));
	strcpy_s(param + sizeof(isBehind) + sizeof(isCopy), BufferSize/*sizeof(param)*/ - sizeof(isBehind) - sizeof(isCopy), strPath.toLocal8Bit().data());
	QVariant var = m_pClientThread->invokeFuntion(NAME_insertExternalFile, param, BufferSize/*sizeof(param)*/, m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);

	delete[] param;
	return var.toInt();
}

int ScanForIE::insertExternalFiles(const QString & strFilesList, bool isBehind, bool isCopy)
{
	char* param = new char[BufferSize];
	memset(param, 0, BufferSize);
	//char param[1024 * 1024 * 2] = { 0 };
	memcpy_s(param, BufferSize/*sizeof(param)*/, &isBehind, sizeof(isBehind));
	memcpy_s(param + sizeof(isCopy), BufferSize/*sizeof(param)*/ - sizeof(isCopy), &isCopy, sizeof(isCopy));
	strcpy_s(param + sizeof(isBehind) + sizeof(isCopy), BufferSize/*sizeof(param)*/ - sizeof(isBehind) - sizeof(isCopy), strFilesList.toLocal8Bit().data());
	QVariant var = m_pClientThread->invokeFuntion(NAME_insertExternalFiles, param, BufferSize/*sizeof(param)*/, m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);
	delete[] param;
	return var.toInt();
}

int ScanForIE::replaceCurrentFile(const QString& strPath, bool isCopy)
{
	if (strPath.length() > 252)
	{
		return 0;
	}

	char param[1024 * 104] = { 0 };
	memcpy_s(param, sizeof(param), &isCopy, sizeof(isCopy));
	strcpy_s(param + sizeof(isCopy), sizeof(param) - sizeof(isCopy), strPath.toLocal8Bit().data());
	QVariant var = m_pClientThread->invokeFuntion(NAME_replaceCurrentFile, param, sizeof(param), m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);

	return var.toInt();
}

QString ScanForIE::getDiskFile(const QString &caption, const QString& strDir, const QString& filter)
{
	QStringList fileNames;

	/*QFileDialog openFilesDialog(this, Qt::Popup);
	openFilesDialog.setModal(true);
	openFilesDialog.setNameFilter(filter);
	openFilesDialog.setWindowTitle(caption);
	openFilesDialog.setDirectory(strDir);
	openFilesDialog.setFileMode(QFileDialog::ExistingFiles);
	if (openFilesDialog.exec())
	{
		fileNames = openFilesDialog.selectedFiles();
	}*/
	fileNames = QFileDialog::getOpenFileNames(this, caption, strDir, filter);
	//QString fileName;
	QString tempString;
	for (int i = 0; i < fileNames.length(); i++)
	{
		tempString += fileNames[i];
		if (i != fileNames.length() - 1)
			tempString += "|";
	}
	return tempString;
}

QString ScanForIE::writeTextToImage(int iIndex, int x, int y, uint cololr, uint size, double angle, bool isBold, bool isUnderline, bool StrikeOut, const QString& font, const QString& text)
{
	QString strSrcImage = getFileFullName(iIndex);
	if (strSrcImage.length() == 0) return 0;

	QImage srcImage(strSrcImage);
	QPainter painter(&srcImage); //为这个QImage构造一个QPainter
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);//设置画刷的组合模式CompositionMode_SourceOut这个模式为目标图像在上。
	//改变画笔和字体
	QPen pen = painter.pen();
	QColor color = QColor::fromRgba(cololr);
	pen.setColor(color);
	QFont myFont = painter.font();
	myFont.setUnderline(isUnderline);
	myFont.setStrikeOut(StrikeOut);
	myFont.setBold(isBold);//加粗
	myFont.setPixelSize(size);//改变字体大小
	myFont.setFamily(font);

	painter.setPen(pen);
	painter.setFont(myFont);
	painter.translate(x, y);
	painter.rotate(angle);
	
	painter.drawText(0, 0, text);
	
	//将Hello写在Image的中心
	
	QString newFileName;
	saveImageAs(strSrcImage, srcImage, &newFileName);

	int iResut = insertExternalFile(newFileName, true, false);

	return newFileName;
}

QString ScanForIE::drawImageToImage(int iIndex, int x, int y, bool rotCenter, double rot, double scale, const QString & strWatermark)
{
	QString strSrcImage = getFileFullName(iIndex);
	if (strSrcImage.length() == 0) return 0;

	QImage srcImage(strSrcImage);
	QPainter painter(&srcImage); //为这个QImage构造一个QPainter

	QPixmap markPixmap(strWatermark);
	int iWidth = markPixmap.width() / 2;
	int iHeight = markPixmap.height() / 2;

	QTransform transform;

	if (rotCenter)
	{
		transform.translate(iWidth, iHeight);
		transform.rotate(rot);
		transform.scale(scale, scale);

		markPixmap.transformed(transform);
		QPixmap newPixmap = markPixmap.transformed(transform);

		painter.drawPixmap(x - newPixmap.width()/2, y - newPixmap.height()/2, newPixmap);
	}
	else
	{
		transform.rotate(rot);
		transform.scale(scale, scale);

		markPixmap.transformed(transform);
		QPixmap newPixmap = markPixmap.transformed(transform);

		painter.drawPixmap(x, y, newPixmap);
	}

	//painter.scale(scale, scale);
	//QRectF rect(x, y, markImage.width() * scale, markImage.height() * scale);

	QString newFileName;
	saveImageAs(strSrcImage, srcImage, &newFileName);

	int iResut = insertExternalFile(newFileName, true, false);
	return newFileName;
}

uint ScanForIE::getColor(uint color)
{
	QColor selectColor;
	selectColor = QColorDialog::getColor(color, this);

	return selectColor.rgba();
}

uint ScanForIE::getColor()
{
	QColor selectColor = QColorDialog::getColor(Qt::white, this);

	return selectColor.rgba();
}

QString ScanForIE::getFont()
{
	bool ok;
	QFont selectFont = QFontDialog::getFont(&ok, this);

	QString result;
	result += selectFont.family();
	result += "|";
	result += QString("%1").arg(selectFont.pointSize());
	result += "|";
	result += QString("%1").arg(selectFont.bold());
	result += "|";
	result += QString("%1").arg(selectFont.underline());
	result += "|";
	result += QString("%1").arg(selectFont.strikeOut());
	return result;
}

QString ScanForIE::getFont(uint size, bool isBold, bool isUnderline, bool StrikeOut, const QString& font)
{
	bool ok;

	QFont myFont;
	myFont.setUnderline(isUnderline);
	myFont.setStrikeOut(StrikeOut);
	myFont.setBold(isBold);//加粗
	//myFont.setStyle();
	myFont.setPointSize(size);//改变字体大小
	myFont.setFamily(font);
	
	QFont selectFont = QFontDialog::getFont(&ok, myFont, this);

	QString result;
	result += selectFont.family();
	result += "|";
	result += QString("%1").arg(selectFont.pointSize());
	result += "|";
	result += QString("%1").arg(selectFont.bold());
	result += "|";
	result += QString("%1").arg(selectFont.underline());
	result += "|";
	result += QString("%1").arg(selectFont.strikeOut());
	return result;
}

QString ScanForIE::rotateImage(int iIndex, double rot)
{
	QString strSrcImage = getFileFullName(iIndex);
	if (strSrcImage.length() == 0) return 0;

	QPixmap srcPixmap(strSrcImage);

	QTransform rotTrans;
	rotTrans.rotate(rot);

	QPixmap newPixmap = srcPixmap.transformed(rotTrans);
	
	/*qreal hypotenuse = qSqrt(srcPixmap.width() * srcPixmap.width() + srcPixmap.height() * srcPixmap.height());
	qreal halfWidth = srcPixmap.width() / 2;
	qreal halfHeight = srcPixmap.height() / 2;
	QPixmap pixmap(hypotenuse, hypotenuse);
	QPainter painter(&pixmap);
	painter.setBackground(QBrush(QColor::fromRgba(0xFFFFFFFF)));
	painter.drawPixmap(0, 0, newPixmap);*/
	
	QImage srcImage = newPixmap.toImage();
	QString newFileName;
	saveImageAs(strSrcImage, srcImage, &newFileName);

	int iResut = insertExternalFile(newFileName, true, false);
	
	if (iResut)
	{
		return newFileName;
	}
	else
	{
		return "";
	}
}

QString ScanForIE::sharpenImage(int iIndex)
{
	QString strSrcImage = getFileFullName(iIndex);
	if (strSrcImage.length() == 0) return 0;

	QImage origin(strSrcImage);
	QImage newImage(strSrcImage);

	int kernel[3][3] = { {0,-1,0},
						{-1,5,-1},
						{0,-1,0} };
	int kernelSize = 3;
	int sumKernel = 1;
	int r, g, b;
	QColor color;

	for (int x = kernelSize / 2; x < newImage.width() - (kernelSize / 2); x++) {
		for (int y = kernelSize / 2; y < newImage.height() - (kernelSize / 2); y++) {

			r = 0;
			g = 0;
			b = 0;

			for (int i = -kernelSize / 2; i <= kernelSize / 2; i++) {
				for (int j = -kernelSize / 2; j <= kernelSize / 2; j++) {
					color = QColor(origin.pixel(x + i, y + j));
					r += color.red()*kernel[kernelSize / 2 + i][kernelSize / 2 + j];
					g += color.green()*kernel[kernelSize / 2 + i][kernelSize / 2 + j];
					b += color.blue()*kernel[kernelSize / 2 + i][kernelSize / 2 + j];
				}
			}

			r = qBound(0, r / sumKernel, 255);
			g = qBound(0, g / sumKernel, 255);
			b = qBound(0, b / sumKernel, 255);

			newImage.setPixel(x, y, qRgb(r, g, b));

		}
	}

	QString newFileName;
	saveImageAs(strSrcImage, newImage, &newFileName);

	int iResut = insertExternalFile(newFileName, true, false);
	//newImage.save(strSrcImage);

	if (iResut)
	{
		return newFileName;
	}
	else
	{
		return "";
	}
}

int ScanForIE::rectifyImage(int iIndex, double param)
{
	QString strSrcImage = getFileFullName(iIndex);
	if (strSrcImage.length() == 0) return 0;

	Mat src = imread(strSrcImage.toStdString());
	double rot = CalcDegree(src, param);
	Mat dst;
	CV_rotateImage(src, dst, rot);

	bool iResult = imwrite(strSrcImage.toStdString(), dst);

	return iResult;
}

QString ScanForIE::clipImage(int iIndex, int x, int y, int width, int height)
{
	QString strSrcImage = getFileFullName(iIndex);
	if (strSrcImage.length() == 0) return 0;

	QImage srcImage(strSrcImage);
	QImage newImage = srcImage.copy(x, y, width, height);

	QString newFileName;
	saveImageAs(strSrcImage, newImage, &newFileName);

	int iResut = insertExternalFile(newFileName, true, false);

	if (iResut)
	{
		return newFileName;
	}
	else
	{
		return "";
	}
	
}

QString ScanForIE::mosaicImage(int iIndex, int x, int y, int width, int height, double scale)
{
	QString strSrcImage = getFileFullName(iIndex);
	if (strSrcImage.length() == 0) return 0;

	QImage origin(strSrcImage);
	
	QPainter backgroudImage(&origin);
	QImage mosaic = origin.copy(x, y, width, height);

	QPixmap pixmap(width, height);
	QPainter mosaicPixmap(&pixmap);
	mosaicPixmap.drawImage(0, 0, mosaic);

	QTransform trans;
	trans.scale(scale, scale);
	QPixmap newPixmap = pixmap.transformed(trans);
	//pixmap.save("D:\\test.bmp");
	backgroudImage.drawPixmap(QRect(x,y,width,height), newPixmap);

	//origin.save(strSrcImage);

	QString newFileName;
	saveImageAs(strSrcImage, origin, &newFileName);

	int iResut = insertExternalFile(newFileName, true, false);

	if (iResut)
	{
		return newFileName;
	}
	else
	{
		return "";
	}
}

#if 0
int ScanForIE::writeTextToImage(int iIndex, int x, int y, uint cololr, uint size, bool isBold, const QString& font, const QString& text)
{
	char param[1024 * 104] = { 0 };
	size_t l0(sizeof(param)), l1(sizeof(iIndex)), l2(sizeof(x)), l3(sizeof(y)), l4(sizeof(cololr)), l5(sizeof(size)), l6(sizeof(isBold)), l7(sizeof(uint)), l8(font.toLocal8Bit().size());

	memcpy_s(param, l0, &iIndex, l1);
	memcpy_s(param + l1, l0 - l1, &x, l2);
	memcpy_s(param + l1 + l2, l0 - (l1 + l2), &y, l3);
	memcpy_s(param + l1 + l2 + l3, l0 - (l1 + l2 + l3), &cololr, l4);
	memcpy_s(param + l1 + l2 + l3 + l4, l0 - (l1 + l2 + l3 + l4), &size, l5);
	memcpy_s(param + l1 + l2 + l3 + l4 + l5, l0 - (l1 + l2 + l3 + l4 + l5), &isBold, l6);

	uint stringLen = font.toLocal8Bit().size();
	memcpy_s(param + l1 + l2 + l3 + l4 + l5 + l6, l0 - (l1 + l2 + l3 + l4 + l5 + l6), &stringLen, l7);
	strcpy_s(param + l1 + l2 + l3 + l4 + l5 + l6 + l7, stringLen + 1, font.toLocal8Bit().data());

	strcpy_s(param + l1 + l2 + l3 + l4 + l5 + l6 + l7 + l8 + 1, l0 - (l1 + l2 + l3 + l4 + l5 + l6 + l7 + l8 + 1), text.toLocal8Bit().data());

	QVariant var = m_pClientThread->invokeFuntion(NAME_writeTextToImage, param, sizeof(param));
	return var.toInt();
}

int ScanForIE::drawImageToImage(int iIndex, int x, int y, const QString& strWatermark)
{
	char param[1024 * 104] = { 0 };

	char* pCurAddress = param;

	memcpy_s(pCurAddress, sizeof(iIndex), &iIndex, sizeof(iIndex));

	pCurAddress += sizeof(iIndex);
	memcpy_s(pCurAddress, sizeof(x), &x, sizeof(x));

	pCurAddress += sizeof(x);
	memcpy_s(pCurAddress, sizeof(y), &y, sizeof(y));

	pCurAddress += sizeof(y);
	QByteArray ba = strWatermark.toLocal8Bit();
	strcpy_s(pCurAddress, ba.size() + 1, ba.data());

	QVariant var = m_pClientThread->invokeFuntion(NAME_drawImageToImage, param, sizeof(param));
	return var.toInt();
}
#endif
QString ScanForIE::getFileFullName(int iIndex)
{
	QVariant var = m_pClientThread->invokeFuntion(NAME_getFileFullName, (char*)&iIndex, sizeof(iIndex), m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);
	return var.toString();
}

QString ScanForIE::GenerateMicrograph(const QString & fileName, float scala)
{
	QFileInfo sourceFile(fileName);
	QString strFileName = sourceFile.fileName();
	QString strParentPath = sourceFile.absolutePath();

	QString strMicrographDir = strParentPath + QDir::separator() + "Micrograph_Image";
	QDir micrographPath(strMicrographDir);
	if (!micrographPath.exists())
	{
		micrographPath.mkdir(strMicrographDir);
		if (!micrographPath.exists())
			return "";
	}

	//QFile file(fileName);

	QImage image(fileName);

	QString newFileName = strMicrographDir + QDir::separator() + strFileName;

	QImage newImage = image.scaled(image.width() * scala, image.height() * scala);

	if (newImage.save(newFileName))
		return newFileName;
	else
		return "";
}

QString ScanForIE::ScalaImage(const QString & sourceFile, float scala)
{
	QFileInfo fi(sourceFile);
	QImage image(sourceFile);
	QImage newImage = image.scaled(image.width() * scala, image.height() * scala);
	QString outDir = QString::fromLocal8Bit("C:/Windows/Temp/");
#if defined(Q_QS_WIN)
	outDir = QString::fromLocal8Bit("C:/Windows/Temp/");
#elif defined(Q_QS_LINUX)
	outDir = QString::fromLocal8Bit("/tmp/");
#endif
	QUuid uuidCreater = QUuid::createUuid();
	QString strUUIDName = uuidCreater.toString();

	QString newFileName = outDir + strUUIDName + "." + fi.suffix();

	if (newImage.save(newFileName))
		return newFileName;
	else
		return QString();
		
}

bool ScanForIE::DeleteMicroGraph(const QString & fileName)
{
	QFile deleteFile(fileName);

	if (deleteFile.remove())
	{
		return true;
	}
	return false;
}

void ScanForIE::ShowColorDialog(uint color)
{
	m_pColorDialog->setCurrentColor(QColor(color));
	m_pColorDialog->show();
}

uint ScanForIE::GetSelectColor()
{
	uint color = m_pColorDialog->currentColor().rgba();
	m_pColorDialog->hide();
	return color;
}

void ScanForIE::enableLoggingOutput(bool bE)
{
	m_pClientThread->writeLogging(bE);
}

QString ScanForIE::getDeviceList()
{
	QVariant var = m_pClientThread->invokeFuntion(NAME_getDeviceList, 0, 0, m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);
	return var.toString();
}

int ScanForIE::setDefaultDevice(const QString& strName)
{
	QVariant result = -1;
	int iLen = strName.length();
	if (0 < iLen && iLen < 256)
	{
		QByteArray byteArray = strName.toLocal8Bit();
		char* pBuffer = byteArray.data();
		result = m_pClientThread->invokeFuntion(NAME_setDefaultDevice, pBuffer, strName.size() + 1, m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);
	}

	return result.toInt();
}

QString ScanForIE::getDefaultDevice()
{
	QVariant var = m_pClientThread->invokeFuntion(NAME_getDefaultDevice, 0, 0, m_iConnectTimeout, m_iWriteTimeout, m_iReadTimeout);
	return var.toString();
}

QString ScanForIE::getRemoveInvokeError()
{
	return m_pClientThread->GetRemoveInvokeError();
}

void ScanForIE::setConnectTimeout(int iT)
{
	m_iConnectTimeout = iT;
}

void ScanForIE::setWriteTimeout(int iT)
{
	m_iWriteTimeout = iT;
}

void ScanForIE::setReadTimeout(int iT)
{
	m_iReadTimeout = iT;
}

int ScanForIE::saveImageAs(const QString & strSrcName, const QImage& srcImage, QString* strAsName)
{
	QFileInfo fi(strSrcName);
	QString strPath = fi.absolutePath();
	QUuid uuidCreater = QUuid::createUuid();
	QString strUUIDName = uuidCreater.toString();

	QString newFileName = strPath + QDir::separator() + strUUIDName + "." + fi.suffix();

	int iResult = srcImage.save(newFileName);

	*strAsName = newFileName;

	return iResult;
}

// 通过霍夫变换计算角度vc
double ScanForIE::CalcDegree(Mat& srcImage, int proValue)
{
	Mat midImage, dstImage;
	
	Canny(srcImage, midImage, 50, 200, 3);
	cvtColor(midImage, dstImage, CV_GRAY2BGR);

	//通过霍夫变换检测直线
	std::vector<Vec2f> lines;
	HoughLines(midImage, lines, 1, CV_PI / 180, proValue, 0, 0);//第5个参数就是阈值，阈值越大，检测精度越高
	//cout << lines.size() << endl;

	//由于图像不同，阈值不好设定，因为阈值设定过高导致无法检测直线，阈值过低直线太多，速度很慢
	//所以根据阈值由大到小设置了三个阈值，如果经过大量试验后，可以固定一个适合的阈值。

	if (!lines.size())
	{
		HoughLines(midImage, lines, 1, CV_PI / 180, 200, 0, 0);
	}
	//cout << lines.size() << endl;

	if (!lines.size())
	{
		HoughLines(midImage, lines, 1, CV_PI / 180, 150, 0, 0);
	}

	if (!lines.size())
	{
		qDebug() << QString::fromLocal8Bit("没有检测到直线！")<<endl;
	}

	float sum = 0;
	size_t count = 1;
	//依次画出每条线段
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0];
		float theta = lines[i][1];
		Point pt1, pt2;
		//cout << theta << endl;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		//只选角度最小的作为旋转角度
		if (theta > 0 && theta < 0.7)
		{
			sum += theta;
			count++;
		}
		
	}
	float average = sum / count; //对所有角度求平均，这样做旋转效果会更好
	double angle = average / CV_PI * 180;
	return angle;
}

//逆时针旋转图像degree角度（原尺寸）    
void ScanForIE::CV_rotateImage(Mat src, Mat& img_rotate, double degree)
{
	//旋转中心为图像中心    
	Point2f center;
	center.x = float(src.cols / 2.0);
	center.y = float(src.rows / 2.0);
	int length = 0;
	length = sqrt(src.cols*src.cols + src.rows*src.rows);
	//计算二维旋转的仿射变换矩阵  
	Mat M = getRotationMatrix2D(center, degree, 1);
	warpAffine(src, img_rotate, M, Size(length, length), 1, 0, Scalar(255, 255, 255));//仿射变换，背景色填充为白色  
}

