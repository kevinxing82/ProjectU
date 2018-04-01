#pragma once
#include "kxGlobal.h"
#include "kxMaterial.h"

KX_BEGIN
class kxUtils
{
public:		 
	static kxMaterial materials[MAX_MATERIALS];
	static int num_materials;
public:
	static int StripChars(char *string_in, char *string_out, char *strip_chars, int case_on=1);
	static int ReplaceChars(char * string_in, char *string_out, char *replace_chars, char rep_char, int case_on = 1);

};
KX_END
