#pragma once
#include <stdio.h>
#include "kx\kx.h"

namespace KevinX
{
	class ProjectUTest
	{
	public:
		ProjectUTest();
		~ProjectUTest();
		int GameInit(HWND hWnd);
		int GameShutdown(void *parms = NULL);
		int GameMain(void *parms = NULL);

		kxParser parser;
		kxRenderer render;
		kxDirectX  directX;

		float x_ang, y_ang, z_ang;
		kxVector4 polyPos = kxVector4(0, 0, 100, 1);
		kxVector4* vscale = new kxVector4(5.0,5.0,5.0,1);
		kxVector4* vpos = new kxVector4(0,0,0,1);
		kxVector4* vrot = new kxVector4(0,0,0,1);
	};
}

