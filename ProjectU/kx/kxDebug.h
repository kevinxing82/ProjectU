#pragma once
#include<stdio.h>
#include <tchar.h>
#include "kxGlobal.h"
#include <sys/timeb.h>
#include <ctime>
#include "stdarg.h"

KX_BEGIN
#define KX_ERROR(str,...)
#define KX_LOG(str,...)
#define KX_WARN(str,...) 
//#define KX_ERROR(str,...) kxDebug::Error(str,__VA_ARGS__)
//#define KX_LOG(str,...) kxDebug::Log(str,__VA_ARGS__)
//#define KX_WARN(str,...) kxDebug::Warnning(str,__VA_ARGS__)
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
	int WriteLog(char* string, ...);
	
	static kxDebug* GetInstance()
	{
		if (pInstance == NULL)
		{
			pInstance = new kxDebug();
		}
		return pInstance;
	}
	
	static void  Log(char*string, ...)
	{
		va_list argList;
		va_start(argList, string);
		kxDebug::GetInstance()->WriteLog(string, argList);
		va_end(argList);
	}
	void static Error(char* string, ...)
	{
		va_list argList;
		va_start(argList, string);
		kxDebug::GetInstance()->WriteLog(string, argList);
		va_end(argList);
	}

	void static Warnning(char* string, ...)
	{
		va_list argList;
		va_start(argList, string);
		kxDebug::GetInstance()->WriteLog(string, argList);
		va_end(argList);
	}
private:
	static kxDebug* pInstance;
	kxDebug();
	~kxDebug();
	FILE* fpLog;
	int OpenLogFile(char* filename, FILE *fpOverride=NULL);
	int CloseLogFile(void);
};

kxDebug::kxDebug()
{
	OpenLogFile("Log.txt");
}

kxDebug::~kxDebug()
{
	CloseLogFile();
}
KX_END
