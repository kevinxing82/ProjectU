#pragma once
#include "kxGlobal.h"
#include "kxMaterial.h"

KX_BEGIN
static kxMaterial materials[MAX_MATERIALS];
static int num_materials;
class kxUtils
{
public:
	static int StripChars(char *string_in, char *string_out, char *strip_chars, int case_on=1);
	static int ReplaceChars(char * string_in, char *string_out, char *replace_chars, char rep_char, int case_on = 1);
	static char *StringRtrim(char *string);
	static char *StringLtrim(char *string);
};
KX_END
