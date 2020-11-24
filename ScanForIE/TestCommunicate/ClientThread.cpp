#include "ClientThread.h"
#include <QLocalSocket>
#include <QWaitCondition>
#include "InvokeDefine.h"

void ClientThread::run()
{
	
}

ClientThread::ClientThread()
{
	qRegisterMetaType<QLocalSocket::LocalSocketError>("QLocalSocket::LocalSocketError");
	connect(&localSocket, SIGNAL(connected()), SLOT(slot_connected()));
	connect(&localSocket, SIGNAL(disconnected()), SLOT(slot_disconnected()));
	connect(&localSocket, SIGNAL(error(QLocalSocket::LocalSocketError)), SLOT(slot_error(QLocalSocket::LocalSocketError)));
	//connect(&localSocket, SIGNAL(readyRead()), SLOT(slot_readyRead()));
}

QVariant ClientThread::invokeFuntion(char * funName, char * param)
{
	QVariant result;
	localSocket.setServerName(QString::fromLocal8Bit("\\\\.\\pipe\\ScanServer_1_0"));
	localSocket.connectToServer();
	if (localSocket.waitForConnected())
	{
		char* buffer = InvokeToBuffer(funName, param);
		uint size = sizeof(INVOKE_DATA);
		qint64 writeSize = localSocket.write(buffer, size);
		if (localSocket.waitForBytesWritten())
		{
			if (localSocket.waitForReadyRead())
			{
				QByteArray buffer = localSocket.readAll();
				char* pBuffer = 0;
				pBuffer = buffer.data();
				//P_INVOKE_DATA pInvokeData = (P_INVOKE_DATA)pBuffer;

				P_INVOKE_DATA pInvokeData = (P_INVOKE_DATA)pBuffer;

				if (pInvokeData->param_type == QVariant::String)
				{
					result = QString::fromLocal8Bit(pInvokeData->result);
				}
				else if (pInvokeData->param_type == QVariant::Int)
				{
					result = (int)*pInvokeData->result;
				}
				else
				{

				}
				localSocket.disconnectFromServer();
			}
		}
	}
	localSocket.disconnectFromServer();
	return result;
}

ClientThread::~ClientThread()
{
}


void ClientThread::connectServer()
{
	
}

void ClientThread::slot_connected()
{

}

void ClientThread::slot_disconnected()
{
}

void ClientThread::slot_error(QLocalSocket::LocalSocketError socketError)
{
	qDebug() << socketError;
	localSocket.disconnectFromServer();
}

void ClientThread::slot_stateChanged(QLocalSocket::LocalSocketState socketState)
{
}

void ClientThread::slot_readyRead()
{
	m_callLock.unlock();
}