#pragma once

#include <QtCore/qstring.h>
#include <QtCore/qobject.h>
#include <QtCore/QScopedPointer>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include "qftp.h"
/**********************************************************************
 *
 * QFtpPrivate
 *
 *********************************************************************/

class QFtpPrivate
{
	Q_DECLARE_PUBLIC(QFtp)
public:
	friend class QFtp;
	inline QFtpPrivate(QFtp *owner) : close_waitForStateChange(false), state(QFtp::Unconnected),
		transferMode(QFtp::Passive), error(QFtp::NoError), q_ptr(owner)
	{ }

	~QFtpPrivate() { while (!pending.isEmpty()) delete pending.takeFirst(); }

	// private slots
	void _q_startNextCommand();
	void _q_piFinished(const QString&);
	void _q_piError(int, const QString&);
	void _q_piConnectState(int);
	void _q_piFtpReply(int, const QString&);

	int addCommand(QFtpCommand *cmd);

	QFtpPI pi;
	QList<QFtpCommand *> pending;
	bool close_waitForStateChange;
	QFtp::State state;
	QFtp::TransferMode transferMode;
	QFtp::Error error;
	QString errorString;

	QString host;
	quint16 port;
	QString proxyHost;
	quint16 proxyPort;
	QFtp *q_ptr;
};
