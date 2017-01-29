#include "kxRenderer.h"

USING_KX
kxRenderer::kxRenderer()
{
	kxVector4 camPos = kxVector4(0, 40, 0, 1);
	kxVector4 camTarget = kxVector4(0,0,0,1);
	kxVector4 camDir = kxVector4(0, 0, 0, 1);
	mCamera = kxCamera(CAM_MODEL_EULER,&camPos,&camDir,&camTarget,200.0f,12000.0f,120.f,640,480);
	//parser load
}

kxRenderer::kxRenderer()
{
}

int kxRenderer::buildMatrix(float thetaX, float thetaY, float thetaZ)
{
	return 0;
}

int kxRenderer::transform(int coord_select)
{
	return 0;
}

int kxRenderer::modelToWorld(kxVector4 world_pos, int coord_select)
{
	return 0;
}

int kxRenderer::worldToCamera()
{
	return 0;
}

int kxRenderer::cameraToPerspective()
{
	return 0;
}

int kxRenderer::perspectiveToScreen()
{
	return 0;
}

void kxRenderList::Reset()
{
}

int kxRenderList::Insert(kxPolygonList * polygonList)
{
	return 0;
}

int kxRenderList::Insert(kxPolygon * polygon)
{
	return 0;
}

int kxRenderList::Insert(kxRenderObject * object)
{
	return 0;
}
