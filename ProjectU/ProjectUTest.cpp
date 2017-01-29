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
	render.mRenderList.Reset();
	x_ang = 0;
	y_ang = 1;
	z_ang = 0;
	render.buildMatrix(x_ang, y_ang, z_ang);
	render.transform(TRANSFORM_LOCAL_ONLY);
	render.mCamera.buildEulerMatrix(CAM_ROT_SEQ_ZYX);
	//remove backfaces
	render.worldToCamera();
	render.cameraToPerspective();
	render.perspectiveToScreen();
	//directX.Draw(something);
}


