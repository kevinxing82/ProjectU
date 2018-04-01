#include "kxParser.h"
#include "kxDebug.h"
#include "kxUtils.h"
#include "kxMatrix.h"

USING_KX;

kxParser::kxParser()
{
	texture_path = "./";
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
	if (strlen(string) < PARSER_MAX_COMMENT)
	{
		strcpy_s(comment, sizeof(comment), string);
		return 1;
	}
	return 0;
}

int kxParser::PatternMatch(char * string, char * pattern, ...)
{
	char token_type[PATTERN_MAX_ARGS];
	char token_string[PATTERN_MAX_ARGS][PATTERN_BUFFER_SIZE];
	char token_operator[PATTERN_MAX_ARGS];
	int token_numeric[PATTERN_MAX_ARGS];

	char buffer[PARSER_BUFFER_SIZE];

	if ((!string || strlen(string) == 0) || (!pattern || strlen(pattern) == 0))
	{
		return 0;
	}

	strcpy_s(buffer, string);

	int tok_start = 0,
		tok_end = 0,
		tok_restart = 0,
		tok_first_pass = 0,
		num_tokens = 0;
	// step 1: extract token list
	while (true)
	{
		while (isspace(pattern[tok_start]))
		{
			tok_start++;
		}

		if (tok_start >= strlen(pattern))
		{
			break;
		}

		if (pattern[tok_start] == '[')
		{
			switch (pattern[tok_start + 1])
			{
			case PATTERN_TOKEN_FLOAT:
			{
				if (pattern[tok_start + 2] != ']')
				{
					return 0;
				}

				tok_start += 3;

				token_type[num_tokens] = PATTERN_TOKEN_FLOAT;
				strcpy_s(token_string[num_tokens], "");
				token_operator[num_tokens] = 0;
				token_numeric[num_tokens] = 0;

				num_tokens++;
			}
			break;

			case PATTERN_TOKEN_INT:
			{
				if (pattern[tok_start + 2] != ']')
				{
					return 0;
				}
				tok_start += 3;

				token_type[num_tokens] = PATTERN_TOKEN_INT;
				strcpy_s(token_string[num_tokens], "");
				token_operator[num_tokens] = 0;
				token_numeric[num_tokens] = 0;

				num_tokens++;
			}
			break;

			case PATTERN_TOKEN_STRING:
			{
				if (pattern[tok_start + 2] == '=' ||
					pattern[tok_start + 2] == '>' ||
					pattern[tok_start + 2] == '<')
				{
					tok_end = tok_start + 3;
					while (isdigit(pattern[tok_end]))
					{
						tok_end++;
					}
					if (pattern[tok_end] != ']')
					{
						return 0;
					}

					memcpy_s(buffer, sizeof(buffer),&pattern[tok_start + 3], (tok_end - tok_start));
					buffer[tok_end - tok_start] = 0;

					token_type[num_tokens] = PATTERN_TOKEN_STRING;
					strcpy_s(token_string[num_tokens], "");
					token_operator[num_tokens] = pattern[tok_start + 2];
					token_numeric[num_tokens] = atoi(buffer);
				}
				else
				{
					return 0;
				}

				tok_start = tok_end + 1;
				num_tokens++;
			}
			break;

			case PATTERN_TOKEN_LITERAL:
			{
				tok_start += 2;
				tok_end = tok_start;

				while (pattern[tok_end] != PATTERN_TOKEN_LITERAL)
				{
					tok_end++;
				}

				if (pattern[tok_end + 1] != ']')
				{
					return 0;
				}

				memcpy_s(token_string[num_tokens],sizeof(token_string[num_tokens]), &pattern[tok_start], (tok_end - tok_start));
				token_string[num_tokens][(tok_end - tok_start)] = 0;

				token_type[num_tokens] = PATTERN_TOKEN_LITERAL;
				token_operator[num_tokens] = 0;
				token_numeric[num_tokens] = 0;

				tok_start = tok_end + 2;
				num_tokens++;
			}
			break;
			default:
				break;
			}
		}
		if (tok_start >= strlen(pattern))
		{
			break;
		}
	}

	int pattern_state = PATTERN_STATE_INIT;
	int curr_tok = 0;
	char token[PATTERN_BUFFER_SIZE];

	while (true)
	{
		switch (pattern_state)
		{
		case PATTERN_STATE_INIT:
		{
			strcpy_s(buffer, string);

			tok_start = 0;
			tok_end = 0;
			tok_restart = 0;
			tok_first_pass = 0;
			curr_tok = 0;

			num_pints = num_pfloats = num_pstrings = 0;
			pattern_state = PATTERN_STATE_RESTART;
		}
		break;

		case PATTERN_STATE_RESTART:
		{
			curr_tok = 0;
			tok_first_pass = 1;

			if (tok_end >= strlen(buffer))
			{
				return 0;
			}

			tok_start = tok_end = tok_restart;
			
			pattern_state = PATTERN_STATE_NEXT;
		}
		break;

		case PATTERN_STATE_NEXT:
		{
			if (curr_tok >= num_tokens)
			{
				pattern_state = PATTERN_STATE_MATCH;
			}
			else
			{
				if (tok_end >= strlen(buffer))
				{
					return 0;
				}

				tok_start = tok_end;
				while (isspace(buffer[tok_start]))
				{
					tok_start++;
				}
				tok_end = tok_start;

				while (!isspace(buffer[tok_end]) && tok_end < strlen(buffer))
				{
					tok_end++;
				}

				memcpy_s(token, sizeof(token),&buffer[tok_start], tok_end - tok_start);
				token[tok_end - tok_start] = 0;

				if (strlen(token) == 0)
				{
					return 0;
				}

				if (tok_first_pass)
				{
					tok_first_pass = 0;
					tok_restart = tok_end;
				}
				switch (token_type[curr_tok])
				{
				case PATTERN_TOKEN_FLOAT:
				{
					pattern_state = PATTERN_STATE_FLOAT;
				}
				break;
				case PATTERN_TOKEN_INT:
				{
					pattern_state = PATTERN_TOKEN_INT;
				}
				break;
				case PATTERN_TOKEN_STRING:
				{
					pattern_state = PATTERN_STATE_STRING;
				}
				break;
				case PATTERN_TOKEN_LITERAL:
				{
					pattern_state = PATTERN_TOKEN_LITERAL;
				}
				break;
				default:break;
				}
			}
		}
		break;

		case PATTERN_STATE_FLOAT:
		{
			float f = IsFloat(token);

			if (f != FLT_MIN)
			{
				pfloats[num_pfloats++] = f;

				curr_tok++;
				pattern_state = PATTERN_STATE_NEXT;
			}
			else
			{
				pattern_state = PATTERN_STATE_RESTART;
			}
		}
		break;

		case PATTERN_STATE_INT:
		{
			int i = IsInt(token);
			if (i != INT_MIN)
			{
				pints[num_pints++] = i;

				curr_tok++;
				pattern_state = PATTERN_STATE_NEXT;
			}
			else
			{
				pattern_state = PATTERN_STATE_RESTART;
			}
		}
		break;

		case PATTERN_STATE_LITERAL:
		{
			if (strcmp(token, token_string[curr_tok]) == 0)
			{
				strcpy_s(pstring[num_pstrings++], token);

				curr_tok++;
				pattern_state - PATTERN_STATE_NEXT;
			}
			else
			{
				pattern_state = PATTERN_STATE_RESTART;
			}
		}
		break;

		case PATTERN_STATE_STRING:
		{
			switch (token_operator[curr_tok])
			{
			case '=':
			{
				if (strlen(token) == token_numeric[curr_tok])
				{
					strcpy_s(pstring[num_pstrings++], token);
					curr_tok++;
					pattern_state = PATTERN_STATE_NEXT;
				}
				else
				{
					pattern_state = PATTERN_STATE_RESTART;
				}
			}
			break;
			case '>':
			{
				if (strlen(token) > token_numeric[curr_tok])
				{
					strcpy_s(pstring[num_pstrings++], token);
					curr_tok++;
					pattern_state = PATTERN_STATE_NEXT;
				}
				else
				{
					pattern_state = PATTERN_STATE_RESTART;
				}
			}
			break;
			case '<':
			{
				if (strlen(token) < token_numeric[curr_tok])
				{
					strcpy_s(pstring[num_pstrings++], token);
					curr_tok++;
					pattern_state = PATTERN_STATE_NEXT;
				}
				else
				{
					pattern_state = PATTERN_STATE_RESTART;
				}
			}
			break;
			default:break;
			}
		}
		break;

		case PATTERN_STATE_MATCH:
		{
			return 1;
		}
		break;

		case PATTERN_STATE_END:{}
		break;
		default:break;
		}
	}
}

char * kxParser::ExtractFilenameFromPath(char * filepath, char * filename)
{
	if (!filepath || strlen(filepath) == 0)
	{
		return nullptr;
	}

	int index_end = strlen(filepath) - 1;

	while ((filepath[index_end] != '\\') &&
		(filepath[index_end] != '/') &&
		(filepath[index_end] > 0))
	{
		index_end--;
	}
	memcpy_s(filename, sizeof(filename), &filepath[index_end + 1], strlen(filepath) - index_end);
	return filename;
}

float kxParser::IsFloat(char * fstring)
{
	char *string = fstring;

	while (isspace(*string))
	{
		string++;
	}

	if (*string == '+' || *string == '-')
	{
		string++;
	}

	while (isdigit(*string))
	{
		string++;
	}

	if (*string == '.')
	{
		string++;
		while (isdigit(*string))
		{
			string++;
		}
	}

	if (*string == 'e' || *string == 'E' || *string == 'd' || *string == 'D')
	{
		string++;

		if (*string == '+' || *string == '-')
		{
			string++;
		}

		while (isdigit(*string))
		{
			string++;
		}
	}

	if (strlen(fstring) == (int)(string - fstring))
	{
		return (atof(fstring));
	}
	else
	{
		return (FLT_MIN);
	}
}

int kxParser::IsInt(char * istring)
{
	char *string = istring;
	
	while (isspace(*string))
	{
		string++;
	}

	if (*string == '+' || *string == '-')
	{
		string++;
	}
	
	while (isdigit(*string))
	{
		string++;
	}

	if (strlen(istring) == (int)(string - istring))
	{
		return atoi(istring);
	}
	else
	{
		return (INT_MIN);
	}
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

			obj->plist[poly].color->setRGBA(red * 16, green * 16, blue * 16, 1);
			KX_LOG("\nRGB color = [%d, %d, %d]", red, green, blue);
		}
		else
		{
			SET_BIT(obj->plist[poly].attr, POLY4D_ATTR_8BITCOLOR);

			obj->plist[poly].color->setRGBA(polySurfaceDesc & 0x00ff);

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

	kxColor* color = new kxColor();

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
				KX_ERROR("\nMaterial list ended abruptly! in .ASC file %s.", filename);
				return nullptr;
			}

			kxUtils::StripChars(this->buffer, this->buffer, ":\"rgba", ' ');

			if (this->PatternMatch(this->buffer, "[i] [i] [i]"))
			{
				color->setRed(this->pints[0]);
				color->setGreen(this->pints[1]);
				color->setBlue(this->pints[2]);

				obj->plist[poly].color = color;

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

kxRenderObject*  kxParser::Load_Object_COB(char * filename, kxVector4 * scale, kxVector4 * pos, kxVector4 * rot, int vertex_flag)
{
	char seps[16];
	char token_buffer[256];
	char *token;
	kxColor* color;

	kxVector2* texture_vertices[OBJECT4D_MAX_VERTICES];
	int num_texture_vertices = 0;

	// Step 1: 初始化

	kxMatrix44* mat_local = new kxMatrix44();
	kxMatrix44*	mat_world = new kxMatrix44();

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
	// Step 2: 打开文件
	if (!this->Open(filename))
	{
		KX_ERROR("Couldn't open .COB file %s.", filename);
		return nullptr;
	}

	// Step 3: 
	// 先找到物体的名字
	while (true)
	{
		if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			KX_ERROR("Image 'name' not found in .COB file %s.", filename);
			return nullptr;
		}

		if (this->PatternMatch(this->buffer, "['Name'] [s>0]"))
		{
			strcpy_s(obj->name, sizeof(obj->name), this->pstring[1]);
			KX_ERROR("\nCOB Reader Object Name: %s", obj->name);
			break;
		}
	}

	// step 4: 解析和存储局部和世界坐标
	// center 0 0 0
	// x axis 1 0 0
	// y axis 0 1 0
	// z axis 0 0 1
	while (true)
	{
		if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			KX_ERROR("Center not found in .COB file %s", filename);
			return nullptr;
		}

		if (this->PatternMatch(this->buffer, "['center'] [f] [f] [f]"))
		{
			mat_local[3][0] = -this->pfloats[0];
			mat_local[3][1] = -this->pfloats[1];
			mat_local[3][2] = -this->pfloats[2];

			this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS);
			this->PatternMatch(this->buffer, "['x'] ['axis'] [f] [f] [f]");

			mat_local[0][0] = -this->pfloats[0];
			mat_local[1][0] = -this->pfloats[1];
			mat_local[2][0] = -this->pfloats[2];

			this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS);
			this->PatternMatch(this->buffer, "['y'] ['axis'] [f] [f] [f]");

			mat_local[0][1] = -this->pfloats[0];
			mat_local[1][1] = -this->pfloats[1];
			mat_local[2][1] = -this->pfloats[2];

			this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS);
			this->PatternMatch(this->buffer, "['z'] ['axis'] [f] [f] [f]");

			mat_local[0][2] = -this->pfloats[0];
			mat_local[1][2] = -this->pfloats[1];
			mat_local[2][2] = -this->pfloats[2];

			break;
		}
	}

	// now "Transform"
	while (true)
	{
		if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			KX_ERROR("Transform not found in .COB file %s.", filename);
			return nullptr;
		}

		if (this->PatternMatch(this->buffer, "['Transform']"))
		{
			this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS);
			this->PatternMatch(this->buffer, "[f] [f] [f]");

			mat_world[0][0] = this->pfloats[0];
			mat_world[1][0] = this->pfloats[1];
			mat_world[2][0] = this->pfloats[2];

			this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS);
			this->PatternMatch(this->buffer, "[f] [f] [f]");

			mat_world[0][1] = this->pfloats[0];
			mat_world[1][1] = this->pfloats[1];
			mat_world[2][1] = this->pfloats[2];

			this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS);
			this->PatternMatch(this->buffer, "[f] [f] [f]");

			mat_world[0][2] = this->pfloats[0];
			mat_world[1][2] = this->pfloats[1];
			mat_world[2][2] = this->pfloats[2];

			break;
		}
	}

	// step 6: 解析多边形和顶点数
	while (true)
	{
		if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			KX_ERROR("'World Vertices' line not found in .COB file %s.", filename);
			return nullptr;
		}

		if (this->PatternMatch(this->buffer, "['World'] ['Vertices'] [i]"))
		{
			obj->num_vertices = this->pints[0];
			KX_ERROR("\nCOB Reader Num Vertices : %d", obj->num_vertices);
			break;
		}
	}

	obj->Init();

	// Step 7:解析顶点列表
	// "d.d d.d d.d"
	for (int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		while (true)
		{
			if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				KX_ERROR("\nVertex list ended abruptly in .COB file %s.", filename);
				return nullptr;
			}

			if (this->PatternMatch(this->buffer, "[f] [f] [f]"))
			{
				obj->vlist_local[vertex].position.x = this->pfloats[0];
				obj->vlist_local[vertex].position.y = this->pfloats[1];
				obj->vlist_local[vertex].position.z = this->pfloats[2];
				obj->vlist_local[vertex].position.w = 1;

				kxVector4* temp_vector;

				if (vertex_flag&VERTEX_FLAGS_TRANSFORM_LOCAL)
				{
					obj->vlist_local[vertex].position = (*mat_local)*obj->vlist_local[vertex].position;
				}

				if (vertex_flag&VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD)
				{
					obj->vlist_local[vertex].position = (*mat_world)*obj->vlist_local[vertex].position;
				}

				float temp_f;

				if (vertex_flag&VERTEX_FLAGS_INVERT_X)
				{
					obj->vlist_local[vertex].position.x = -obj->vlist_local[vertex].position.x;
				}
				if (vertex_flag&VERTEX_FLAGS_INVERT_Y)
				{
					obj->vlist_local[vertex].position.y = -obj->vlist_local[vertex].position.y;
				}
				if (vertex_flag&VERTEX_FLAGS_INVERT_Z)
				{
					obj->vlist_local[vertex].position.z = -obj->vlist_local[vertex].position.z;
				}

				if (vertex_flag&VERTEX_FLAGS_SWAP_YZ)
				{
					SWAP(obj->vlist_local[vertex].position.y, obj->vlist_local[vertex].position.z, temp_f);
				}
				if (vertex_flag&VERTEX_FLAGS_SWAP_XZ)
				{
					SWAP(obj->vlist_local[vertex].position.x, obj->vlist_local[vertex].position.z, temp_f);
				}
				if (vertex_flag&VERTEX_FLAGS_SWAP_XY)
				{
					SWAP(obj->vlist_local[vertex].position.x, obj->vlist_local[vertex].position.y, temp_f);
				}

				if (scale)
				{
					obj->vlist_local[vertex].position.x *= scale->x;
					obj->vlist_local[vertex].position.y *= scale->y;
					obj->vlist_local[vertex].position.z *= scale->z;
				}

				KX_ERROR("\nVertex %d = %f, %f, %f, %f", vertex,
					obj->vlist_local[vertex].position.x,
					obj->vlist_local[vertex].position.y,
					obj->vlist_local[vertex].position.z,
					obj->vlist_local[vertex].position.w);

				SET_BIT(obj->vlist_local[vertex].attr, VERTEX4D_ATTR_POINT);
				break;
			}
		}
	}

	obj->ComputeRadius();

	KX_ERROR("\nObject average radius = %f,max radius = %f",
		obj->avg_radius, obj->max_radius);

	// step 8: 解析贴图顶点
	while (true)
	{
		if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			KX_ERROR("'Texture Vertices' line not found in .COB file %s.", filename);
			return nullptr;
		}

		if (this->PatternMatch(this->buffer, "['Texture'] ['Vertices'] [i]"))
		{
			num_texture_vertices = this->pints[0];
			KX_ERROR("\nCOB Reader Texture Vertices : %d", num_texture_vertices);
			break;
		}
	}

	// Step 9: 读取贴图顶点数据
	// "d.d d.d"
	for (int tvertex = 0; tvertex < num_texture_vertices; tvertex++)
	{
		while (true)
		{
			if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				KX_ERROR("\nTexture Vertex list ended abrubtly! in .COB file %s.", filename);
				return nullptr;
			}

			if (this->PatternMatch(this->buffer, "[f] [f]"))
			{
				obj->tlist[tvertex].x = this->pfloats[0];
				obj->tlist[tvertex].y = this->pfloats[1];

				KX_ERROR("\nTexture Vertex %d: U=%f, V=%f", tvertex,
					obj->tlist[tvertex].x,
					obj->tlist[tvertex].y);
				break;
			}
		}
	}

	int poly_material[OBJECT4D_MAX_POLYS];
	int *material_index_referenced = new int[MAX_MATERIALS];

	// step 10: 解析多边形
	// "Faces ddd:"

	while (true)
	{
		if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			KX_ERROR("\n'Faces' line not found in .COB files %s.", filename);
			return nullptr;
		}
		if (this->PatternMatch(this->buffer, "['Face'] [i]"))
		{
			KX_ERROR("\nCOB Reader found face list in .COB file %s.", filename);
			obj->num_polys = this->pints[0];
			break;
		}
	}

	int poly_surface_desc = 0;
	int poly_num_verts = 0;
	int num_material_object = 0;

	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		KX_ERROR("\nPolygon %d:", poly);
		while (true)
		{
			if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				KX_ERROR("\nface list ended abruptly! in .COB file %s.", filename);
				return nullptr;
			}

			if (this->PatternMatch(this->buffer, "['Face'] ['verts'] [i] ['flags'] [i] ['mat'] [i]"))
			{
				poly_material[poly] = this->pints[2];
				if (material_index_referenced[poly_material[poly]] == 0)
				{
					material_index_referenced[poly_material[poly]] = 1;
					num_material_object++;
				}

				if (this->pints[0] != 3)
				{
					KX_ERROR("\nface not a triangle! in .COB file %s.", filename);
					return nullptr;
				}

				if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
				{
					KX_ERROR("\nface list ended abruptly! in .COB file %s,", filename);
					return nullptr;
				}

				kxUtils::ReplaceChars(this->buffer, this->buffer, ",<>", ' ');
				this->PatternMatch(this->buffer, "[i] [i] [i] [i] [i] [i]");

				if (vertex_flag&VERTEX_FLAGS_INVERT_WINDING_ORDER)
				{
					poly_num_verts = 3;
					obj->plist[poly].vert[0] = this->pints[4];
					obj->plist[poly].vert[1] = this->pints[2];
					obj->plist[poly].vert[2] = this->pints[0];

					obj->plist[poly].text[0] = this->pints[5];
					obj->plist[poly].text[1] = this->pints[3];
					obj->plist[poly].text[2] = this->pints[1];

					KX_ERROR("\nAssigning texture vertex index %d [%f, %f] to mesh vertex %d",
						this->pints[5],
						obj->tlist[this->pints[5]].x,
						obj->tlist[this->pints[5]].y,
						obj->plist[poly].vert[0]);

					KX_ERROR("\nAssigning texture vertex index %d [%f, %f] to mesh vertex %d",
						this->pints[3],
						obj->tlist[this->pints[3]].x,
						obj->tlist[this->pints[3]].y,
						obj->plist[poly].vert[1]);

					KX_ERROR("\nAssigning texture vertex index %d [%f, %f] to mesh vertex %d",
						this->pints[1],
						obj->tlist[this->pints[1]].x,
						obj->tlist[this->pints[1]].y,
						obj->plist[poly].vert[2]);
				}
				else
				{
					poly_num_verts = 3;
					obj->plist[poly].vert[0] = this->pints[0];
					obj->plist[poly].vert[1] = this->pints[2];
					obj->plist[poly].vert[2] = this->pints[4];

					obj->plist[poly].text[0] = this->pints[1];
					obj->plist[poly].text[1] = this->pints[3];
					obj->plist[poly].text[2] = this->pints[5];

					KX_ERROR("\nAssigning texture vertex index %d [%f, %f] to mesh vertex %d",
						this->pints[5],
						obj->tlist[this->pints[5]].x,
						obj->tlist[this->pints[5]].y,
						obj->plist[poly].vert[0]);

					KX_ERROR("\nAssigning texture vertex index %d [%f, %f] to mesh vertex %d",
						this->pints[3],
						obj->tlist[this->pints[3]].x,
						obj->tlist[this->pints[3]].y,
						obj->plist[poly].vert[1]);

					KX_ERROR("\nAssigning texture vertex index %d [%f, %f] to mesh vertex %d",
						this->pints[1],
						obj->tlist[this->pints[1]].x,
						obj->tlist[this->pints[1]].y,
						obj->plist[poly].vert[2]);
				}

				obj->plist[poly].vlist = obj->vlist_local;

				obj->plist[poly].tlist = obj->tlist;

				obj->plist[poly].state = POLY4D_STATE_ACTIVE;

				break;
			}
		}

		KX_ERROR("\nLocal material Index=%d, total materials for object = %d, vertex_indices [%d,%d, %d]",
			poly_material[poly],
			num_material_object,
			obj->plist[poly].vert[0],
			obj->plist[poly].vert[1],
			obj->plist[poly].vert[2]);
	}

	for (int curr_material = 0; curr_material < num_material_object; curr_material++)
	{
		while (true)
		{
			if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				KX_ERROR("\nmaterial list ended abruptly! in .COB file %s.", filename);
				return nullptr;
			}

			if (this->PatternMatch(this->buffer, "['mat#'] [i]"))
			{
				int material_index = this->pints[0];

				while (true)
				{
					if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
					{
						KX_ERROR("\nRGB color ended abruptly! in .COB file %s,", filename);
						return nullptr;
					}

					kxUtils::ReplaceChars(this->buffer, this->buffer, ",", ' ', 1);

					if (this->PatternMatch(this->buffer, "['rgb'] [f] [f] [f]"))
					{
						kxUtils::materials[material_index + kxUtils::num_materials].color->setRed(this->pfloats[0] * 255 + 0.5);
						kxUtils::materials[material_index + kxUtils::num_materials].color->setGreen(this->pfloats[1] * 255 + 0.5);
						kxUtils::materials[material_index + kxUtils::num_materials].color->setBlue(this->pfloats[2] * 255 + 0.5);
						break;
					}
				}

				while (true)
				{
					if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
					{
						KX_ERROR("\nmaterial properties ended abruptly! in .COB file %s,", filename);
						return nullptr;
					}

					if (this->PatternMatch(this->buffer, "['alpha'] [f] ['ka'] [f] ['ks'] [f] ['exp'] [f]"))
					{
						kxUtils::materials[material_index + kxUtils::num_materials].color->setRed(this->pfloats[0] * 255 + 0.5);
						kxUtils::materials[material_index + kxUtils::num_materials].ka = this->pfloats[1];
						kxUtils::materials[material_index + kxUtils::num_materials].kd = 1;
						kxUtils::materials[material_index + kxUtils::num_materials].ks = this->pfloats[2];
						kxUtils::materials[material_index + kxUtils::num_materials].power = this->pfloats[3];

						//ambient reflectivity
						kxUtils::materials[material_index + kxUtils::num_materials].ra->setRed(
							kxUtils::materials[material_index + kxUtils::num_materials].ka*
							kxUtils::materials[material_index + kxUtils::num_materials].color->getRed + 0.5);
						kxUtils::materials[material_index + kxUtils::num_materials].ra->setBlue(
							kxUtils::materials[material_index + kxUtils::num_materials].ka*
							kxUtils::materials[material_index + kxUtils::num_materials].color->getBlue + 0.5);
						kxUtils::materials[material_index + kxUtils::num_materials].ra->setGreen(
							kxUtils::materials[material_index + kxUtils::num_materials].ka*
							kxUtils::materials[material_index + kxUtils::num_materials].color->getGreen + 0.5);

						// diffuse reflectivity
						kxUtils::materials[material_index + kxUtils::num_materials].ra->setRed(
							kxUtils::materials[material_index + kxUtils::num_materials].kd*
							kxUtils::materials[material_index + kxUtils::num_materials].color->getRed + 0.5);
						kxUtils::materials[material_index + kxUtils::num_materials].ra->setBlue(
							kxUtils::materials[material_index + kxUtils::num_materials].kd*
							kxUtils::materials[material_index + kxUtils::num_materials].color->getBlue + 0.5);
						kxUtils::materials[material_index + kxUtils::num_materials].ra->setGreen(
							kxUtils::materials[material_index + kxUtils::num_materials].kd*
							kxUtils::materials[material_index + kxUtils::num_materials].color->getGreen + 0.5);

						//specular reflectivity
						kxUtils::materials[material_index + kxUtils::num_materials].ra->setRed(
							kxUtils::materials[material_index + kxUtils::num_materials].ks*
							kxUtils::materials[material_index + kxUtils::num_materials].color->getRed + 0.5);
						kxUtils::materials[material_index + kxUtils::num_materials].ra->setBlue(
							kxUtils::materials[material_index + kxUtils::num_materials].ks*
							kxUtils::materials[material_index + kxUtils::num_materials].color->getBlue + 0.5);
						kxUtils::materials[material_index + kxUtils::num_materials].ra->setGreen(
							kxUtils::materials[material_index + kxUtils::num_materials].ks*
							kxUtils::materials[material_index + kxUtils::num_materials].color->getGreen + 0.5);

						break;
					}
				}
				while (true)
				{
					if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
					{
						KX_ERROR("\nShader class ended abruptly! in .COB file %s.", filename);
						return nullptr;
					}
					if (this->PatternMatch(this->buffer, "['Shader'] ['class:'] ['color']"))
					{
						break;
					}
				}

				while (true)
				{
					if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
					{
						KX_ERROR("\nShader name ended abruptly! in .COB file %s.", filename);
						return nullptr;
					}

					kxUtils::ReplaceChars(this->buffer, this->buffer, "\"", ' ', 1);

					if (this->PatternMatch(this->buffer, "['Shader'] ['name:'] ['plain'] ['color']"))
					{
						break;
					}

					if (this->PatternMatch(this->buffer, "['Shader'] ['name:'] ['texture'] ['map']"))
					{
						SET_BIT(kxUtils::materials[material_index + kxUtils::num_materials].attr, MAT_ATTR_SHADE_MODE_TEXTURE);

						while (true)
						{
							if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
							{
								KX_ERROR("\nCouldn't find texture name! in .COB file %s.", filename);
								return nullptr;
							}

							kxUtils::ReplaceChars(this->buffer, this->buffer, "\"", ' ', 1);

							if (this->PatternMatch(this->buffer, "['file'] ['name:'] ['string']"))
							{
								memcpy_s(kxUtils::materials[material_index + kxUtils::num_materials].texture_file,
									sizeof(kxUtils::materials[material_index + kxUtils::num_materials].texture_file),
									&this->buffer[18],
									strlen(this->buffer) - 18 + 2);

								if (!obj->texture)
								{
									obj->texture = new kxBitmap();

									char filename[80];
									char  path_filename[80];

									ExtractFilenameFromPath(kxUtils::materials[material_index + kxUtils::num_materials].texture_file, filename);

									strcpy_s(path_filename, texture_path);
									strcpy_s(path_filename, filename);

									obj->texture->load(path_filename);

									SET_BIT(obj->attr, OBJECT4D_ATTR_TEXTURES);
								}
								break;
							}
						}
						break;
					}
				}

				while (true)
				{
					if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
					{
						KX_ERROR("\nShader reflectance class not found in .COB file %s.", filename);
						return 0;
					}
					if (this->PatternMatch(this->buffer, "['Shader'] ['class:'] ['reflactance']"))
					{
						break;
					}
				}

				while (true)
				{
					if (!this->GetLine(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
					{
						KX_ERROR("\nShader name ended abruptly! in .COB file %s.", filename);
						return 0;
					}

					kxUtils::ReplaceChars(this->buffer, this->buffer, "\"", ' ', 1);

					if (this->PatternMatch(this->buffer, "['Shader'] ['name:'] [s>0]"))
					{
						if (strcmp(this->pstring[2], "constant") == 0)
						{
							SET_BIT(kxUtils::materials[material_index + kxUtils::num_materials].attr, MAT_ATTR_SHADE_MODE_CONSTANT);
						}
						else if (strcmp(this->pstring[2], "matte") == 0)
						{
							SET_BIT(kxUtils::materials[material_index + kxUtils::num_materials].attr, MAT_ATTR_SHADE_MODE_FLAT);
						}
						else if (strcmp(this->pstring[2], "plastic") == 0)
						{
							SET_BIT(kxUtils::materials[material_index + kxUtils::num_materials].attr, MAT_ATTR_SHADE_MODE_GOURAUD);
						}
						else if (strcmp(this->pstring[2], "phong") == 0)
						{
							SET_BIT(kxUtils::materials[material_index + kxUtils::num_materials].attr, MAT_ATTR_SHADE_MODE_FASTPHONG);
						}
						else
						{
							SET_BIT(kxUtils::materials[material_index + kxUtils::num_materials].attr, POLY4D_ATTR_SHADE_MODE_FLAT);
						}
						break;
					}
				}

				break;
			}
		}
	}

	for (int curr_poly = 0; curr_poly < obj->num_polys; curr_poly++)
	{
		KX_ERROR("\nFixing poly material %d from index %d", curr_poly,
			poly_material[curr_poly],
			poly_material[curr_poly] + kxUtils::num_materials);
		poly_material[curr_poly] = poly_material[curr_poly] + kxUtils::num_materials;

		SET_BIT(obj->plist[curr_poly].attr, POLY4D_ATTR_RGB16);
		if (kxUtils::materials[poly_material[curr_poly]].attr&MAT_ATTR_SHADE_MODE_TEXTURE)
		{
			obj->plist[curr_poly].color = new kxColor();
			obj->plist[curr_poly].color->setRGBA(255, 255, 255,255);
		}
		else
		{
			obj->plist[curr_poly].color = kxUtils::materials[poly_material[curr_poly]].color;
		}

		if (kxUtils::materials[poly_material[curr_poly]].attr&MAT_ATTR_SHADE_MODE_CONSTANT)
		{
			SET_BIT(obj->plist[curr_poly].attr,POLY4D_ATTR_SHADE_MODE_CONSTANT);
		}
		else if (kxUtils::materials[poly_material[curr_poly]].attr&MAT_ATTR_SHADE_MODE_FLAT)
		{
			SET_BIT(obj->plist[curr_poly].attr, POLY4D_ATTR_SHADE_MODE_FLAT);
		}else if (kxUtils::materials[poly_material[curr_poly]].attr&MAT_ATTR_SHADE_MODE_GOURAUD)
		{
			SET_BIT(obj->plist[curr_poly].attr, POLY4D_ATTR_SHADE_MODE_GOURAUD);

			SET_BIT(obj->vlist_local[obj->plist[curr_poly].vert[0]].attr, VERTEX4D_ATTR_NORMAL);
			SET_BIT(obj->vlist_local[obj->plist[curr_poly].vert[1]].attr, VERTEX4D_ATTR_NORMAL);
			SET_BIT(obj->vlist_local[obj->plist[curr_poly].vert[2]].attr, VERTEX4D_ATTR_NORMAL);
		}
		else if (kxUtils::materials[poly_material[curr_poly]].attr&MAT_ATTR_SHADE_MODE_FASTPHONG)
		{
			SET_BIT(obj->plist[curr_poly].attr, POLY4D_ATTR_SHADE_MODE_FASTPHONG);

			SET_BIT(obj->vlist_local[obj->plist[curr_poly].vert[0]].attr, VERTEX4D_ATTR_NORMAL);
			SET_BIT(obj->vlist_local[obj->plist[curr_poly].vert[1]].attr, VERTEX4D_ATTR_NORMAL);
			SET_BIT(obj->vlist_local[obj->plist[curr_poly].vert[2]].attr, VERTEX4D_ATTR_NORMAL);
		}
		else
		{
			SET_BIT(obj->plist[curr_poly].attr, POLY4D_ATTR_SHADE_MODE_FLAT);
		}
			
		if (kxUtils::materials[poly_material[curr_poly]].attr&MAT_ATTR_SHADE_MODE_TEXTURE)
		{
			SET_BIT(obj->plist[curr_poly].attr, POLY4D_ATTR_SHADE_MODE_TEXTURE);

			obj->plist[curr_poly].texture = obj->texture;

			SET_BIT(obj->vlist_local[obj->plist[curr_poly].vert[0]].attr, VERTEX4D_ATTR_TEXTURE);
			SET_BIT(obj->vlist_local[obj->plist[curr_poly].vert[1]].attr, VERTEX4D_ATTR_TEXTURE);
			SET_BIT(obj->vlist_local[obj->plist[curr_poly].vert[2]].attr, VERTEX4D_ATTR_TEXTURE);
		}

		SET_BIT(obj->plist[curr_poly].attr, POLY4D_ATTR_DISABLE_MATERIAL);
	}

	kxUtils::num_materials += num_material_object;

	if (obj->texture)
	{
		for (int tvertex = 0; tvertex < num_texture_vertices; tvertex++)
		{
			int texture_size = obj->texture->width;

			obj->tlist[tvertex].x *= (texture_size - 1);
			obj->tlist[tvertex].y *= (texture_size - 1);

			if (vertex_flag&VERTEX_FLAGS_INVERT_TEXTURE_U)
			{
				obj->tlist[tvertex].x = (texture_size - 1) - obj->tlist[tvertex].x;
			}
			if (vertex_flag&VERTEX_FLAGS_INVERT_TEXTURE_V)
			{
				obj->tlist[tvertex].y = (texture_size - 1) - obj->tlist[tvertex].y;
			}
			if (vertex_flag&VERTEX_FLAGS_INVERT_SWAP_UV)
			{
				float temp;
				SWAP(obj->tlist[tvertex].x, obj->tlist[tvertex].y, temp);
			}
		}
	}

	for (int curr_material = 0; curr_material < kxUtils::num_materials; curr_material++)
	{
		KX_ERROR("\nMaterial %d", curr_material);

		KX_ERROR("\nint  state    = %d", kxUtils::materials[curr_material].state);
		KX_ERROR("\nint  id       = %d", kxUtils::materials[curr_material].id);
		KX_ERROR("\nchar name[64] = %s", kxUtils::materials[curr_material].name);
		KX_ERROR("\nint  attr     = %d", kxUtils::materials[curr_material].attr);
		KX_ERROR("\nint r         = %d", kxUtils::materials[curr_material].color->getRed());
		KX_ERROR("\nint g         = %d", kxUtils::materials[curr_material].color->getGreen());
		KX_ERROR("\nint b         = %d", kxUtils::materials[curr_material].color->getBlue());
		KX_ERROR("\nint alpha     = %d", kxUtils::materials[curr_material].color->getAlpha());
		KX_ERROR("\nint color     = %d", kxUtils::materials[curr_material].attr);
		KX_ERROR("\nfloat ka      = %f", kxUtils::materials[curr_material].ka);
		KX_ERROR("\nkd            = %f", kxUtils::materials[curr_material].kd);
		KX_ERROR("\nks            = %f", kxUtils::materials[curr_material].ks);
		KX_ERROR("\npower         = %f", kxUtils::materials[curr_material].power);
		KX_ERROR("\nchar texture_file = %s\n", kxUtils::materials[curr_material].texture_file);
	}

	obj->ComputePolyNormals();
	obj->ComputeVertexNormasl();

	return obj;
}
