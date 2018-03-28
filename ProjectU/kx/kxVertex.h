#pragma once
#include "kxGlobal.h"
#include "kxVector.h"
#include "kxColor.h"

KX_BEGIN
class kxVertex
{
public:
	kxVector4 position;
	kxVector4 normal;
	kxVector2 textureUV;

	kxColor i; 
	int attr;
};
KX_END
