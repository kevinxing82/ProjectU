#pragma once
#include <stdio.h>
#include "kx\kx.h"

#define UNIVERSE_RADIUS 4000

#define POINT_SIZE 200
#define NUM_POINT_X (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINT_Z (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS (NUM_POINT_X*NUM_POINT_Z)

#define NUM_TOWERS 96
#define NUM_TANKS 24
#define TANK_SPEED 15

KX_BEGIN
class BattleField
{
public:
	int GameInit(HWND hWnd);
	int GameShutdown(void *parms = NULL);
	int GameMain(void *parms = NULL);
	DWORD GetClock();
	DWORD StartClock();
	DWORD WaitClock(DWORD count);
	void turnLeft();
	void turnRight();
	void speedUp();
	void speedDown();

	kxParser* parser;
	kxRenderer* render;
	kxDirectX*  directX;
	kxDrawer* drawer;

	float x_ang, y_ang, z_ang;
	DWORD start_clock_count;
	kxVector4 polyPos = kxVector4(0, 0, 400, 1);
	kxVector4 vscale =  kxVector4(1.0, 1.0, 1.0, 1);
	kxVector4 vpos =  kxVector4(0, 0, 0, 1);
	kxVector4 vrot =  kxVector4(0, 0, 0, 1);
	kxRenderObject 	  obj_tower, obj_tank, obj_marker, obj_player;
	kxVector4 towers[NUM_TOWERS];
	kxVector4 tanks[NUM_TANKS];

	float turning = 0;
	int tankSpeed = 0;
};
KX_END

