#include "kxRenderer.h"

using namespace KevinX;
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
