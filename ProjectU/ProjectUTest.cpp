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
	parser = kxParser();
}

int ProjectUTest::GameShutdown(void * parms)
{

}

int ProjectUTest::GameMain(void * parms)
{
	render.renderList.Reset();
	x_ang = 0;
	y_ang = 1;
	z_ang = 0;
	kxRenderObject* obj = new kxRenderObject();
	parser.Load_Object_PLG(obj, "cube1.plg",vscale,vpos,vrot);
	render.renderList.Insert(obj);
	render.buildMatrix(x_ang, y_ang, z_ang);
	render.transform(TRANSFORM_LOCAL_ONLY);
	render.modelToWorld(polyPos);
	render.mCamera.buildEulerMatrix(CAM_ROT_SEQ_ZYX);
	//remove backfaces
	render.worldToCamera();
	render.cameraToPerspective();
	render.perspectiveToScreen();
	directX.Render(render.renderList);
}


