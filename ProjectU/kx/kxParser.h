#pragma once
#include <iostream>
#include "kxGlobal.h"
#include "kxVector.h"
#include "kxRenderObject.h"

KX_BEGIN
	class kxParser
	{
	public:
		kxParser();
		~kxParser();

		int Reset();
		int Open(char* filename);
		int Close();

		char* GetLine(int mode);
		char* GetLine_PLG(char* buffer, int maxLength, FILE* fp);

		int SetComment(char* string);
		int PatternMatch(char* string, char* pattern, ...);

		int Load_Object_PLG(kxRenderObject* obj, char* filename, kxVector4* scale, kxVector4* pos, kxVector4* rot);

		float ComputeRenderObjectRadius(kxRenderObject* obj);

	public:
		FILE *fsteam;
		char buffer[PARSER_BUFFER_SIZE];
		int length;
		int num_lines;
		char comment[PARSER_MAX_COMMENT];

		char pstring[PATTERN_MAX_ARGS][PATTERN_BUFFER_SIZE];
		int num_pstrings;

		float pfloats[PATTERN_MAX_ARGS];
		int num_pfloats;
	};
KX_END