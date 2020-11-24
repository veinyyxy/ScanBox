#include "pch.h"
#include "NativeCommThread.h"

void writeLogingA(const char* fileName, const char* strFormat, ...)
{
	FILE *fp = NULL;
	fopen_s(&fp, fileName, "a");
	///////////////////////////////////////////////////
	int _Result;
	va_list _ArgList;
	va_start(_ArgList, strFormat);
	_Result = _vfprintf_s_l(fp, strFormat, NULL, _ArgList);
	va_end(_ArgList);
	fclose(fp);
}

NativeCommThread::NativeCommThread()
{
}


NativeCommThread::~NativeCommThread()
{
}

void NativeCommThread::run()
{
	Sleep(200);
	char buffer[2048] = { 0 };
	char outBuf[2048] = { 0 };
	while (1)
	{
		memset(buffer, 0, sizeof(buffer));
		//FILE* stdinHandle = stdin;
		_setmode(_fileno(stdin), O_BINARY);

#if 0
		int  ch = fgetc(stdin);
		if (ch == EOF) continue;

		char len[4] = { 0 };
		len[0] = ch;
		for (int i = 1; i < 4; i++)
		{
			len[i] = fgetc(stdin);
		}

		int textLen = *(int*)len;

		for (size_t i = 0; i < textLen; i++)
		{
			ch = fgetc(stdin);
			buffer[i] = ch;
		}
#else
		char len[4] = { 0 };
		size_t textLen = 0;
		if (fread(len, sizeof(char), 4, stdin) == 0)
		{
			break;
		}
		else
		{
			textLen = *(int*)len;
			if (fread(buffer, sizeof(char), textLen, stdin) == 0)
			{
				break;
			}
		}
#endif
		//writeLogingA("D:\\logging.txt", "%s", buffer);
		memset(outBuf, 0, sizeof(outBuf));

		size_t sumLen = textLen + 4;
		memcpy_s(outBuf, sizeof(outBuf), &textLen, sizeof(textLen));
		memcpy_s(outBuf + sizeof(textLen), sizeof(outBuf) - sizeof(textLen), buffer, strlen(buffer));

		_setmode(_fileno(stdout), O_BINARY);
		size_t iwr = fwrite(outBuf, sizeof(char), sumLen, stdout);
		fflush(stdout);
	}
}
