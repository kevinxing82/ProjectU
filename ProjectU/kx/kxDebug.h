#pragma once
#include<stdio.h>
#include <tchar.h>
#include "kxGlobal.h"
#include <sys/timeb.h>
#include <ctime>

KX_BEGIN
#define KX_ERROR(str,...) kxDebug::Error(str,__VA_ARGS__)
#define KX_LOG(str,...) kxDebug::Log(str,__VA_ARGS__)
#define KX_WARN(str,...) kxDebug::Warnning(str,__VA_ARGS__)
//	void TRACE(const CHAR* format, ...)
//	{
//#ifdef  _DEBUG
//		static const int BufferLen = 1212;
//		va_list pNextArg;
//		CHAR szMessageBuffer[BufferLen] = { 0 };
//		va_start(pNextArg, format);
//		_vsnprintf_s(szMessageBuffer, BufferLen - 1, format, pNextArg);
//		va_end(pNextArg);
//		OutputDebugStringA(szMessageBuffer);
//#endif //  _DEBUG
//
//	}
class kxDebug
{
public :
	kxDebug();
	~kxDebug();
	void static Log(char*string, ...);
	void static Error(char* string, ...);
	void static Warnning(char* string, ...);
private:
	static kxDebug* mInstance;
	FILE* fpLog;
	int OpenLogFile(char* filename, FILE *fpOverride=NULL);
	int CloseLogFile(void);
	int WriteLog(char* string, ...);
};
KX_END
