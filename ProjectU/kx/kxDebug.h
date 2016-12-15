#pragma once
#include <Windows.h>
#include<stdio.h>
#include <tchar.h>

namespace KevinX {
	void TRACE(const CHAR* format, ...)
	{
#ifdef  _DEBUG
		static const int BufferLen = 1212;
		va_list pNextArg;
		CHAR szMessageBuffer[BufferLen] = { 0 };
		va_start(pNextArg, format);
		_vsnprintf_s(szMessageBuffer, BufferLen - 1, format, pNextArg);
		va_end(pNextArg);
		OutputDebugStringA(szMessageBuffer);
#endif //  _DEBUG

	}
}
