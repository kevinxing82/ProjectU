#include "ProjectUTest.h"
#include "kx\kx.h"

using namespace KevinX;

ProjectUTest::ProjectUTest()
{
}

ProjectUTest::~ProjectUTest()
{
}

int ProjectUTest::GameInit(HWND hWnd)
{
	//render = kxRenderer();
	directX =new kxDirectX();
	if (SUCCEEDED(directX->InitD3D(hWnd)))
	{
		return 0;
	}
	return -1;
}

int ProjectUTest::GameShutdown(void * parms)
{
	directX->CleanUp();
	return 0;
}

int ProjectUTest::GameMain(void * parms)
{
	directX->Render();
	return 0;
}


