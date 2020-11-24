var backgroundPort = null;

//window.addEventListener("message", messageDispatch);

//将page消息直接转发到gackground.js
function messageDispatch(event)
{
	let messageText = event.data;
	backgroundPort.postMessage(messageText);
}

function postMessage2(messageText)
{
	backgroundPort.postMessage(messageText);
}
//导出函数用于连接host
function connectNavtiveHost()
{
	backgroundPort = browser.runtime.connect({name:"firefox_scanbox_page"});
	//backgroundPort.onmessage = onBackgroundMessage;
	//backgroundPort.onmessageerror = onMessageError;
	
	backgroundPort.onMessage.addListener(onBackgroundMessage);
}

function disconnectNavtiveHost()
{
	backgroundPort.disconnect();
}

//转发backgroud.js接收到host的回复信息
function onBackgroundMessage(m)
{
	window.postMessage(m);
}

function onMessageError(m)
{
	console.log(m);
}

exportFunction(connectNavtiveHost, window, {defineAs:'connectNavtiveHost'});
exportFunction(postMessage2, window, {defineAs:'postMessage2'});
exportFunction(disconnectNavtiveHost, window, {defineAs:'disconnectNavtiveHost'});
