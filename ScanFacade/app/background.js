var webPagePort = null;
var scanBoxPort = null;

chrome.runtime.onConnectExternal.addListener(onWebPageConnect);
function synWebPageMessage(request, sender, callback)
{
	if(scanBoxPort != null)
	{
		scanBoxPort.sendMessage(message);
	}
	return false;
}

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
	
}

function connectScanBox() {
	var hostName = "veiny.scanbox.forchrome";
	scanBoxPort = chrome.runtime.connectNative(hostName);
	scanBoxPort.onMessage.addListener(onNativeMessage);
	scanBoxPort.onDisconnect.addListener(onDisconnected);
}

function toDisconnect()
{
	scanBoxPort.disconnect();
	scanBoxPort = null;
}