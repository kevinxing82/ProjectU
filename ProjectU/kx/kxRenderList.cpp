#include "kxRenderList.h"
#include	<algorithm>

USING_KX
kxRenderList::kxRenderList()
{
	for (int i = 0; i < RENDERLIST4DV1_MAX_POLYS; i++)
	{
		poly_ptrs[i] = new kxPolygonList();
	}
}
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
				!(currPoly->state&POLY4D_STATE_ACTIVE) ||
				(currPoly->state&POLY4D_STATE_CLIPPED) ||
				(currPoly->state&POLY4D_STATE_BACKFACE))
			{
				continue;
			}
			for (int vertex = 0; vertex < 3; vertex++)
			{
				currPoly->tlist[vertex]->position = currPoly->vlist[vertex]->position + world_pos;
			}
		}
	}
	else
	{
		for (int poly = 0; poly < num_polys; poly++)
		{
			kxPolygonList* currPoly = poly_ptrs[poly];

			if ((currPoly == NULL) || !(currPoly->state&POLY4D_STATE_ACTIVE) ||
				(currPoly->state&POLY4D_STATE_CLIPPED) ||
				(currPoly->state&POLY4D_STATE_BACKFACE))
			{
				continue;
			}
			for (int vertex = 0; vertex < 3; vertex++)
			{
				currPoly->tlist[vertex]->position = currPoly->tlist[vertex]->position + world_pos;
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

	poly_data[num_polys].tlist[0]->position = kxVector4(polygon->vlist[polygon->vert[0]].position);
	poly_data[num_polys].tlist[1]->position = kxVector4(polygon->vlist[polygon->vert[1]].position);
	poly_data[num_polys].tlist[2]->position = kxVector4(polygon->vlist[polygon->vert[2]].position);

	poly_data[num_polys].vlist[0]->position = kxVector4(polygon->vlist[polygon->vert[0]].position);
	poly_data[num_polys].vlist[1]->position = kxVector4(polygon->vlist[polygon->vert[1]].position);
	poly_data[num_polys].vlist[2]->position = kxVector4(polygon->vlist[polygon->vert[2]].position);

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
	if (!(object->state&OBJECT4D_STATE_ACTIVE) ||
		(object->state&OBJECT4D_STATE_CULLED) ||
		!(object->state&OBJECT4D_STATE_VISIBLE))
	{
		return (0);
	}
	for (int poly = 0; poly < object->num_polys; poly++)
	{
		kxPolygon* currPoly = &object->plist[poly];

		if (!(currPoly->state&POLY4D_STATE_ACTIVE) ||
			(currPoly->state&POLY4D_STATE_CLIPPED) ||
		    (currPoly->state&POLY4D_STATE_BACKFACE))
		{
			continue;
		}

		kxVector4* vlistOld = new kxVector4(currPoly->vlist->position);
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
			currPoly->vlist->position = *vlistOld;
			return 0;
		}
		currPoly->vlist->position = *vlistOld;
	}
	return 1;
}

int kxRenderList::lightWorld(kxLight * lights, int maxLights)
{
	unsigned int r_base, g_base, b_base,	  //base color being lit
		r_sum, g_sum, b_sum,						 //sum of lighting process over all lights					
		r_sum0,g_sum0,b_sum0,
		r_sum1,g_sum1,b_sum1,
		r_sum2,g_sum2,b_sum2,
		ri,gi,bi,
		shaded_color;									//final color															    
	float dp,									           // dot product
		dist,												 //distance from light to surface
		dists,
		i,													 // general intensities
		nl,													//length of normal
		atten;											//attenuation computations
	kxVector4 u, v, n, l, d, s;

	for (int poly = 0; poly < num_polys; poly++)
	{
		kxPolygonList* currPoly = poly_ptrs[poly];

		if (!(currPoly->state&POLY4D_STATE_ACTIVE) ||
			(currPoly->state&POLY4D_STATE_CLIPPED) ||
			(currPoly->state&POLY4D_STATE_BACKFACE)||
			(currPoly->state&POLY4D_STATE_LIT))
		{
			continue;
		}

		SET_BIT(currPoly->state, POLY4D_STATE_LIT);

		if (currPoly->attr&POLY4D_ATTR_SHADE_MODE_FLAT )
		{
			r_base = currPoly->color->getRed();
			g_base = currPoly->color->getGreen();
			b_base = currPoly->color->getBlue();
					   

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

				if (lights[currLight].attr&LIGHT_ATTR_AMBIENT)
				{
					r_sum += ((lights[currLight].ambient.getRed()*r_base) / 256);
					g_sum += ((lights[currLight].ambient.getGreen()*g_base) / 256);
					b_sum += ((lights[currLight].ambient.getBlue()*b_base) / 256);
				}
				else if (lights[currLight].attr&LIGHT_ATTR_INFINITE)
				{
					u = currPoly->vlist[1]->position - currPoly->vlist[0]->position;
					u.w = 1;
					v = currPoly->vlist[2]->position - currPoly->vlist[0]->position;
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
				else if (lights[currLight].attr&LIGHT_ATTR_POINT)
				{
					u = currPoly->vlist[1]->position - currPoly->vlist[0]->position;
					u.w = 1;
					v = currPoly->vlist[2]->position - currPoly->vlist[0]->position;
					v.w = 1;

					kxVector3 tmp;
					tmp = kxVector3(u.x, u.y, u.z).cross(kxVector3(v.x, v.y, v.z));
					n = kxVector4(tmp.x, tmp.y, tmp.z, 1);

					nl = n.length();

					l = lights[currLight].pos - currPoly->vlist[0]->position;
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
				else if (lights[currLight].attr &LIGHT_ATTR_SPOTLIGHT1)
				{
					u = currPoly->vlist[1]->position - currPoly->vlist[0]->position;
					u.w = 1;
					v = currPoly->vlist[2]->position - currPoly->vlist[0]->position;
					v.w = 1;

					kxVector3 tmp;
					tmp = kxVector3(u.x, u.y, u.z).cross(kxVector3(v.x, v.y, v.z));
					n = kxVector4(tmp.x, tmp.y, tmp.z, 1);

					nl = n.length();

					l = lights[currLight].pos - currPoly->vlist[0]->position;
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
				else if (lights[currLight].attr&LIGHT_ATTR_SPOTLIGHT2)
				{
					u = currPoly->vlist[1]->position - currPoly->vlist[0]->position;
					u.w = 1;
					v = currPoly->vlist[2]->position - currPoly->vlist[0]->position;
					v.w = 1;
					kxVector3 tmp;
					tmp = kxVector3(u.x, u.y, u.z).cross(kxVector3(v.x, v.y, v.z));
					n = kxVector4(tmp.x, tmp.y, tmp.z, 1);

					nl = n.length();

					dp = n.dot(lights[currLight].dir);

					if (dp > 0)
					{
						s = currPoly->vlist[0]->position - lights[currLight].pos;
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
			currPoly->lit_color[0]->setRGBA(r_sum, g_sum, b_sum, 0);
		}
		else if(currPoly->attr & POLY4D_ATTR_SHADE_MODE_GOURAUD)
		{
		     r_base = currPoly->color->getRed();
			 g_base = currPoly->color->getGreen();
			 b_base = currPoly->color->getBlue();

			 r_sum0 = 0;
			 g_sum0 = 0;
			 b_sum0 = 0;

			 r_sum1 = 0;
			 g_sum1 = 0;
			 b_sum1 = 0;

			 r_sum2 = 0;
			 g_sum2 = 0;
			 b_sum2 = 0;

			 for (int currLight = 0; currLight < MAX_LIGHTS; currLight++)
			 {
				 if (lights[currLight].state == LIGHTV1_STATE_OFF)
				 {
					 continue;
				 }

				 if (lights[currLight].attr & LIGHT_ATTR_AMBIENT)
				 {
					 ri = ((lights[currLight].ambient.getRed() * r_base) / 256);
					 gi = ((lights[currLight].ambient.getGreen() * g_base) / 256);
					 bi = ((lights[currLight].ambient.getBlue() * b_base) / 256);

					 r_sum0 += ri;
					 g_sum0 += gi;
					 b_sum0 += bi;

					 r_sum1 += ri;
					 g_sum1 += gi;
					 b_sum1 += bi;

					 r_sum2 += ri;
					 g_sum2 += gi;
					 b_sum2 += bi;
				 }
				 else if (lights[currLight].attr & LIGHT_ATTR_INFINITE)
				 {
					 dp = currPoly->tlist[0]->normal.dot(lights[currLight].dir);
					 if (dp > 0)
					 {
						 i = 128 * dp ;
						 r_sum0 += (lights[currLight].diffuse.getRed() * r_base * i) / (256 * 128);
						 g_sum0 += (lights[currLight].diffuse.getGreen() * g_base * i) / (256 * 128);
						 b_sum0 += (lights[currLight].diffuse.getBlue() * b_base * i) / (256 * 128);
					 }

					 dp = currPoly->tlist[1]->normal.dot(lights[currLight].dir);
					 if (dp > 0)
					 {
						 i = 128 * dp;
						 r_sum1 += (lights[currLight].diffuse.getRed() * r_base * i) / (256 * 128);
						 g_sum1 += (lights[currLight].diffuse.getGreen() * g_base * i) / (256 * 128);
						 b_sum1 += (lights[currLight].diffuse.getBlue() * b_base * i) / (256 * 128);
					 }

					 dp = currPoly->tlist[2]->normal.dot(lights[currLight].dir);
					 if (dp > 0)
					 {
						 i = 128 * dp;
						 r_sum2 += (lights[currLight].diffuse.getRed() * r_base * i) / (256 * 128);
						 g_sum2 += (lights[currLight].diffuse.getGreen() * g_base * i) / (256 * 128);
						 b_sum2 += (lights[currLight].diffuse.getBlue() * b_base * i) / (256 * 128);
					 }
				 }
				 else if (lights[currLight].attr & LIGHT_ATTR_POINT)
				 {
					 l = lights[currLight].pos - currPoly->tlist[0]->position;
					 l.w = 1;
					 dist = l.length();

					 dp = currPoly->tlist[0]->normal.dot(l);
					 if (dp > 0)
					 {
						 atten = (lights[currLight].kc + lights[currLight].kl * dist + lights[currLight].kq * dist * dist);
						 i = 128 * dp / ( dist * atten);
						 r_sum0 += (lights[currLight].diffuse.getRed() * r_base * i) / (256 * 128);
						 g_sum0 += (lights[currLight].diffuse.getGreen() * g_base * i) / (256 * 128);
						 b_sum0 += (lights[currLight].diffuse.getBlue() * b_base * i) / (256 * 128);
					 }

					 dp = currPoly->tlist[1]->normal.dot(l);
					 if (dp > 0)
					 {
						 atten = (lights[currLight].kc + lights[currLight].kl * dist + lights[currLight].kq * dist * dist);
						 i = 128 * dp / (dist * atten);
						 r_sum1 += (lights[currLight].diffuse.getRed() * r_base * i) / (256 * 128);
						 g_sum1 += (lights[currLight].diffuse.getGreen() * g_base * i) / (256 * 128);
						 b_sum1 += (lights[currLight].diffuse.getBlue() * b_base * i) / (256 * 128);
					 }

					 dp = currPoly->tlist[2]->normal.dot(l);
					 if (dp > 0)
					 {
						 atten = (lights[currLight].kc + lights[currLight].kl * dist + lights[currLight].kq * dist * dist);
						 i = 128 * dp / (dist * atten);
						 r_sum2 += (lights[currLight].diffuse.getRed() * r_base * i) / (256 * 128);
						 g_sum2 += (lights[currLight].diffuse.getGreen() * g_base * i) / (256 * 128);
						 b_sum2 += (lights[currLight].diffuse.getBlue() * b_base * i) / (256 * 128);
					 }
				 }
				 else if (lights[currLight].attr & LIGHT_ATTR_SPOTLIGHT1)
				 {
					 l = lights[currLight].pos - currPoly->tlist[0]->position;
					 dist = l.length();

					 dp = currPoly->tlist[0]->normal.dot(lights[currLight].dir);
					 if (dp > 0)
					 {
						 atten = (lights[currLight].kc + lights[currLight].kl * dist + lights[currLight].kq * dist * dist);
						 i = 128 * dp / ( atten);
						 r_sum0 += (lights[currLight].diffuse.getRed() * r_base * i) / (256 * 128);
						 g_sum0 += (lights[currLight].diffuse.getGreen() * g_base * i) / (256 * 128);
						 b_sum0 += (lights[currLight].diffuse.getBlue() * b_base * i) / (256 * 128);
					 }

					 dp = currPoly->tlist[1]->normal.dot(lights[currLight].dir);
					 if (dp > 0)
					 {
						 atten = (lights[currLight].kc + lights[currLight].kl * dist + lights[currLight].kq * dist * dist);
						 i = 128 * dp / (atten);
						 r_sum1 += (lights[currLight].diffuse.getRed() * r_base * i) / (256 * 128);
						 g_sum1 += (lights[currLight].diffuse.getGreen() * g_base * i) / (256 * 128);
						 b_sum1 += (lights[currLight].diffuse.getBlue() * b_base * i) / (256 * 128);
					 }

					 dp = currPoly->tlist[2]->normal.dot(lights[currLight].dir);
					 if (dp > 0)
					 {
						 atten = (lights[currLight].kc + lights[currLight].kl * dist + lights[currLight].kq * dist * dist);
						 i = 128 * dp / (atten);
						 r_sum2 += (lights[currLight].diffuse.getRed() * r_base * i) / (256 * 128);
						 g_sum2 += (lights[currLight].diffuse.getGreen() * g_base * i) / (256 * 128);
						 b_sum2 += (lights[currLight].diffuse.getBlue() * b_base * i) / (256 * 128);
					 }
				 }
				 else if (lights[currLight].attr & LIGHT_ATTR_SPOTLIGHT2)
				 {
					 dp = currPoly->tlist[0]->normal.dot(lights[currLight].dir);

					 if (dp > 0)
					 {
						 s = currPoly->tlist[0]->position - lights[currLight].pos;
						 s.w = 1;
						 dists = s.length();

						 float dps1 = s.dot(lights[currLight].dir) / dists;
						 if (dps1 > 0)
						 {
							 atten = (lights[currLight].kc + lights[currLight].kl * dists + lights[currLight].kq * dists * dists);
							 float dps1_exp = dps1;
							 for (int e_index = 1; e_index < (int)lights[currLight].pf; e_index++)
							 {
								 dps1_exp *= dps1;
							 }
							 i = 128 * dp * dps1_exp / (atten);

							 r_sum0 += (lights[currLight].diffuse.getRed() * r_base * i) / (256 * 128);
							 g_sum0 += (lights[currLight].diffuse.getGreen() * g_base * i) / (256 * 128);
							 b_sum0 += (lights[currLight].diffuse.getBlue() * b_base * i) / (256 * 128);
						 }
					 }

					 dp = currPoly->tlist[1]->normal.dot(lights[currLight].dir);

					 if (dp > 0)
					 {
						 s = currPoly->tlist[1]->position - lights[currLight].pos;
						 s.w = 1;
						 dists = s.length();

						 float dps1 = s.dot(lights[currLight].dir) / dists;
						 if (dps1 > 0)
						 {
							 atten = (lights[currLight].kc + lights[currLight].kl * dists + lights[currLight].kq * dists * dists);
							 float dps1_exp = dps1;
							 for (int e_index = 1; e_index < (int)lights[currLight].pf; e_index++)
							 {
								 dps1_exp *= dps1;
							 }
							 i = 128 * dp * dps1_exp / (atten);

							 r_sum1 += (lights[currLight].diffuse.getRed() * r_base * i) / (256 * 128);
							 g_sum1 += (lights[currLight].diffuse.getGreen() * g_base * i) / (256 * 128);
							 b_sum1 += (lights[currLight].diffuse.getBlue() * b_base * i) / (256 * 128);
						 }
					 }

					 dp = currPoly->tlist[2]->normal.dot(lights[currLight].dir);

					 if (dp > 0)
					 {
						 s = currPoly->tlist[2]->position - lights[currLight].pos;
						 s.w = 1;
						 dists = s.length();

						 float dps1 = s.dot(lights[currLight].dir) / dists;
						 if (dps1 > 0)
						 {
							 atten = (lights[currLight].kc + lights[currLight].kl * dists + lights[currLight].kq * dists * dists);
							 float dps1_exp = dps1;
							 for (int e_index = 1; e_index < (int)lights[currLight].pf; e_index++)
							 {
								 dps1_exp *= dps1;
							 }
							 i = 128 * dp * dps1_exp / (atten);

							 r_sum2 += (lights[currLight].diffuse.getRed() * r_base * i) / (256 * 128);
							 g_sum2 += (lights[currLight].diffuse.getGreen() * g_base * i) / (256 * 128);
							 b_sum2 += (lights[currLight].diffuse.getBlue() * b_base * i) / (256 * 128);
						 }
					 }
				 }
			 }
			 if (r_sum0> 255)r_sum0 = 255;
			 if (g_sum0 > 255)g_sum0 = 255;
			 if (b_sum0 > 255)b_sum0 = 255;

			 if (r_sum1 > 255)r_sum1 = 255;
			 if (g_sum1 > 255)g_sum1 = 255;
			 if (b_sum1 > 255)b_sum1 = 255;

			 if (r_sum2 > 255)r_sum2 = 255;
			 if (g_sum2 > 255)g_sum2 = 255;
			 if (b_sum2 > 255)b_sum2 = 255;

			 currPoly->lit_color[0]->setRGBA(r_sum0, g_sum0, b_sum0, 0);
			 currPoly->lit_color[1]->setRGBA(r_sum1, g_sum1, b_sum1, 0);
			 currPoly->lit_color[2]->setRGBA(r_sum2, g_sum2, b_sum2, 0);
		}
		else
		{
			currPoly->lit_color[0]->setRGBA((int)(currPoly->color->getRGBA() << 16 | currPoly->color->getRGBA()));
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

	z1 = (float)0.33333*(poly1->tlist[0]->position.z + poly1->tlist[1]->position.z + poly1->tlist[2]->position.z);
	z2 = (float)0.33333*(poly2->tlist[0]->position.z + poly2->tlist[1]->position.z + poly2->tlist[2]->position.z);
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

	z1 = MIN(poly1->tlist[0]->position.z, poly1->tlist[1]->position.z);
	z1 = MIN(z1, poly1->tlist[2]->position.z);

	z2 = MIN(poly2->tlist[0]->position.z, poly2->tlist[1]->position.z);
	z2 = MIN(z2, poly2->tlist[2]->position.z);

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

	z1 = MAX(poly1->tlist[0]->position.z, poly1->tlist[1]->position.z);
	z1 = MAX(z1, poly1->tlist[2]->position.z);

	z2 = MAX(poly2->tlist[0]->position.z, poly2->tlist[1]->position.z);
	z2 = MAX(z2, poly2->tlist[2]->position.z);

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