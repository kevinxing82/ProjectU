#include "BattleField.h"
#include "kx\kx.h"

using namespace KevinX;

int BattleField::GameInit(HWND hWnd)
{
	tankSpeed = TANK_SPEED;
	render = new  kxRenderer();
	render->init();
	directX = new  kxDirectX();
	directX->InitD3D(hWnd);
	directX->SetRender(render);
	parser = new kxParser();
	vscale = kxVector4(0.5, 1.0,0.5, 0);
	parser->Load_Object_PLG(&obj_tower, "tower1.plg", &vscale, &vpos, &vrot);
	vscale = kxVector4(0.75, 0.75, 0.75, 0);
	parser->Load_Object_PLG(&obj_tank, "tank2.plg", &vscale, &vpos, &vrot);
	vscale = kxVector4(0.75, 0.75, 0.75, 0);
	parser->Load_Object_PLG(&obj_player, "tank3.plg", &vscale, &vpos, &vrot);
	vscale = kxVector4(3.0, 3.0, 3.0, 0);
	parser->Load_Object_PLG(&obj_marker, "marker1.plg", &vscale,&vpos, &vrot);
	int index;
	for (index = 0; index < NUM_TANKS; index++)
	{
		tanks[index].x = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		tanks[index].y = 0;
		tanks[index].z = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		tanks[index].w = RAND_RANGE(0, 360);
	}
	for (index = 0; index < NUM_TOWERS; index++)
	{
		towers[index].x = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		towers[index].y = 0;
		towers[index].z = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
	}
	return 1;
}

int BattleField::GameShutdown(void * parms)
{
return 1;
}

int BattleField::GameMain(void * parms)
{
	int index;
			//StartClock();
	render->renderList->Reset();
	render->mCamera.buildEulerMatrix(CAM_ROT_SEQ_ZYX);

	for (index = 0; index < NUM_TANKS; index++)
	{
		obj_tank.Reset();
		render->buildMatrix(0, tanks[index].w, 0);
		render->transform(&obj_tank, TRANSFORM_LOCAL_ONLY, 1);
		obj_tank.world_pos.x = tanks[index].x;
		obj_tank.world_pos.y = tanks[index].y;
		obj_tank.world_pos.z = tanks[index].z;

		if (!render->CullObject(&obj_tank, CULL_OBJECT_XYZ_PLANES))
		{
			obj_tank.ModelToWorld();
			render->renderList->Insert(&obj_tank);
		}
	}

	obj_player.Reset();
	obj_player.world_pos.x = render->mCamera.pos.x +300 * sinf(DEG_TO_RAD(render->mCamera.dir.y));
	obj_player.world_pos.y = render->mCamera.pos.y-70 ;
	obj_player.world_pos.z = render->mCamera.pos.z + 300 * cosf(DEG_TO_RAD(render->mCamera.dir.y));	   

	render->buildMatrix(0, render->mCamera.dir.y + turning, 0);
	render->transform(&obj_player,TRANSFORM_LOCAL_TO_TRANS,1);
	obj_player.ModelToWorld(TRANSFORM_TRANS_ONLY);
	render->renderList->Insert(&obj_player);

	for (index = 0; index < NUM_TOWERS; index++)
	{
		obj_tower.Reset();
		obj_tower.world_pos.x = towers[index].x;
		obj_tower.world_pos.y = towers[index].y;
		obj_tower.world_pos.z = towers[index].z;

		if (!render->CullObject(&obj_tower, CULL_OBJECT_XYZ_PLANES))
		{
			obj_tower.ModelToWorld();
			render->renderList->Insert(&obj_tower);
		}
	}
	srand(13);
	for (int index_x = 0; index_x < NUM_POINT_X; index_x++)
	{
		for (int index_z = 0; index_z < NUM_POINT_Z; index_z++)
		{
			obj_marker.Reset();
			obj_marker.world_pos.x = RAND_RANGE(-100, 100) - UNIVERSE_RADIUS + index_x*POINT_SIZE;
			obj_marker.world_pos.y = -obj_marker.max_radius;
			obj_marker.world_pos.z = RAND_RANGE(-100, 100) - UNIVERSE_RADIUS + index_z*POINT_SIZE;

			if (!render->CullObject(&obj_marker, CULL_OBJECT_XYZ_PLANES))
			{
				obj_marker.ModelToWorld();
				render->renderList->Insert(&obj_marker);
			}
		}
	}
	//render->RemoveBackfaces();
	render->worldToCamera();
	render->cameraToPerspective();
	render->perspectiveToScreen();
	directX->Render(*render->renderList);
	//WaitClock(1000);

	if (turning > 0)
	{
		turning--;
	}
	else if (turning < 0)
	{
		turning++;
	}
	return 1;
}

DWORD KevinX::BattleField::GetClock()
{
	return (GetTickCount64());
}

DWORD KevinX::BattleField::StartClock()
{
	return start_clock_count = GetClock();
}

DWORD KevinX::BattleField::WaitClock(DWORD count)
{
	while ((GetClock() - start_clock_count) < count);
	return (GetClock());
}

void BattleField::turnLeft()
{
	render->mCamera.dir.y += 3;
	if ((turning += 2) > 15)
	{
		turning = 15;
	}
}

void BattleField::turnRight()
{
	render->mCamera.dir.y -= 3;
	if ((turning -= 2) < -15)
	{
		turning = -15;
	}
}

void BattleField::speedUp()
{
	render->mCamera.pos.x  -= tankSpeed*sinf(DEG_TO_RAD(render->mCamera.dir.y));
	render->mCamera.pos.z  += tankSpeed*cosf(DEG_TO_RAD(render->mCamera.dir.y));
}

void BattleField::speedDown()
{
	render->mCamera.pos.x += tankSpeed*sinf(DEG_TO_RAD(render->mCamera.dir.y));
	render->mCamera.pos.z -= tankSpeed*cosf(DEG_TO_RAD(render->mCamera.dir.y));
}




