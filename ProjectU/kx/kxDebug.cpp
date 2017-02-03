#include "kxDebug.h"

USING_KX
int kxDebug::OpenLogFile(char * filename, FILE * fpOverride)
{
	if (fpOverride)
	{
		fpLog = fpOverride;
	}
	errno_t err;
	if ((err = fopen_s(&fpLog, filename, "w")) != 0)
	{
		return 0;
	}

	struct _timeb timeBuffer;
	char timeLine[32] = { 0 };
	char timestring[280];

	_ftime_s(&timeBuffer);
	ctime_s(timeLine, sizeof(timeBuffer.time), &(timeBuffer.time));

	sprintf_s(timestring, "%.19s.%hu,%s", timeLine, timeBuffer.millitm, &timeLine[20]);

	WriteLog("\nOpening Log Output File (%s) on %s\n", filename, timestring);
	if (!fpOverride)
	{
		fclose(fpLog);
		if ((err = fopen_s(&fpLog, filename, "a+")) != 0)
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