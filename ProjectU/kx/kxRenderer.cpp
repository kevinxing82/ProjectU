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

int kxRenderer::lightWorld(kxLight * lights, int maxLights)
{
	unsigned int r_base, g_base, b_base,	  //base color being lit
		r_sum, g_sum, b_sum,						 //sum of lighting process over all lights								  
		shaded_color;									//final color															    
	float dp,									           // dot product
		dist,												 //distance from light to surface
		i,													 // general intensities
		nl,													//length of normal
		atten;											//attenuation computations
	for (int poly = 0; poly < renderList->num_polys; poly++)
	{
		kxPolygonList* currPoly = renderList->poly_ptrs[poly];

		if (!(currPoly->state&POLY4DV1_STATE_ACTIVE) ||
			(currPoly->state&POLY4DV1_STATE_CLIPPED) ||
			(currPoly->state&POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}

		if (currPoly->attr&POLY4DV1_ATTR_SHADE_MODE_FLAT ||
			currPoly->attr&POLY4DV1_ATTR_SHADE_MODE_GOURAUD)
		{
			r_base = currPoly->color.r;
			g_base = currPoly->color.g;
			b_base = currPoly->color.b;

			r_base <<= 3;
			g_base <<= 3;
			b_base <<= 3;

			r_sum = 0;
			g_sum = 0;
			b_sum = 0;

			for (int currLight = 0; currLight < MAX_LIGHTS; currLight++)
			{
				if (lights[currLight].state == LIGHTV1_STATE_OFF)
				{
					continue;
				}

				if (lights[currLight].attr&LIGHTV1_ATTR_AMBIENT)
				{
					r_sum += ((lights[currLight].ambient.r*r_base) / 256);
					g_sum += ((lights[currLight].ambient.g*g_base) / 256);
					b_sum += ((lights[currLight].ambient.b*b_base) / 256);
				}
				else if (lights[currLight].attr&LIGHTV1_ATTR_INFINITE)
				{
					kxVector4 u, v, n;
					u = currPoly->vlist[1] - currPoly->vlist[0];
					u.w = 1;
					v = currPoly->vlist[2] - currPoly->vlist[0];
					v.w = 1;
					kxVector3 tmp;
					tmp = kxVector3(u.x, u.y, u.z).cross(kxVector3(v.x, v.y, v.z));
					n = kxVector4(tmp.x, tmp.y, tmp.z, 1);

					nl = n.length();

					dp = n.dot(lights[currLight].dir);
					if (dp > 0)
					{
						i = 128 * dp / nl;
						r_sum += (lights[currLight].diffuse.r*r_base*i) / (256 * 128);
						g_sum += (lights[currLight].diffuse.g*g_base*i) / (256 * 128);
						b_sum += (lights[currLight].diffuse.b*b_base*i) / (256 * 128);
					}
				}
				else if (lights[currLight].attr&LIGHTV1_ATTR_POINT)
				{
					kxVector4 u, v, n, l;
					u = currPoly->vlist[1] - currPoly->vlist[0];
					u.w = 1;
					v = currPoly->vlist[2] - currPoly->vlist[0];
					v.w = 1;

					kxVector3 tmp;
					tmp = kxVector3(u.x, u.y, u.z).cross(kxVector3(v.x, v.y, v.z));
					n = kxVector4(tmp.x, tmp.y, tmp.z, 1);

					nl =n.length();

					l = lights[currLight].pos - currPoly->vlist[0];
					dist =l.length();

					dp = n.dot(lights[currLight].dir);

					if (dp > 0)
					{
						atten = (lights[currLight].kc + lights[currLight].kl*dist + lights[currLight].kq*dist*dist);
						i = 128 * dp / (nl*dist*atten);
						r_sum += (lights[currLight].diffuse.r*r_base*i) / (256 * 128);
						g_sum += (lights[currLight].diffuse.g*g_base*i) / (256 * 128);
						b_sum += (lights[currLight].diffuse.b*b_base*i) / (256 * 128);
					}
				}
				else if (lights[currLight].attr &LIGHTV1_ATTR_SPOTLIGHT1)
				{
					kxVector4 u, v, n, l;
					u = currPoly->vlist[1] - currPoly->vlist[0];
					u.w = 1;
					v = currPoly->vlist[2] - currPoly->vlist[0];
					v.w = 1;

					kxVector3 tmp;
					tmp = kxVector3(u.x, u.y, u.z).cross(kxVector3(v.x, v.y, v.z));
					n = kxVector4(tmp.x, tmp.y, tmp.z, 1);

					nl = n.length();

					l = lights[currLight].pos - currPoly->vlist[0];
					dist =l.length();

					dp = n.dot(lights[currLight].dir);

					if (dp > 0)
					{
						atten = (lights[currLight].kc + lights[currLight].kl*dist + lights[currLight].kq*dist*dist);
						i = 128 * dp / (nl*atten);
						r_sum += (lights[currLight].diffuse.r*r_base*i) / (256 * 128);
						g_sum += (lights[currLight].diffuse.g*g_base*i) / (256 * 128);
						b_sum += (lights[currLight].diffuse.b*b_base*i) / (256 * 128);
					}
				}
				else if (lights[currLight].attr&LIGHTV1_ATTR_SPOTLIGHT2)
				{
					kxVector4 u, v, n, d, s;
					u = currPoly->vlist[1] - currPoly->vlist[0];
					u.w = 1;
					v = currPoly->vlist[2] - currPoly->vlist[0];
					v.w = 1;
					kxVector3 tmp;
					tmp = kxVector3(u.x, u.y, u.z).cross(kxVector3(v.x, v.y, v.z));
					n = kxVector4(tmp.x, tmp.y, tmp.z, 1);

					nl = n.length();

					dp = n.dot(lights[currLight].dir);

					if (dp > 0)
					{
						s = currPoly->vlist[0] - lights[currLight].pos;
						s.w = 1;
						dist = s.length();

						float dps1 = s.dot(lights[currLight].dir) / dist;
						if (dps1 > 0)
						{
							atten = (lights[currLight].kc + lights[currLight].kl*dist + lights[currLight].kq*dist*dist);
							float dps1_exp = dps1;
							for (int e_index = 1; e_index < (int)lights[currLight].pf; e_index++)
							{
								dps1_exp *= dps1;
							}
							i = 128 * dp*dps1_exp / (nl*atten);

							r_sum += (lights[currLight].diffuse.r*r_base*i) / (256 * 128);
							g_sum += (lights[currLight].diffuse.g*g_base*i) / (256 * 128);
							b_sum += (lights[currLight].diffuse.b*b_base*i) / (256 * 128);
						}
					}
				}
			}
			if (r_sum > 255)r_sum = 255;
			if (g_sum > 255)g_sum = 255;
			if (b_sum > 255)b_sum = 255;	 
			currPoly->color.r = r_sum;
			currPoly->color.g = g_sum;
			currPoly->color.b = b_sum;
		}
		else
		{

		}
	}
	return (1);
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
