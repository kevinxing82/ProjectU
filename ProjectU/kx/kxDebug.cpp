#include "kxDebug.h"
#include <stdarg.h>
USING_KX;

kxDebug::kxDebug()
{
	OpenLogFile("Log.txt");
}

kxDebug::~kxDebug()
{
	CloseLogFile();
}

void kxDebug::Log(char * string, ...)
{
	if (mInstance==nullptr)
	{
		mInstance = new kxDebug();
	}
	va_list argList;
	va_start(argList, string);
	mInstance->WriteLog(string, argList);
	va_end(argList);
}

void kxDebug::Error(char * string, ...)
{
	if (mInstance == nullptr)
	{
		mInstance = new kxDebug();
	}
	va_list argList;
	va_start(argList, string);
	mInstance->WriteLog(string, argList);
	va_end(argList);
}

void kxDebug::Warnning(char * string, ...)
{
	if (mInstance == nullptr)
	{
		mInstance = new kxDebug();
	}
	va_list argList;
	va_start(argList, string);
	mInstance->WriteLog(string, argList);
	va_end(argList);
}

int kxDebug::OpenLogFile(char * filename, FILE * fpOverride)
{
	if (fpOverride)
	{
		fpLog = fpOverride;
	}
	if ((fpLog = fopen(filename, "w")) == NULL)
	{
		return 0;
	}

	struct _timeb timeBuffer;
	char* timeLine;
	char timestring[280];

	_ftime(&timeBuffer);
	timeLine = ctime(&(timeBuffer.time));

	sprintf_s(timestring, "%.19s.%hu,%s", timeLine, timeBuffer.millitm, &timeLine[20]);

	WriteLog("\nOpening Log Output File (%s) on %s\n", filename, timestring);
	if (!fpOverride)
	{
		fclose(fpLog);
		if ((fpLog = fopen(filename, "a+")) == NULL)
		{
			return 0;
		}
	}
	return 1;
}

int kxDebug::CloseLogFile(void)
{
	if (fpLog)
	{
		WriteLog("\nClosing Log Ouput File.");
		if (fpLog != stdout || fpLog != stderr)
		{
			fclose(fpLog);
		}
		fpLog = NULL;
		return (1);
	}
	else
	{
		return (0);
	}

	return 0;
}

int kxDebug::WriteLog(char * string, ...)
{
	char buffer[1024];
	va_list argList;
	if (!string || !fpLog)
	{
		return 0;
	}
	va_start(argList, string);
	vsprintf_s(buffer, string, argList);
	va_end(argList);

	fprintf_s(fpLog, buffer);

	fflush(fpLog);
	return (1);
}
