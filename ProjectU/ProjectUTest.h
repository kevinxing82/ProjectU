#pragma once
#include <stdio.h>
#include "kx\kx.h"

namespace KevinX
{
	class ProjectUTest
	{
	public:
		int GameInit(HWND hWnd);
		int GameShutdown(void *parms = NULL);
		int GameMain(void *parms = NULL);
		DWORD GetClock();
		DWORD StartClock();
		DWORD WaitClock(DWORD count);

		kxParser* parser;
		kxRenderer* render;
		kxDirectX*  directX;

		float x_ang, y_ang, z_ang;
		DWORD start_clock_count;
		kxVector4 polyPos = kxVector4(0, 0, 40, 1);
		kxVector4* vscale = new kxVector4(1.0,1.0,1.0,1);
		kxVector4* vpos = new kxVector4(0,0,0,1);
		kxVector4* vrot = new kxVector4(0,0,0,1);
	};
}

