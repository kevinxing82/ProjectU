#include "kxParser.h"
#include "kxDebug.h"
#include "kxUtils.h"

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

kxRenderObject* kxParser::Load_Object_PLG(char * filename, kxVector4 * scale, kxVector4 * pos, kxVector4 * rot, int vertex_flag)
{
	FILE *fp;
	char buffer[256];

	char *token_string;

	RGB16Bit = RGB16Bit565;

	// Step 1: clear out the object and initialize it a bit
	kxRenderObject* obj = new kxRenderObject();

	obj->state = OBJECT4D_STATE_ACTIVE | OBJECT4D_STATE_VISIBLE;

	obj->world_pos.x = pos->x;
	obj->world_pos.y = pos->y;
	obj->world_pos.z = pos->z;
	obj->world_pos.w = pos->w;

	obj->num_frames = 1;
	obj->curr_frame = 0;
	obj->attr = OBJECT4D_ATTR_SINGLE_FRAME;

	// Step 2: open the file for reading
	errno_t err;
	if (err = fopen_s(&fp, filename, "r") != 0)
	{
		KX_ERROR("Couldn't open PLG file %s.", filename);
		return nullptr;
	}

	// Step 3: get the first token string which should be the object descriptor
	if (!(token_string = GetLine_PLG(buffer, 255, fp)))
	{
		KX_ERROR("PLG file error with file %s (object descriptor invalid)", filename);
		return nullptr;
	}

	KX_ERROR("Object Descriptor: %s", token_string);

	int e = sscanf_s(token_string, "%s %d %d", obj->name, sizeof(obj->name), &obj->num_vertices, &obj->num_polys);

	obj->Init();

	// Step 4: load the vertex list
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		if (!(token_string = GetLine_PLG(buffer, 255, fp)))
		{
			KX_ERROR("PLG file error with file %s(vertex list invalid).", filename);
			return nullptr;
		}

		sscanf_s(token_string, "%f %f %f",
			&obj->vlist_local[vertex].position.x,
			&obj->vlist_local[vertex].position.y,
			&obj->vlist_local[vertex].position.z);

		obj->vlist_local[vertex].position.w = 1;

		obj->vlist_local[vertex].position.x *= scale->x;
		obj->vlist_local[vertex].position.y *= scale->y;
		obj->vlist_local[vertex].position.z *= scale->z;

		KX_LOG("\nVertex %d = %f, %f, %f, %f",
			vertex,
			obj->vlist_local[vertex].position.x,
			obj->vlist_local[vertex].position.y,
			obj->vlist_local[vertex].position.z,
			obj->vlist_local[vertex].position.w);
		SET_BIT(obj->vlist_local[vertex].attr, VERTEX4D_ATTR_POINT);
	}

	obj->ComputeRadius();

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
			return nullptr;
		}
		KX_LOG("\nPolygon %d:", poly);

		sscanf_s(token_string, "%s %d %d %d %d", tmpString, sizeof(tmpString),
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
			SET_BIT(obj->plist[poly].attr, POLY4D_ATTR_2SIDED);
			KX_LOG("\n2 sided.");
		}
		else
		{
			KX_LOG("\n1 sided.");
		}

		// now let's set the color type and color
		if ((polySurfaceDesc & PLX_COLOR_MODE_RGB_FLAG))
		{
			SET_BIT(obj->plist[poly].attr, POLY4D_ATTR_RGB16);
			int red = ((polySurfaceDesc & 0x0f00) >> 8);
			int green = ((polySurfaceDesc & 0x00f0) >> 4);
			int blue = (polySurfaceDesc & 0x000f);

			obj->plist[poly].color.setRGBA(red * 16, green * 16, blue * 16, 1);
			KX_LOG("\nRGB color = [%d, %d, %d]", red, green, blue);
		}
		else
		{
			SET_BIT(obj->plist[poly].attr, POLY4D_ATTR_8BITCOLOR);

			obj->plist[poly].color.setRGBA(polySurfaceDesc & 0x00ff);

			KX_LOG("\n8-bit color index = %d", obj->plist[poly].color);
		}

		int shadeMode = (polySurfaceDesc & PLX_SHADE_MODE_MASK);

		switch (shadeMode)
		{
		case PLX_SHADE_MODE_PURE_FLAG:
		{
			SET_BIT(obj->plist[poly].attr, POLY4D_ATTR_SHADE_MODE_PURE);
			KX_LOG("\nShaow mode = pure");
			break;
		}
		case PLX_SHADE_MODE_FLAT_FLAG:
		{
			SET_BIT(obj->plist[poly].attr, POLY4D_ATTR_SHADE_MODE_FLAT);
			KX_LOG("\nShadow mode = flat");
			break;
		}
		case PLX_SHADE_MODE_GOURAUD_FLAG:
		{
			SET_BIT(obj->plist[poly].attr, POLY4D_ATTR_SHADE_MODE_GOURAUD);
			KX_LOG("\nShadow mode = gouraud");
			//设置顶点属性 来计算法线
			SET_BIT(obj->vlist_local[obj->plist[poly].vert[0]].attr, VERTEX4D_ATTR_NORMAL);
			SET_BIT(obj->vlist_local[obj->plist[poly].vert[1]].attr, VERTEX4D_ATTR_NORMAL);
			SET_BIT(obj->vlist_local[obj->plist[poly].vert[2]].attr, VERTEX4D_ATTR_NORMAL);
			break;
		}
		case PLX_SHADE_MODE_PHONG_FLAG:
		{
			SET_BIT(obj->plist[poly].attr, POLY4D_ATTR_SHADE_MODE_PHONG);
			KX_LOG("\nShadow mode = phong");
			SET_BIT(obj->vlist_local[obj->plist[poly].vert[0]].attr, VERTEX4D_ATTR_NORMAL);
			SET_BIT(obj->vlist_local[obj->plist[poly].vert[1]].attr, VERTEX4D_ATTR_NORMAL);
			SET_BIT(obj->vlist_local[obj->plist[poly].vert[2]].attr, VERTEX4D_ATTR_NORMAL);
			break;
		}
		default:break;
		}
		SET_BIT(obj->plist[poly].attr, POLY4D_ATTR_DISABLE_MATERIAL);
		obj->plist[poly].state = POLY4D_STATE_ACTIVE;
		//将多边形的顶点列表指针指向物体的顶点列表
		obj->plist[poly].vlist = obj->vlist_local;
		//	设置纹理坐标列表
		obj->plist[poly].tlist = obj->tlist;
	}
	obj->ComputePolyNormals();
	obj->ComputeVertexNormasl();
	fclose(fp);
	return obj;
}

kxRenderObject*  kxParser::Load_Object_3DSASC(char * filename, kxVector4 * scale, kxVector4 * pos, kxVector4 * rot, int vertex_flags)
{
	char seps[16];
	char token_buffer[256];
	char *token;

	kxColor* color =new kxColor();

	kxRenderObject* obj = new kxRenderObject();
	obj->state = OBJECT4D_STATE_ACTIVE | OBJECT4D_STATE_VISIBLE;

	obj->num_frames = 1;
	obj->curr_frame = 0;
	obj->attr = OBJECT4D_ATTR_SINGLE_FRAME;

	if (pos)
	{
		obj->world_pos.x = pos->x;
		obj->world_pos.y = pos->y;
		obj->world_pos.z = pos->z;
		obj->world_pos.w = pos->w;
	}
	else
	{
		obj->world_pos.x = 0;
		obj->world_pos.y = 0;
		obj->world_pos.z = 0;
		obj->world_pos.w = 1;
	}

	if (!this->Open(filename))
	{
		KX_ERROR("Couldn't open .ASC file %s.", filename);
		return nullptr;
	}

	//Step 3:
	while (true)
	{
		if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			KX_ERROR("Image 'name' not found in .ASC file %s.", filename);
			return nullptr;
		}

		if (this->PatternMatch(this->buffer, "['Named'] ['object:']"))
		{
			strcpy_s(token_buffer, sizeof(token_buffer), this->buffer);
			strcpy_s(seps, sizeof(seps), "\"");

			char *pTmp = NULL;
			strtok_s(token_buffer, seps, &pTmp);
			token = strtok_s(NULL, seps, &pTmp);

			strcpy_s(obj->name, sizeof(obj->name), token);
			KX_ERROR("\nASC Reader Object Name: %s", obj->name);
			break;
		}
	}

	//Step 4:
	while (true)
	{
		if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			KX_ERROR("'Tri-mesh', line not found in .ASC file", filename);
			return nullptr;
		}

		if (this->PatternMatch(this->buffer, "['Tri-mesh,'] ['Vertices:'] [i] ['Faces:'] [i]"))
		{
			obj->num_vertices = this->pints[0];
			obj->num_polys = this->pints[1];
			KX_ERROR("\nASC Reader Num Vertices : %d, Num Polys : %d", obj->num_vertices, obj->num_polys);
			break;
		}
	}

	obj->Init();

	while (true)
	{
		if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			KX_ERROR("\n'Vertex list: ' line not found in .ASC file s%.", filename);
			return nullptr;
		}

		if (this->PatternMatch(this->buffer, "['Vertex']['list:']"))
		{
			KX_ERROR("\nASC Reader found vertex list in .ASC file %s", filename);
			return nullptr;
		}
	}

	// Read in vertex list
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		while (true)
		{
			if (!this->GetLine(PARSER_LEAVE_EMPTY_LINES | PARSER_LEAVE_WS_ENDS))
			{
				KX_ERROR("\nVertex list ended abrupty! in .ASC file %s.", filename);
				return nullptr;
			}
			kxUtils::StripChars(this->buffer, this->buffer, ":XYZ");

			if (this->PatternMatch(this->buffer, "['Vertex'] [i] [f] [f] [f]"))
			{
				obj->vlist_local[vertex].position.x = this->pfloats[0];
				obj->vlist_local[vertex].position.y = this->pfloats[1];
				obj->vlist_local[vertex].position.z = this->pfloats[2];
				obj->vlist_local[vertex].position.w = 1;

				float temp_f;

				if (vertex_flags&VERTEX_FLAGS_INVERT_X)
				{
					obj->vlist_local[vertex].position.x = -obj->vlist_local[vertex].position.x;
				}
				if (vertex_flags&VERTEX_FLAGS_INVERT_Y)
				{
					obj->vlist_local[vertex].position.y = -obj->vlist_local[vertex].position.y;
				}
				if (vertex_flags&VERTEX_FLAGS_INVERT_Z)
				{
					obj->vlist_local[vertex].position.z = -obj->vlist_local[vertex].position.z;
				}

				if (vertex_flags&VERTEX_FLAGS_SWAP_YZ)
				{
					SWAP(obj->vlist_local[vertex].position.y, obj->vlist_local[vertex].position.z, temp_f);
				}
				if (vertex_flags&VERTEX_FLAGS_SWAP_XZ)
				{
					SWAP(obj->vlist_local[vertex].position.x, obj->vlist_local[vertex].position.z, temp_f);
				}
				if (vertex_flags&VERTEX_FLAGS_SWAP_XY)
				{
					SWAP(obj->vlist_local[vertex].position.x, obj->vlist_local[vertex].position.y, temp_f);
				}

				KX_ERROR("\nVertex %d = %f, %f, %f, %f", vertex,
					obj->vlist_local[vertex].position.x,
					obj->vlist_local[vertex].position.y,
					obj->vlist_local[vertex].position.z,
					obj->vlist_local[vertex].position.w);

				if (scale)
				{
					obj->vlist_local[vertex].position.x *= scale->x;
					obj->vlist_local[vertex].position.y *= scale->y;
					obj->vlist_local[vertex].position.z *= scale->z;
				}
				break;
			}//end if
		}//end while
	}//end  for vertex

	obj->ComputeRadius();

	KX_ERROR("\nObject average radius = %f, max radius = %f", 
		obj->avg_radius[obj->curr_frame], 
		obj->max_radius[obj->curr_frame]);

	while (true)
	{
		if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			KX_ERROR("\n'Face list: ' line not found in .ASC file %s.", filename);
			return nullptr;
		}

		if (this->PatternMatch(this->buffer, "['Face'] ['list:']"))
		{
			KX_ERROR("\nASC Reader found face list in .ASC file %s", filename);
			return nullptr;
		}
	}

	int poly_surface_desc = 0;
	int poly_num_verts = 0;
	char tmp_string[8];

	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		while (true)
		{
			KX_ERROR("\nFace list ended abrupty! in .ASC file %s. ", filename);
			return nullptr;

			kxUtils::StripChars(this->buffer, this->buffer, ":ABC");

			if (this->PatternMatch(this->buffer, "['Face'] [i] [i] [i] [i]"))
			{
				if (vertex_flags&VERTEX_FLAGS_INVERT_WINDING_ORDER)
				{
					poly_num_verts = 3;
					obj->plist[poly].vert[0] = this->pints[3];
					obj->plist[poly].vert[1] = this->pints[2];
					obj->plist[poly].vert[2] = this->pints[1];
				}
				else
				{
					poly_num_verts = 3;
					obj->plist[poly].vert[0] = this->pints[1];
					obj->plist[poly].vert[1] = this->pints[2];
					obj->plist[poly].vert[2] = this->pints[3];
				}
				obj->plist[poly].vlist = obj->vlist_local;
				break;
			}	
		}

		while (true)
		{
			if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				KX_ERROR("\nMaterial list ended abruptly! in .ASC file %s.",filename);
				return nullptr;
			}

			kxUtils::StripChars(this->buffer, this->buffer, ":\"rgba", ' ');

			if (this->PatternMatch(this->buffer, "[i] [i] [i]"))
			{
				color->setRed(this->pints[0]);
				color->setGreen(this->pints[1]);
				color->setBlue(this->pints[2]);

				obj->plist[poly].color = *color;

				SET_BIT(obj->plist[poly].attr, POLY4D_ATTR_SHADE_MODE_FLAT);

				obj->plist[poly].state = POLY4D_STATE_ACTIVE;

				break;
			}
		}
		KX_ERROR("\nPolygon %d: ", poly);
		KX_ERROR("\nSurface Desc = [RGB]=[%d, %d, %d]",
			color->getRed(), color->getGreen(), color->getBlue(),
			obj->plist[poly].vert[0],
			obj->plist[poly].vert[1],
			obj->plist[poly].vert[2]);
	}
	return obj;
}

kxRenderObject*  kxParser::Load_Object_COD(char * filename, kxVector4 * scale, kxVector4 * pos, kxVector4 * rot, int vertex_flag)
{
	return 0;
}
