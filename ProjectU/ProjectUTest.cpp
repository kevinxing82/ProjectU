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
	render = kxRenderer();
	directX =  kxDirectX();
	directX.InitD3D(hWnd);
}

int ProjectUTest::GameShutdown(void * parms)
{

}

int ProjectUTest::GameMain(void * parms)
{

}


