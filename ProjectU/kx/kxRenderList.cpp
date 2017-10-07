#include "kxRenderList.h"
#include	<algorithm>

USING_KX
void kxRenderList::Reset()
{
	num_polys = 0;
}

int kxRenderList::modelToWorld(const kxVector4 & world_pos, int coord_select)
{
	if (coord_select == TRANSFORM_LOCAL_TO_TRANS)
	{
		for (int poly = 0; poly < num_polys; poly++)
		{
			kxPolygonList* currPoly = poly_ptrs[poly];

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
		for (int poly = 0; poly < num_polys; poly++)
		{
			kxPolygonList* currPoly = poly_ptrs[poly];

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

int kxRenderList::Insert(kxPolygonList * polygonList)
{
	if (num_polys >= RENDERLIST4DV1_MAX_POLYS)
	{
		return (0);
	}
	memcpy_s((void*)&poly_data[num_polys], sizeof(kxPolygonList), (void *)polygonList, sizeof(kxPolygonList));

	if (num_polys == 0)
	{
		poly_data[0].next = NULL;
		poly_data[0].prev = NULL;
	}
	else
	{
		poly_data[num_polys].next = NULL;
		poly_data[num_polys].prev = &poly_data[num_polys - 1];

		poly_data[num_polys - 1].next = &poly_data[num_polys];
	}
	num_polys++;
	return (1);
}

int kxRenderList::Insert(kxPolygon * polygon)
{
	if (num_polys >= RENDERLIST4DV1_MAX_POLYS)
	{
		return (0);
	}
	poly_ptrs[num_polys] = &poly_data[num_polys];

	poly_data[num_polys].state = polygon->state;
	poly_data[num_polys].attr = polygon->attr;
	poly_data[num_polys].color = polygon->color;

	poly_data[num_polys].tlist[0] = kxVector4(polygon->vlist[polygon->vert[0]]);
	poly_data[num_polys].tlist[1] = kxVector4(polygon->vlist[polygon->vert[1]]);
	poly_data[num_polys].tlist[2] = kxVector4(polygon->vlist[polygon->vert[2]]);

	poly_data[num_polys].vlist[0] = kxVector4(polygon->vlist[polygon->vert[0]]);
	poly_data[num_polys].vlist[1] = kxVector4(polygon->vlist[polygon->vert[1]]);
	poly_data[num_polys].vlist[2] = kxVector4(polygon->vlist[polygon->vert[2]]);

	if (num_polys == 0)
	{
		poly_data[0].next = NULL;
		poly_data[0].prev = NULL;
	}
	else
	{
		poly_data[num_polys].next = NULL;
		poly_data[num_polys].prev = &poly_data[num_polys - 1];

		poly_data[num_polys - 1].next = &poly_data[num_polys];
	}
	num_polys++;
	return (1);
}

int kxRenderList::Insert(kxRenderObject * object,int insert_local)
{
	if (!(object->state&OBJECT4DV1_STATE_ACTIVE) ||
		(object->state&OBJECT4DV1_STATE_CULLED) ||
		!(object->state&OBJECT4DV1_STATE_VISIBLE))
	{
		return (0);
	}
	for (int poly = 0; poly < object->num_polys; poly++)
	{
		kxPolygon* currPoly = &object->plist[poly];

		if (!(currPoly->state&POLY4DV1_STATE_ACTIVE) ||
			(currPoly->state&POLY4DV1_STATE_CLIPPED) ||
		    (currPoly->state&POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}

		kxVector4* vlistOld = currPoly->vlist;
		if (insert_local)
		{
			currPoly->vlist = object->vlist_local;
		}
		else
		{
			currPoly->vlist = object->vlist_tran;
		}
		if (!Insert(currPoly))
		{
			currPoly->vlist = vlistOld;
			return 0;
		}
		currPoly->vlist = vlistOld;
	}
	return 1;
}

int kxRenderList::lightWorld(kxLight * lights, int maxLights)
{
	unsigned int r_base, g_base, b_base,	  //base color being lit
		r_sum, g_sum, b_sum,						 //sum of lighting process over all lights								  
		shaded_color;									//final color															    
	float dp,									           // dot product
		dist,												 //distance from light to surface
		i,													 // general intensities
		nl,													//length of normal
		atten;											//attenuation computations

	for (int poly = 0; poly < num_polys; poly++)
	{
		kxPolygonList* currPoly = poly_ptrs[poly];

		if (!(currPoly->state&POLY4DV1_STATE_ACTIVE) ||
			(currPoly->state&POLY4DV1_STATE_CLIPPED) ||
			(currPoly->state&POLY4DV1_STATE_BACKFACE))
		{
			continue;
		}


		if (currPoly->attr&POLY4DV1_ATTR_SHADE_MODE_FLAT ||
			currPoly->attr&POLY4DV1_ATTR_SHADE_MODE_GOURAUD)
		{
			r_base = currPoly->color.getRed();
			g_base = currPoly->color.getGreen();
			b_base = currPoly->color.getBlue();
					   

			//r_base <<= 3;
			//g_base <<= 3;
			//b_base <<= 3;

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
					r_sum += ((lights[currLight].ambient.getRed()*r_base) / 256);
					g_sum += ((lights[currLight].ambient.getGreen()*g_base) / 256);
					b_sum += ((lights[currLight].ambient.getBlue()*b_base) / 256);
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
						r_sum += (lights[currLight].diffuse.getRed()*r_base*i) / (256 * 128);
						g_sum += (lights[currLight].diffuse.getGreen()*g_base*i) / (256 * 128);
						b_sum += (lights[currLight].diffuse.getBlue()*b_base*i) / (256 * 128);
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

					nl = n.length();

					l = lights[currLight].pos - currPoly->vlist[0];
					dist = l.length();

					dp = n.dot(lights[currLight].dir);

					if (dp > 0)
					{
						atten = (lights[currLight].kc + lights[currLight].kl*dist + lights[currLight].kq*dist*dist);
						i = 128 * dp / (nl*dist*atten);
						r_sum += (lights[currLight].diffuse.getRed()*r_base*i) / (256 * 128);
						g_sum += (lights[currLight].diffuse.getGreen()*g_base*i) / (256 * 128);
						b_sum += (lights[currLight].diffuse.getBlue()*b_base*i) / (256 * 128);
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
					dist = l.length();

					dp = n.dot(lights[currLight].dir);

					if (dp > 0)
					{
						atten = (lights[currLight].kc + lights[currLight].kl*dist + lights[currLight].kq*dist*dist);
						i = 128 * dp / (nl*atten);
						r_sum += (lights[currLight].diffuse.getRed()*r_base*i) / (256 * 128);
						g_sum += (lights[currLight].diffuse.getGreen()*g_base*i) / (256 * 128);
						b_sum += (lights[currLight].diffuse.getBlue()*b_base*i) / (256 * 128);
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

							r_sum += (lights[currLight].diffuse.getRed()*r_base*i) / (256 * 128);
							g_sum += (lights[currLight].diffuse.getGreen()*g_base*i) / (256 * 128);
							b_sum += (lights[currLight].diffuse.getBlue()*b_base*i) / (256 * 128);
						}
					}
				}
			}
			if (r_sum > 255)r_sum = 255;
			if (g_sum > 255)g_sum = 255;
			if (b_sum > 255)b_sum = 255;
			currPoly->color.setRGBA(r_sum, g_sum, b_sum, 1);
		}
		else
		{
			//currPoly->color.setRGBA((int)(currPoly->color.getRGBA() << 16 | currPoly->color.getRGBA()));
		}
	}
	return (1);
}

void kxRenderList::Sort(int method)
{
	switch (method)
	{
	case SORT_POLYLIST_AVGZ:
	{
		qsort((void*)poly_ptrs, num_polys, sizeof(kxPolygonList*), CompareAvgZ);
	}
	break;
	case SORT_POLYLIST_NEARZ:
	{
		qsort((void*)poly_ptrs, num_polys, sizeof(kxPolygonList*), CompareNearZ);
	}
	break;
	case SORT_POLYLIST_FARZ:
	{
		qsort((void*)poly_ptrs, num_polys, sizeof(kxPolygonList*), CompareFarZ);
	}
	break;
	default:
		break;
	}
}

int kxRenderList::CompareAvgZ(const void * arg1, const void * arg2)
{
	float z1, z2;
	kxPolygonList* poly1;
	kxPolygonList* poly2;
	poly1 =*((kxPolygonList**)(arg1));
	poly2 = *((kxPolygonList**)(arg2));

	z1 = (float)0.33333*(poly1->tlist[0].z + poly1->tlist[1].z + poly1->tlist[2].z);
	z2 = (float)0.33333*(poly2->tlist[0].z + poly2->tlist[1].z + poly2->tlist[2].z);
	if (z1 > z2)
	{
		return  -1;
	}
	else if (z1 < z2)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int kxRenderList::CompareNearZ(const void * arg1, const void * arg2)
{
	float z1, z2;
	kxPolygonList* poly1;
	kxPolygonList* poly2;
	poly1 = (kxPolygonList*)arg1;
	poly2 = (kxPolygonList*)arg2;

	z1 = MIN(poly1->tlist[0].z, poly1->tlist[1].z);
	z1 = MIN(z1, poly1->tlist[2].z);

	z2 = MIN(poly2->tlist[0].z, poly2->tlist[1].z);
	z2 = MIN(z2, poly2->tlist[2].z);

	if (z1 > z2)
	{
		return -1;
	}
	else if (z1 < z2)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int kxRenderList::CompareFarZ(const void * arg1, const void * arg2)
{
	float z1, z2;
	kxPolygonList* poly1;
	kxPolygonList* poly2;
	poly1 = (kxPolygonList*)arg1;
	poly2 = (kxPolygonList*)arg2;

	z1 = MAX(poly1->tlist[0].z, poly1->tlist[1].z);
	z1 = MAX(z1, poly1->tlist[2].z);

	z2 = MAX(poly2->tlist[0].z, poly2->tlist[1].z);
	z2 = MAX(z2, poly2->tlist[2].z);

	if (z1 > z2)
	{
		return -1;
	}
	else if (z1 < z2)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	return 0;
}