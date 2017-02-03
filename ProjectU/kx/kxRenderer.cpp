#include "kxRenderer.h"
USING_KX
void kxRenderer::init()
{
	kxVector4* camPos = new  kxVector4(0, 0, 0, 1);
	kxVector4* camTarget =  new kxVector4(0, 0, 0, 1);
	kxVector4* camDir =  new kxVector4(0, 0, 0, 1);

	mCamera = kxCamera(CAM_MODEL_EULER, camPos, camDir, camTarget, 200.0f, 12000.0f, 120.f, WIN_WIDTH, WIN_HEIGHT);
	renderList = new kxRenderList();
}

int kxRenderer::buildMatrix(float thetaX, float thetaY, float thetaZ)
{
	kxMatrix4 mx, my, mz, mtmp;
	float sinTheta = 0, cosTheta = 0;
	int rotSeq = 0;

	mRot = kxMatrix4();

	if (fabs(thetaX) > EPSILON_E5)
	{
		rotSeq = rotSeq | 1;
	}
	if(fabs(thetaY) > EPSILON_E5)
	{
		rotSeq = rotSeq | 2;
	}
	if (fabs(thetaZ) > EPSILON_E5)
	{
		rotSeq = rotSeq | 4;
	}

	switch (rotSeq)
	{
	case 0:
	{
		return 0;
	}
	break;
	case 1: // x rotation
	{
		cosTheta = cosf(thetaX);
		sinTheta = sinf(thetaX);

		mx = kxMatrix4(
			1, 0, 0, 0,
			0, cosTheta, sinTheta, 0,
			0,-sinTheta,cosTheta,0,
			0,0,0,1);
		mRot = kxMatrix4(mx);
		return 0;
	}
	break;
	case 2: //y rotation
	{
		cosTheta = cosf(thetaY);
		sinTheta = sinf(thetaY);

		my = kxMatrix4(
			cosTheta,0, -sinTheta, 0,
			0, 1, 0, 0,
			sinTheta,0, cosTheta, 0,
			0, 0, 0, 1);
		mRot = kxMatrix4(my);
		return 0;
	}
	break;
	case 3: //xy rotation
	{
		cosTheta = cosf(thetaX);
		sinTheta = sinf(thetaX);

		mx = kxMatrix4(
			1, 0, 0, 0,
			0, cosTheta, sinTheta, 0,
			0, -sinTheta, cosTheta, 0,
			0, 0, 0, 1);

		cosTheta = cosf(thetaY);
		sinTheta = sinf(thetaY);

		my = kxMatrix4(
			cosTheta, 0, -sinTheta, 0,
			0, 1, 0, 0,
			sinTheta, 0, cosTheta, 0,
			0, 0, 0, 1);

		mRot = mx*my;
		return 0;
	}
	break;
	case 4://z rotation
	{
		cosTheta = cosf(thetaZ);
		sinTheta = sinf(thetaZ);

		mz = kxMatrix4(
			cosTheta,sinTheta, 0,0,
			-sinTheta, cosTheta, 0,0,
			0, 0, 1, 0,
			0, 0, 0, 1);
		mRot = kxMatrix4(mz);
		return  0;
	}
	break;
	case 5://xz rotation
	{
		cosTheta = cosf(thetaX);
		sinTheta = sinf(thetaX);

		mx = kxMatrix4(
			1, 0, 0, 0,
			0, cosTheta, sinTheta, 0,
			0, -sinTheta, cosTheta, 0,
			0, 0, 0, 1);

		cosTheta = cosf(thetaZ);
		sinTheta = sinf(thetaZ);

		mz = kxMatrix4(
			cosTheta, sinTheta, 0, 0,
			-sinTheta, cosTheta, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
		mRot = mx*mz;
		return 0;
	}
	break;
	case 6:	 //yz rotation
	{
		cosTheta = cosf(thetaY);
		sinTheta = sinf(thetaY);

		my = kxMatrix4(
			cosTheta, 0, -sinTheta, 0,
			0, 1, 0, 0,
			sinTheta, 0, cosTheta, 0,
			0, 0, 0, 1);

		cosTheta = cosf(thetaZ);
		sinTheta = sinf(thetaZ);

		mz = kxMatrix4(
			cosTheta, sinTheta, 0, 0,
			-sinTheta, cosTheta, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
		mRot = my*mz;
		return 0;
	}
	break;
	case 7:	 //xyz rotation
	{
		cosTheta = cosf(thetaX);
		sinTheta = sinf(thetaX);

		mx = kxMatrix4(
			1, 0, 0, 0,
			0, cosTheta, sinTheta, 0,
			0, -sinTheta, cosTheta, 0,
			0, 0, 0, 1);

		cosTheta = cosf(thetaY);
		sinTheta = sinf(thetaY);

		my = kxMatrix4(
			cosTheta, 0, -sinTheta, 0,
			0, 1, 0, 0,
			sinTheta, 0, cosTheta, 0,
			0, 0, 0, 1);

		cosTheta = cosf(thetaZ);
		sinTheta = sinf(thetaZ);

		mz = kxMatrix4(
			cosTheta, sinTheta, 0, 0,
			-sinTheta, cosTheta, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
		mRot = mx*my*mz;
		return 0;
	}
	break;
	default:break;
	}
}

int kxRenderer::transform(int coord_select)
{
	switch (coord_select)
	{
	case TRANSFORM_LOCAL_ONLY:
	{
		for (int poly = 0; poly < renderList->num_polys; poly++)
		{
			kxPolygonList*  currPoly = renderList->poly_ptrs[poly];

			if ((currPoly == NULL) || !(currPoly->state&POLY4DV1_STATE_ACTIVE) ||
				(currPoly->state&POLY4DV1_STATE_CLIPPED) ||
				(currPoly->state&POLY4DV1_STATE_BACKFACE))
			{
				continue;
			}

			for (int vertex = 0; vertex < 3; vertex++)
			{
				currPoly->vlist[vertex] = currPoly->vlist[vertex] * mRot;
			}
		}
	}
	break;
	case  TRANSFORM_TRANS_ONLY:
	{
		for (int poly = 0; poly < renderList->num_polys;poly++)
		{
			kxPolygonList* currPoly = renderList->poly_ptrs[poly];

			if ((currPoly == NULL) || !(currPoly->state&POLY4DV1_STATE_ACTIVE) ||
				(currPoly->state&POLY4DV1_STATE_CLIPPED) ||
				(currPoly->state&POLY4DV1_STATE_BACKFACE))
			{
				continue;
			}
			for (int vertex = 0; vertex < 3; vertex++)
			{
				currPoly->tlist[vertex] = currPoly->tlist[vertex] * mRot;
			}
		}
	}
	break;
	case TRANSFORM_LOCAL_TO_TRANS:
	{
		for (int poly = 0; poly < renderList->num_polys; poly++)
		{
			kxPolygonList* currPoly = renderList->poly_ptrs[poly];

			if ((currPoly == NULL) || !(currPoly->state&POLY4DV1_STATE_ACTIVE) ||
				(currPoly->state&POLY4DV1_STATE_CLIPPED) ||
				(currPoly->state&POLY4DV1_STATE_BACKFACE))
			{
				continue;
			}
			for (int vertex = 0; vertex < 3; vertex++)
			{
				currPoly->vlist[vertex] = currPoly->tlist[vertex] * mRot;
			}
		}
	}
	break;
	default:break;
	}
	return 0;
}

int kxRenderer::modelToWorld(const kxVector4& world_pos, int coord_select)
{
	if (coord_select == TRANSFORM_LOCAL_TO_TRANS)
	{
		for (int poly = 0; poly < renderList->num_polys; poly++)
		{
			kxPolygonList* currPoly = renderList->poly_ptrs[poly];

			if ((currPoly == NULL) ||
				!(currPoly->state&POLY4DV1_STATE_ACTIVE) ||
				(currPoly->state&POLY4DV1_STATE_CLIPPED) ||
				(currPoly->state&POLY4DV1_STATE_BACKFACE))
			{
				continue;
			}
			for (int vertex = 0; vertex < 3; vertex++)
			{
				currPoly->tlist[vertex] = currPoly->vlist[vertex] + world_pos;
			}
		}
	}
	else
	{
		for (int poly = 0; poly < renderList->num_polys; poly++)
		{
			kxPolygonList* currPoly = renderList->poly_ptrs[poly];

			if ((currPoly == NULL) || !(currPoly->state&POLY4DV1_STATE_ACTIVE) ||
				(currPoly->state&POLY4DV1_STATE_CLIPPED) ||
				(currPoly->state&POLY4DV1_STATE_BACKFACE))
			{
				continue;
			}
			for (int vertex = 0; vertex < 3; vertex++)
			{
				currPoly->tlist[vertex] = currPoly->tlist[vertex] + world_pos;
			}
		}
	}
	return 0;
}

int kxRenderer::worldToCamera()
{	
	for (int poly = 0; poly < renderList->num_polys; poly++)
	{
		kxPolygonList* currPoly = renderList->poly_ptrs[poly];

		if ((currPoly == NULL) || !(currPoly->state&POLY4DV1_STATE_ACTIVE) ||
			(currPoly->state&POLY4DV1_STATE_CLIPPED) ||
			(currPoly->state&POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}
		for (int vertex = 0; vertex < 3; vertex++)
		{
			currPoly->tlist[vertex] = currPoly->tlist[vertex] * mCamera.mcam;
		}
	}
	return 0;
}

int kxRenderer::cameraToPerspective()
{
	for (int poly = 0; poly < renderList->num_polys; poly++)
	{
		kxPolygonList* currPoly = renderList->poly_ptrs[poly];

		if ((currPoly == NULL) || !(currPoly->state&POLY4DV1_STATE_ACTIVE) ||
			(currPoly->state&POLY4DV1_STATE_CLIPPED) ||
			(currPoly->state&POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}
		for (int vertex = 0; vertex < 3; vertex++)
		{
			float z = currPoly->tlist[vertex].z;
			currPoly->tlist[vertex].x = mCamera.view_dist*currPoly->tlist[vertex].x / z;
			currPoly->tlist[vertex].y = mCamera.view_dist*currPoly->tlist[vertex].y*mCamera.aspect_ratio / z;
		}
	}
	return 0;
}

int kxRenderer::perspectiveToScreen()
{
	for (int poly = 0; poly < renderList->num_polys; poly++)
	{
		kxPolygonList* currPoly = renderList->poly_ptrs[poly];

		if ((currPoly == NULL) ||
			!(currPoly->state&POLY4DV1_STATE_ACTIVE) ||
			(currPoly->state&POLY4DV1_STATE_CLIPPED) ||
			(currPoly->state&POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}
		float alpha = (0.5*mCamera.viewport_width - 0.5);
		float beta = (0.5*mCamera.viewport_height - 0.5);
		for (int vertex = 0; vertex < 3; vertex++)
		{
			currPoly->tlist[vertex].x = alpha + alpha*currPoly->tlist[vertex].x;
			currPoly->tlist[vertex].y = beta - beta*currPoly->tlist[vertex].y;
		}
	}
	return 0;
}

void kxRenderer::RemoveBackfaces()
{
	for (int poly = 0; poly < renderList->num_polys; poly++)
	{
		kxPolygonList* currPoly = renderList->poly_ptrs[poly];

		if ((currPoly == NULL) ||
			!(currPoly->state&POLY4DV1_STATE_ACTIVE) ||
			(currPoly->state&POLY4DV1_STATE_CLIPPED) ||
			(currPoly->state&POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}

		kxVector4 u, v, n;
		u = currPoly->tlist[1] - currPoly->tlist[0];
		u.w = 1;
		v = currPoly->tlist[2] - currPoly->tlist[0];
		v.w = 1;
		kxVector3 u3, v3,tmp;
		u3 = kxVector3(u.x, u.y, u.z);
		v3 = kxVector3(v.x, v.y, v.z);
		tmp = u3.cross(v3);
		n = kxVector4(tmp.x, tmp.y, tmp.z, 1);
		kxVector4 view;
		view = mCamera.pos - currPoly->tlist[0];
		view.w = 1;
		float dp = n.dot(view);
		if (dp <= 0.0)
		{
			SET_BIT(currPoly->state, POLY4DV1_STATE_BACKFACE);
		}
	}
}
