#include "ProjectUTest.h"
#include "kx\kx.h"

using namespace KevinX;

int ProjectUTest::GameInit(HWND hWnd)
{
	render = new  kxRenderer();
	render->init();
	directX = new  kxDirectX();
	directX->InitD3D(hWnd);
	parser = new kxParser();
	return 1;
}

int ProjectUTest::GameShutdown(void * parms)
{
	return 1;
}

int ProjectUTest::GameMain(void * parms)
{
	StartClock();
	render->renderList->Reset();
	//x_ang += 0.1;
	//y_ang+=0.1;
	//z_ang += 0.1;
	kxRenderObject* obj = new kxRenderObject();
	parser->Load_Object_PLG(obj, "tank1.plg",vscale,vpos,vrot);
	render->renderList->Insert(obj,1);
	render->buildMatrix(x_ang, y_ang, z_ang);
	render->transform(TRANSFORM_LOCAL_ONLY);
	render->modelToWorld(polyPos);
	render->mCamera.buildEulerMatrix(CAM_ROT_SEQ_ZYX);
	render->RemoveBackfaces();
	render->worldToCamera();
	render->cameraToPerspective();
	render->perspectiveToScreen();
	directX->Render(*render->renderList);
	//WaitClock(1000);
	return 1;
}

DWORD KevinX::ProjectUTest::GetClock()
{
	return (GetTickCount64());
}

DWORD KevinX::ProjectUTest::StartClock()
{
	return start_clock_count=GetClock();
}

DWORD KevinX::ProjectUTest::WaitClock(DWORD count)
{
	while ((GetClock() - start_clock_count) < count);
	return (GetClock());
}


