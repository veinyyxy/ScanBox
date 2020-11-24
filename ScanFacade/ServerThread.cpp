#include "ServerThread.h"
#include "InvokeDefine.h"

void ServerThread::run()
{
}

void ServerThread::slot_connected()
{

}

ServerThread::ServerThread()
{
	qRegisterMetaType<QLocalSocket::LocalSocketError>("QLocalSocket::LocalSocketError");
	//qRegisterMetaType<char*>("char*");
	connect(&localServer, SIGNAL(newConnection()), SLOT(slot_newConnection()));
}

bool ServerThread::startListen()
{
	return localServer.listen("\\\\.\\pipe\\ScanServer_1_0");
}

void ServerThread::connectClientSignals()
{
	connect(localSocket, SIGNAL(connected()), SLOT(slot_connected()));
	connect(localSocket, SIGNAL(disconnected()), SLOT(slot_disconnected()));
	connect(localSocket, SIGNAL(error(QLocalSocket::LocalSocketError)), SLOT(slot_error(QLocalSocket::LocalSocketError)));
	//connect(localSocket, SIGNAL(readyRead()), SLOT(slot_readyRead()));
	//QObject::connect(this, SIGNAL(invokeFuntion(char*, char*)), this, SLOT(slot_remoteInvoke(char*, char*)));
}

ServerThread::~ServerThread()
{
}

void ServerThread::slot_disconnected()
{
}

void ServerThread::slot_error(QLocalSocket::LocalSocketError socketError)
{
	qDebug()<<socketError;
}

void ServerThread::slot_stateChanged(QLocalSocket::LocalSocketState socketState)
{
}

void ServerThread::slot_readyRead()
{
	if (localSocket->bytesAvailable())
	{
		
		
	}
}

void ServerThread::slot_newConnection()
{
	localSocket = localServer.nextPendingConnection();
	connectClientSignals();
	if (localSocket->waitForReadyRead())
	{
		QByteArray buffer = localSocket->readAll();
		char* pBuffer = 0;
		pBuffer = buffer.data();
		P_INVOKE_DATA pInvokeData = (P_INVOKE_DATA)pBuffer;
		QVariant result;
		result = emit invokeFuntion((char*)pInvokeData->name, (char*)pInvokeData->param);
		QVariant::Type rt = result.type();
		switch (rt)
		{
		case QVariant::String:
		{
			pInvokeData->param_type = QVariant::String;
			QString tempStrging = result.toString();
			strcpy_s(pInvokeData->result, tempStrging.toLocal8Bit().data());
			break;
		}
		case QVariant::Int:
		{
			pInvokeData->param_type = QVariant::Int;
			int iR = result.toInt();
			memcpy_s(pInvokeData->result, sizeof(pInvokeData->result), &iR, sizeof(iR));
			break;
		}
		case QVariant::StringList:
		{
			pInvokeData->param_type = QVariant::StringList;
			QStringList stringList = result.toStringList();
			QString tempString;
			for (int i = 0; i < stringList.length(); i++)
			{
				tempString += stringList[i];
				if(i != stringList.length() - 1)
					tempString += "|";
			}
			
			strcpy_s(pInvokeData->result, tempString.toLocal8Bit().data());
			break;
		}
		default:
			break;
		}
		///////////////////////////////////////////////////////////
		
		localSocket->write((char*)pInvokeData, sizeof INVOKE_DATA);
		if (localSocket->waitForBytesWritten())
		{

		}
	}
	
	//start();
}
