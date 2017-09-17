#include "kxRenderer.h"
USING_KX
void kxRenderer::init()
{
	kxVector4* camPos = new  kxVector4(0, 40, 100, 1);
	kxVector4* camTarget = new kxVector4(0, 0, 0, 1);
	kxVector4* camDir = new kxVector4(0, 0, 0, 1);

	mCamera = kxCamera(CAM_MODEL_EULER, camPos, camDir, camTarget, 200.0f, 12000.0f, 120.f, WIN_WIDTH, WIN_HEIGHT);
	renderList = new kxRenderList();
}

int kxRenderer::buildMatrix(float thetaX, float thetaY, float thetaZ)
{
	kxMatrix44 mx, my, mz;
	float sinTheta = 0, cosTheta = 0;
	int rotSeq = 0;

	mRot = kxMatrix44();

	if (fabs(thetaX) > EPSILON_E5)
	{
		rotSeq = rotSeq | 1;
	}
	if (fabs(thetaY) > EPSILON_E5)
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

		cosTheta = cosf(DEG_TO_RAD(thetaX));
		sinTheta = sinf(DEG_TO_RAD(thetaX));

		mx = kxMatrix44(
			1, 0, 0, 0,
			0, cosTheta, sinTheta, 0,
			0, -sinTheta, cosTheta, 0,
			0, 0, 0, 1);
		mRot = kxMatrix44(mx);
		return 0;
	}
	break;
	case 2: //y rotation
	{
		cosTheta = cosf(DEG_TO_RAD(thetaY));
		sinTheta = sinf(DEG_TO_RAD(thetaY));

		my = kxMatrix44(
			cosTheta, 0, -sinTheta, 0,
			0, 1, 0, 0,
			sinTheta, 0, cosTheta, 0,
			0, 0, 0, 1);
		mRot = kxMatrix44(my);
		return 0;
	}
	break;
	case 3: //xy rotation
	{
		cosTheta = cosf(DEG_TO_RAD(thetaX));
		sinTheta = sinf(DEG_TO_RAD(thetaX));

		mx = kxMatrix44(
			1, 0, 0, 0,
			0, cosTheta, sinTheta, 0,
			0, -sinTheta, cosTheta, 0,
			0, 0, 0, 1);

		cosTheta = cosf(DEG_TO_RAD(thetaY));
		sinTheta = sinf(DEG_TO_RAD(thetaY));

		my = kxMatrix44(
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
		cosTheta = cosf(DEG_TO_RAD(thetaZ));
		sinTheta = sinf(DEG_TO_RAD(thetaZ));

		mz = kxMatrix44(
			cosTheta, sinTheta, 0, 0,
			-sinTheta, cosTheta, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
		mRot = kxMatrix44(mz);
		return  0;
	}
	break;
	case 5://xz rotation
	{
		cosTheta = cosf(DEG_TO_RAD(thetaX));
		sinTheta = sinf(DEG_TO_RAD(thetaX));

		mx = kxMatrix44(
			1, 0, 0, 0,
			0, cosTheta, sinTheta, 0,
			0, -sinTheta, cosTheta, 0,
			0, 0, 0, 1);

		cosTheta = cosf(DEG_TO_RAD(thetaZ));
		sinTheta = sinf(DEG_TO_RAD(thetaZ));

		mz = kxMatrix44(
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
		cosTheta = cosf(DEG_TO_RAD(thetaY));
		sinTheta = sinf(DEG_TO_RAD(thetaY));

		my = kxMatrix44(
			cosTheta, 0, -sinTheta, 0,
			0, 1, 0, 0,
			sinTheta, 0, cosTheta, 0,
			0, 0, 0, 1);

		cosTheta = cosf(DEG_TO_RAD(thetaZ));
		sinTheta = sinf(DEG_TO_RAD(thetaZ));

		mz = kxMatrix44(
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
		cosTheta = cosf(DEG_TO_RAD(thetaX));
		sinTheta = sinf(DEG_TO_RAD(thetaX));

		mx = kxMatrix44(
			1, 0, 0, 0,
			0, cosTheta, sinTheta, 0,
			0, -sinTheta, cosTheta, 0,
			0, 0, 0, 1);

		cosTheta = cosf(DEG_TO_RAD(thetaY));
		sinTheta = sinf(DEG_TO_RAD(thetaY));

		my = kxMatrix44(
			cosTheta, 0, -sinTheta, 0,
			0, 1, 0, 0,
			sinTheta, 0, cosTheta, 0,
			0, 0, 0, 1);

		cosTheta = cosf(DEG_TO_RAD(thetaZ));
		sinTheta = sinf(DEG_TO_RAD(thetaZ));

		mz = kxMatrix44(
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

void kxRenderer::transform(kxRenderObject * obj, int coord_select, int transform_basis)
{
	switch (coord_select)
	{
	case TRANSFORM_LOCAL_ONLY:
	{
		for (int vertex = 0; vertex < obj->num_vertices; vertex++)
		{
			obj->vlist_local[vertex] = obj->vlist_local[vertex] * mRot;
		}
	}
	break;
	case TRANSFORM_TRANS_ONLY:
	{
		for (int vertex = 0; vertex < obj->num_vertices; vertex++)
		{
			obj->vlist_tran[vertex] = obj->vlist_tran[vertex] * mRot;
		}
	}
	break;
	case TRANSFORM_LOCAL_TO_TRANS:
	{
		for (int vertex = 0; vertex < obj->num_vertices; vertex++)
		{
			obj->vlist_tran[vertex] = obj->vlist_local[vertex] * mRot;
		}
	}
	break;
	default:
		break;
	}
	if (transform_basis)
	{
		obj->ux = obj->ux*mRot;
		obj->uy = obj->uy*mRot;
		obj->uz = obj->uz*mRot;
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
			currPoly->tlist[vertex].w = 1;
			kxVector4 tmp = currPoly->tlist[vertex] * mCamera.mcam;
			currPoly->tlist[vertex] = tmp;
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

int kxRenderer::CullObject(kxRenderObject * obj, int cullFlag)
{
	kxVector4 spherePos;
	spherePos = obj->world_pos*mCamera.mcam;
	if (cullFlag&CULL_OBJECT_Z_PLANE)
	{
		if (((spherePos.z - obj->max_radius) > mCamera.far_clip_z) ||
			((spherePos.z + obj->max_radius) < mCamera.near_clip_z))
		{
			SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
			return (1);
		}
	}
	if (cullFlag&CULL_OBJECT_X_PLANE)
	{
		float z_test = (0.5)*mCamera.viewplane_width*spherePos.z / mCamera.view_dist;

		if (((spherePos.x - obj->max_radius) > z_test) ||
			((spherePos.x + obj->max_radius) < -z_test))
		{
			SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
			return (1);
		}
	}
	if (cullFlag&CULL_OBJECT_Y_PLANE)
	{
		float z_test = (0.5)*mCamera.viewplane_height*spherePos.z / mCamera.view_dist;
		if (((spherePos.y - obj->max_radius) > z_test) ||
			((spherePos.y + obj->max_radius) < -z_test))
		{
			SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
			return (1);
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
		kxVector3 u3, v3, tmp;
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

void kxRenderer::RestLights()
{
	static int first_time = 1;
	memset(lights, 0, MAX_LIGHTS * sizeof(kxLight));
	num_lights = 0;
	first_time = 0;
}

int kxRenderer::InitLight(int index, int _state, int _attr, kxColor _c_ambient, kxColor _c_diffuse, kxColor _c_specular, kxVector4* _pos, kxVector4* _dir, float _kc, float _kl, float _kq, float _spot_inner, float _spot_outer, float _pf)
{
	if (index < 0 || index >= MAX_LIGHTS)
		return(0);
	lights[index].state = _state;
	lights[index].id = index;
	lights[index].attr = _attr;

	lights[index].ambient = _c_ambient;
	lights[index].diffuse = _c_diffuse;
	lights[index].specular = _c_specular;

	lights[index].kc = _kc;
	lights[index].kl = _kl;
	lights[index].kq = _kq;

	lights[index].spotInner = _spot_inner;
	lights[index].spotOuter = _spot_outer;
	lights[index].pf = _pf;

	if (_pos)
	{
		lights[index].pos = *_pos;
	}
	if (_dir)
	{
		lights[index].dir = *_dir;
		lights[index].dir=lights[index].dir.normalize();
	}

	lights[index].spotInner = _spot_inner;
	lights[index].spotOuter = _spot_outer;
	lights[index].pf = _pf;

	return (index);
}
