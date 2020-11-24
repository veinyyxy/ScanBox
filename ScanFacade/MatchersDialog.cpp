#include "MatchersDialog.h"
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QDir>

MatchersDialog::MatchersDialog()
{
	m_setup.setupUi(this);

	this->setWindowTitle("Config Chrome Extension");
}


MatchersDialog::~MatchersDialog()
{
}

void MatchersDialog::parseJsonFile(const QString &fileName, const QString& parentKey, const QString& subKey, QStringList& strList, int iS)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
	{

		qDebug() << QString("fail to open the file: %1, %2, %3").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__);
		return;
	}
	QByteArray array = file.readAll();
	
	QJsonParseError jsonParseError;
	QJsonDocument jsonDocument(QJsonDocument::fromJson(array, &jsonParseError));

	if (QJsonParseError::NoError != jsonParseError.error)
	{
		qDebug() << QString("JsonParseError: %1").arg(jsonParseError.errorString());
		return;
	}
	QJsonObject rootObject = jsonDocument.object();

	if (iS)
	{
		QJsonObject::iterator iter = rootObject.find(parentKey);//"externally_connectable"
		//QJsonValue::Type tt = iter->type();

		if (iter == rootObject.end())
		{
			qDebug() << "Dot find.";
			return;
		}

		QJsonArray matchersArray;
		for (int i = 0; i < strList.size(); i++)
		{
			QString strV = strList[i];
			if (strV.length() != 0)
				matchersArray.push_back(QJsonValue(strV));
		}

		if (subKey.length() == 0)
		{
			*iter = matchersArray;
		}
		else
		{
			QJsonObject externally_connectable = iter->toObject();
			QJsonObject::iterator iterMatchers;
			iterMatchers = externally_connectable.find(subKey);//"matches"
			*iterMatchers = matchersArray;
			rootObject.insert(parentKey, externally_connectable);//"externally_connectable"
		}
		
		QJsonDocument writeJsonDoc(rootObject);
		QByteArray resultArray = writeJsonDoc.toJson();

		file.seek(0);
		//file.flush();
		file.resize(resultArray.size());
		file.write(resultArray);

		file.close();
	}
	else
	{
		if (!rootObject.keys().contains(parentKey))//"externally_connectable"
		{
			qDebug() << "No target value";
			return;
		}
		qDebug() << rootObject.keys();

		QJsonValue jsonObject = rootObject.value(parentKey);//"externally_connectable"
		if (!jsonObject.isObject())
		{
			qDebug() << "No json object";
		}

		QJsonValue jsonValue;
		if (subKey.length() == 0)
		{
			jsonValue = jsonObject;//"matches"
		}
		else
		{
			jsonValue = jsonObject.toObject().value(subKey);//"matches"
		}
		
		if (!jsonValue.isArray())
		{
			qDebug() << "No json array";
		}

		QJsonArray jsonArray = jsonValue.toArray();

		for (auto iter = jsonArray.constBegin(); iter != jsonArray.constEnd(); ++iter)
		{
			strList.append(iter->toString());
		}
		//qDebug() << rootObject.keys();
	}
}

int MatchersDialog::exec()
{
	QString configDir = QCoreApplication::applicationDirPath();
	QString strManifest = configDir + QDir::separator() + "app" + QDir::separator() + "manifest.json";
	QString strHostConfig = configDir + QDir::separator() + "scanboxforchrome_win.json";

	QStringList matchersText;
	QStringList allowKeyList;

	parseJsonFile(strManifest, "externally_connectable", "matches", matchersText, 0);
	parseJsonFile(strHostConfig, "allowed_origins", "", allowKeyList, 0);

	for (int i = 0; i < matchersText.size(); i++)
	{
		m_setup.plainTextEdit->insertPlainText(matchersText[i]);
		m_setup.plainTextEdit->insertPlainText("\n");
	}
	
	for (int i = 0; i < allowKeyList.size(); i++)
	{
		m_setup.plainTextEdit_2->insertPlainText(allowKeyList[i]);
		m_setup.plainTextEdit_2->insertPlainText("\n");
	}

	QDialog::DialogCode dc = (QDialog::DialogCode)QDialog::exec();
	if (dc == QDialog::Accepted)
	{
		QString strMatchers = m_setup.plainTextEdit->document()->toPlainText();
		QStringList matchersList = strMatchers.split("\n");

		QString strAllows = m_setup.plainTextEdit_2->document()->toPlainText();
		QStringList allowList = strAllows.split("\n");

		parseJsonFile(strManifest, "externally_connectable", "matches", matchersList, 1);
		parseJsonFile(strHostConfig, "allowed_origins", "", allowList, 1);
	}
	return dc;
}
