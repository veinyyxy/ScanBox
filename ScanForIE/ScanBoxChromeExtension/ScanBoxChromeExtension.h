#pragma once

class ClientThread;
using namespace cv;
class ScanBoxChromeExtension : public QWidget
{
	Q_OBJECT

public:
	ScanBoxChromeExtension();
	~ScanBoxChromeExtension();
	int newScan(const QString& nothing);
	QString replaceScan();
	QString insertScan(bool isBehind = false);
	void packageImages();
	QString perviousImage();
	QString nextImage();
	void SaveConfig();
	void LoadConfig();
	void clearScan();
	int deleteImage(int startIndex, int iCount);
	int deleteImage2(const QString & strIndexes);
	int uploadFile(const QString & strFile);
	QString uploadAnyFile(const QString& strURL, const QString& strLocalFile);
	int getFileIndex(const QString& strFile);
	int adjustFile(int front, int back);
	int getCurrentIndex();
	int setCurrentIndex(int iIndex);
	int getFileCount();
	int setScanFolder(QString strPath);
	QString getFileInfo(int iIndex);
	QString getFileInfoFromNames(QString strFileFullName);
	QString getFileList();
	int insertExternalFile(QString strPath, bool isBehind = false, bool isCopy = true);
	int insertExternalFiles(const QString& strFilesList, bool isBehind = false, bool isCopy = true);
	int replaceCurrentFile(const QString& strPath, bool isCopy = true);
	QString getDiskFile(const QString &caption, const QString& strDir, const QString& filter);
	QString writeTextToImage(int iIndex, int x, int y, uint cololr, uint size, double angle, bool isBold, bool isUnderline, bool isOverline, const QString& font, const QString& text);
	QString drawImageToImage(int iIndex, int x, int y, bool rotCenter, double rot, double scale, const QString & strWatermark);
	QString rotateImage(int iIndex, double rot);
	QString sharpenImage(int iIndex);
	int rectifyImage(int iIndex, double param);
	QString clipImage(int iIndex, int x, int y, int width, int height);
	QString mosaicImage(int iIndex, int x, int y, int width, int height, double scale);
	uint getColor(uint color);
	QString getFont(uint size, bool isBold, bool isUnderline, bool StrikeOut, const QString& font);
	uint getColor();
	QString getFont();
	QString getDeviceList();
	int setDefaultDevice(const QString& strName);
	QString getDefaultDevice();
	QString getRemoveInvokeError();
	void setConnectTimeout(int iT);
	void setWriteTimeout(int iT);
	void setReadTimeout(int iT);
	QString getFileFullName(int iIndex);
	QString GenerateMicrograph(const QString& fileName, float scala = 1.0f);
	bool DeleteMicroGraph(const QString& fileName);
	QString ScalaImage(const QString& sourceFile, float scala = 1.0f);
	void ShowColorDialog(uint color);
	uint GetSelectColor();
private:
	QColorDialog* m_pColorDialog = new QColorDialog(NULL);
	double CalcDegree(Mat& srcImage, int proValue);
	void CV_rotateImage(Mat src, Mat & img_rotate, double degree);
	int saveImageAs(const QString & strSrcName, const QImage& srcImage, QString* strAsName);
	ClientThread* m_pClientThread = nullptr;
	int m_iConnectTimeout = 30000;
	int m_iWriteTimeout = 30000;
	int m_iReadTimeout = 30000;
};

