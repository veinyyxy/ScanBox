#pragma once
#include <qdialog.h>
#include "ui_MatchersDialog.h"

class MatchersDialog :
	public QDialog
{
public:
	MatchersDialog();
	~MatchersDialog();

	void parseJsonFile(const QString & fileName, const QString& parentKey, const QString& subKey, QStringList& strList, int iS);
	virtual int exec();
private:
	Ui_MatchersDialog m_setup;
};

