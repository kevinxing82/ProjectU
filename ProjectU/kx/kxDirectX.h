#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include "kxGlobal.h"
#include "kxRenderList.h"

KX_BEGIN
class kxDirectX
{
public:
	HRESULT InitD3D(HWND hwnd);
	void CleanUp();
	void Render(const kxRenderList& renderList);

private:
	LPDIRECT3D9  pD3D = NULL;
	LPDIRECT3DDEVICE9 pD3DDevice = NULL;
	LPD3DXLINE pLine = NULL;
	D3DXVECTOR2*  pLineArr = NULL;
};
KX_END


