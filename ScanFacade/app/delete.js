QJsonObject::iterator it = params.begin();
			while (it != params.end())
			{
				switch (it.value().type())
				{
				case QJsonValue::String:
				{
					qDebug() << "Type is string";
					QJsonArray sub = it.value().toArray();
					qDebug() << it.key() << "=" << it.value().toString();
					break;
				}
				case QJsonValue::Array:
				{
					qDebug() << "Type is Array";
					qDebug() << it.key() << "=" << it.value().toArray();
					QJsonArray sub = it.value().toArray();
					qDebug() << "index 1:" << sub.at(0).toDouble();
					qDebug() << "index 2:" << sub.at(1).toString();
					qDebug() << "index 3:" << sub.at(2).toBool();
					break;
				}
				case QJsonValue::Bool:
				{
					qDebug() << "Type is Bool";
					qDebug() << it.key() << "=" << it.value().toBool();
					break;
				}
				case QJsonValue::Double:
				{
					qDebug() << "Type is Double";
					qDebug() << it.key() << "=" << it.value().toDouble();
					break;
				}
				case QJsonValue::Object:
				{
					qDebug() << "Type is Object";
					qDebug() << it.key() << "=" << it.value().toObject();
					break;
				}
				case QJsonValue::Null:
				{
					qDebug() << "Type is Null";
					qDebug() << it.key() << "= NULL";
					break;
				}
				case QJsonValue::Undefined:
				{
					qDebug() << "Type is Undefined";
					break;
				}
				}
				it++;
			}
		}
		

var port = null;

function onExtensionMessage(message)
{
	alert(message);
}

function SendInvokeMessage()
{
	message = document.getElementById('invokeText').value;
	port.postMessage(message);
}

function ConnectScanBoxExtension()
{
	var targetExtensionId = "hnpfkggfnomfockgooccggjdfjpeeijh";
	port = chrome.runtime.connect(targetExtensionId, {"name":"test", "includeTlsChannelId":null});
	port.onMessage.addListener(onExtensionMessage);
	
	/*chrome.runtime.sendMessage(targetExtensionId, "iFrameDataURL",
		function(response)
		{
			if (!response.success)
			handleError(url);
		}
	);*/
}

function disconnect()
{
	port.disconnect();
}