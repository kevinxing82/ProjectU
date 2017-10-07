#include "kxParser.h"
#include "kxDebug.h"

USING_KX;

kxParser::kxParser()
{
}

kxParser::~kxParser()
{
}

int kxParser::Reset()
{
	return 0;
}

int kxParser::Open(char * filename)
{
	return 0;
}

int  kxParser::Close()
{
	return 0;
}

char * kxParser::GetLine(int mode)
{
	return nullptr;
}

char * kxParser::GetLine_PLG(char * buffer, int maxLength, FILE * fp)
{
	int index = 0;
	int length = 0;
	while (1)
	{
		if (!fgets(buffer, maxLength, fp))
		{
			return (NULL);
		}

		for (length = strlen(buffer), index = 0; isspace(buffer[index]); index++);

		if (index >= length || buffer[index] == '#')
		{
			continue;
		}

		return (&buffer[index]);
	}
}

int kxParser::SetComment(char * string)
{
	return 0;
}

int kxParser::PatternMatch(char * string, char * pattern, ...)
{
	return 0;
}

int kxParser::Load_Object_PLG(kxRenderObject * obj, char * filename, kxVector4 * scale, kxVector4 * pos, kxVector4 * rot)
{
	FILE *fp;
	char buffer[256];

	char *token_string;

	RGB16Bit = RGB16Bit565;

	// Step 1: clear out the object and initialize it a bit
	memset(obj, 0, sizeof(kxRenderObject));

	obj->state = OBJECT4DV1_STATE_ACTIVE | OBJECT4DV1_STATE_VISIBLE;

	obj->world_pos.x = pos->x;
	obj->world_pos.y = pos->y;
	obj->world_pos.z = pos->z;

	// Step 2: open the file for reading
	errno_t err;
	if (err=fopen_s(&fp,filename, "r")!=0)
	{
		KX_ERROR("Couldn't open PLG file %s.", filename);
		return(0);
	}

	// Step 3: get the first token string which should be the object descriptor
	if (!(token_string = GetLine_PLG(buffer, 255, fp)))
	{
		KX_ERROR("PLG file error with file %s (object descriptor invalid)", filename);
		return(0);
	}

	KX_ERROR("Object Descriptor: %s", token_string);

	int e = sscanf_s(token_string, "%s %d %d", obj->name,sizeof(obj->name), &obj->num_vertices, &obj->num_polys);

	// Step 4: load the vertex list
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		if (!(token_string = GetLine_PLG(buffer, 255, fp)))
		{
			KX_ERROR("PLG file error with file %s(vertex list invalid).", filename);
			return (0);
		}

		sscanf_s(token_string, "%f %f %f",
			&obj->vlist_local[vertex].x,
			&obj->vlist_local[vertex].y,
			&obj->vlist_local[vertex].z);

		obj->vlist_local[vertex].w = 1;

		obj->vlist_local[vertex].x *= scale->x;
		obj->vlist_local[vertex].y *= scale->y;
		obj->vlist_local[vertex].z *= scale->z;

		KX_LOG("\nVertex %d = %f, %f, %f, %f",
			vertex,
			obj->vlist_local[vertex].x,
			obj->vlist_local[vertex].y,
			obj->vlist_local[vertex].z,
			obj->vlist_local[vertex].w);
	}
	
	ComputeRenderObjectRadius(obj);

	KX_LOG("\nObject average radius = %f,max radius = %f",
		obj->avg_radius,
		obj->max_radius);


	int polySurfaceDesc = 0;
	int polyNumVerts = 0;
	char tmpString[8];

	// Step 5: load the polygon list
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		if (!(token_string = GetLine_PLG(buffer, 255, fp)))
		{
			KX_LOG("PLG file error with file %s (polygon descriptor invalid).", filename);
			return (0);
		}
		KX_LOG("\nPolygon %d:", poly);

		sscanf_s(token_string, "%s %d %d %d %d", tmpString,sizeof(tmpString),
			&polyNumVerts,
			&obj->plist[poly].vert[0],
			&obj->plist[poly].vert[1],
			&obj->plist[poly].vert[2]);

		if (tmpString[0] == '0' && toupper(tmpString[1]) == 'X')
		{
			sscanf_s(tmpString, "%x", &polySurfaceDesc);
		}
		else
		{
			polySurfaceDesc = atoi(tmpString);
		}
		obj->plist[poly].vlist = obj->vlist_local;

		KX_LOG("\nSurface Desc = 0x%.4x, num_verts = %d, vert_indices [%d, %d, %d]",
			polySurfaceDesc,
			polyNumVerts,
			obj->plist[poly].vert[0],
			obj->plist[poly].vert[1],
			obj->plist[poly].vert[2]);

		if ((polySurfaceDesc& PLX_2SIDED_FLAG))
		{
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_2SIDED);
			KX_LOG("\n2 sided.");
		}
		else
		{
			KX_LOG("\n1 sided.");
		}

		// now let's set the color type and color
		if ((polySurfaceDesc & PLX_COLOR_MODE_RGB_FLAG))
		{
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_RGB16);
			int red = ((polySurfaceDesc & 0x0f00) >> 8);
			int green = ((polySurfaceDesc & 0x00f0) >> 4);
			int blue = (polySurfaceDesc & 0x000f);

			obj->plist[poly].color.setRGBA(red * 16, green * 16, blue * 16,1);
			KX_LOG("\nRGB color = [%d, %d, %d]", red, green, blue);
		}
		else
		{
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_8BITCOLOR);

			obj->plist[poly].color.setRGBA(polySurfaceDesc & 0x00ff);

			KX_LOG("\n8-bit color index = %d", obj->plist[poly].color);
		}

		int shadeMode = (polySurfaceDesc & PLX_SHADE_MODE_MASK);

		switch (shadeMode)
		{
		case PLX_SHADE_MODE_PURE_FLAG:
		{
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_PURE);
			KX_LOG("\nShaow mode = pure");
			break;
		}
		case PLX_SHADE_MODE_FLAT_FLAG:
		{
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_FLAT);
			KX_LOG("\nShadow mode = flat");
			break;
		}
		case PLX_SHADE_MODE_GOURAUD_FLAG:
		{
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_GOURAUD);
			KX_LOG("\nShadow mode = gouraud");
			break;
		}
		case PLX_SHADE_MODE_PHONG_FLAG:
		{
			SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_PHONG);
			KX_LOG("\nShadow mode = phong");
			break;
		}
		default:break;
		}
		obj->plist[poly].state = POLY4DV1_STATE_ACTIVE;
	}
	fclose(fp);
	return (1);
}

float kxParser::ComputeRenderObjectRadius(kxRenderObject * obj)
{
	obj->avg_radius = 0;
	obj->max_radius = 0;

	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		float distToVertex = sqrt(obj->vlist_local[vertex].x*obj->vlist_local[vertex].x +
			obj->vlist_local[vertex].y*obj->vlist_local[vertex].y +
			obj->vlist_local[vertex].z*obj->vlist_local[vertex].z);

		obj->avg_radius += distToVertex;
		if (distToVertex > obj->max_radius)
		{
			obj->max_radius = distToVertex;
		}
	}

	obj->avg_radius /= obj->num_vertices;
	return (obj->max_radius);
}
