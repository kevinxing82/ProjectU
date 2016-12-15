#pragma once
#include <stdio.h>

class ProjectUTest
{
public:
	ProjectUTest();
	~ProjectUTest();
	int GameInit(void *parms = NULL);
	int GameShutdown(void *parms = NULL);
	int GameMain(void *parms = NULL);

	kxRenderer* render;
};
