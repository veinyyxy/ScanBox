var webPagePort = null;
var scanBoxPort = null;

//browser.runtime.onMessage.addListener(receiveMessage);

//function receiveMessage(message)
//{
	//if(message.centent == "connect")
//}

browser.runtime.onConnect.addListener(onWebPageConnect)

function OnWebPageMessage(message)
{
	if(scanBoxPort != null)
	{
		scanBoxPort.postMessage(message);
	}
}

function onWebPageConnect(p)
{
	webPagePort = p;
	webPagePort.onMessage.addListener(OnWebPageMessage);
	webPagePort.onDisconnect.addListener(onWebPageDisconnected);
	
	connectScanBox();
}

function onWebPageDisconnected()
{
	toDisconnect();
	scanBoxPort = null;
}

//////////////////////////////////////////////////////////ScanBox///////////////////////////////////////////////////////////

function synNativeMessage(request, sender, callback)
{
	
	return false;
}

function onNativeMessage(message)
{
	webPagePort.postMessage(message);
}

function onDisconnected()
{
	toDisconnect();
}

function connectScanBox() {
	var hostName = "veiny.scanbox.forfirefox";
	scanBoxPort = browser.runtime.connectNative(hostName);
	scanBoxPort.onMessage.addListener(onNativeMessage);
	scanBoxPort.onDisconnect.addListener(onDisconnected);
}

function toDisconnect()
{
	if(scanBoxPort == null) return;
	scanBoxPort.disconnect();
	scanBoxPort = null;
}
