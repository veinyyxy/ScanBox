#include "pch.h"
#include "ClientThread.h"
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

QVariant ClientThread::invokeFuntion(const char * funName, char * param, size_t len, int iConnectTimeout, int iWriteTimeout, int iReadTimeout)
{
	m_strError = "success";

	QVariant result;
	localSocket.setServerName(QString::fromLocal8Bit("\\\\.\\pipe\\ScanServer_1_0"));
	localSocket.connectToServer();
	if (localSocket.waitForConnected(iConnectTimeout))
	{
		P_INVOKE_DATA pInvokeDataWrited = new INVOKE_DATA;
		memset(pInvokeDataWrited, 0, sizeof(INVOKE_DATA));
		////////////////////////////////////////////////////////////////////////
		strcpy_s(pInvokeDataWrited->name, funName);
		memcpy_s(pInvokeDataWrited->param, sizeof(pInvokeDataWrited->param), param, len);
		////////////////////////////////////////////////////////////////////////
		uint size = sizeof(INVOKE_DATA);
		qint64 writeSize = localSocket.write((char*)pInvokeDataWrited, size);

		if (localSocket.waitForBytesWritten(iWriteTimeout))
		{
			if (localSocket.waitForReadyRead(iReadTimeout))
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
					result = *(int*)(pInvokeData->result);
				}
				else if (pInvokeData->param_type == QVariant::StringList)
				{
					QString listString = QString::fromLocal8Bit(pInvokeData->result);
					result = listString;
				}
				else
				{
					delete pInvokeDataWrited;
					return -103;
				}

				delete pInvokeDataWrited;
				localSocket.disconnectFromServer();
			}
			else
			{
				delete pInvokeDataWrited;
				return -102;
			}
		}
		else
		{
			delete pInvokeDataWrited;
			return -101;
		}
	}
	else
	{
		return -100;
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
	switch (socketError)
	{
	case QLocalSocket::ConnectionRefusedError:
		m_strError = QString::fromLocal8Bit("The connection was refused by the peer (or timed out).");
		break;
	case QLocalSocket::PeerClosedError:
		m_strError = QString::fromLocal8Bit("The remote socket closed the connection. Note that the client socket (i.e., this socket) will be closed after the remote close notification has been sent.");
		break;
	case QLocalSocket::ServerNotFoundError:
		m_strError = QString::fromLocal8Bit("The local socket name was not found.");
		break;
	case QLocalSocket::SocketAccessError:
		m_strError = QString::fromLocal8Bit("The socket operation failed because the application lacked the required privileges.");
		break;
	case QLocalSocket::SocketResourceError:
		m_strError = QString::fromLocal8Bit("The local system ran out of resources (e.g., too many sockets).");
		break;
	case QLocalSocket::SocketTimeoutError:
		m_strError = QString::fromLocal8Bit("The socket operation timed out.");
		break;
	case QLocalSocket::DatagramTooLargeError:
		m_strError = QString::fromLocal8Bit("The datagram was larger than the operating system's limit (which can be as low as 8192 bytes).");
		break;
	case QLocalSocket::ConnectionError:
		m_strError = QString::fromLocal8Bit("An error occurred with the connection.");
		break;
	case QLocalSocket::UnsupportedSocketOperationError:
		m_strError = QString::fromLocal8Bit("The requested socket operation is not supported by the local operating system.");
		break;
	case QLocalSocket::OperationError:
		m_strError = QString::fromLocal8Bit("An operation was attempted while the socket was in a state that did not permit it.");
		break;
	case QLocalSocket::UnknownSocketError:
		m_strError = QString::fromLocal8Bit("An unidentified error occurred.");
		break;
	default:
		m_strError == QString::fromLocal8Bit("success");
		break;
	}
	//qDebug() << socketError;
	localSocket.disconnectFromServer();
}

void ClientThread::slot_stateChanged(QLocalSocket::LocalSocketState socketState)
{
}

void ClientThread::slot_readyRead()
{
	m_callLock.unlock();
}