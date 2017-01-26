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

		//kxRenderer render;
		kxDirectX*  directX;
	};
}

