#include "kxParser.h"

using namespace KevinX;

KevinX::kxParser::kxParser()
{
}

KevinX::kxParser::~kxParser()
{
}

int KevinX::kxParser::Reset()
{
	return 0;
}

int KevinX::kxParser::Open(char * filename)
{
	return 0;
}

int KevinX::kxParser::Close()
{
	return 0;
}

char * KevinX::kxParser::Getline(int mode)
{
	return nullptr;
}

int KevinX::kxParser::SetComment(char * string)
{
	return 0;
}

int KevinX::kxParser::PatternMatch(char * string, char * pattern, ...)
{
	return 0;
}

int kxParser::Load_Object_PLG(kxRenderObject * obj, char * filename, kxVector4 * scale, kxVector4 * pos, kxVector4 * rot)
{
	FILE *fp;
	char buffer[256];

	char *token_string;

	ZeroMemory(obj, sizeof(kxRenderObject));

	return 0;
}
