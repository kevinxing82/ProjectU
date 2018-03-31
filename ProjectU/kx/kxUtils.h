#pragma once
#include "kxGlobal.h"

KX_BEGIN
class kxUtils
{
public:
	static int StripChars(char *string_in, char *string_out, char *strip_chars, int case_on=1);
};
KX_END
