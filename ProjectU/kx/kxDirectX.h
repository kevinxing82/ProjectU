#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include "kxGlobal.h"
#include "kxRenderList.h"
#include "kxPolygon.h"

KX_BEGIN
#define D3DFVF (D3DFVF_XYZ|D3DFVF_DIFFUSE)
class kxDirectX
{
public:
	HRESULT InitD3D(HWND hwnd);
	void CleanUp();
	void Render(const kxRenderList& renderList);
	void DrawPolygon(const kxPolygonList* polyList);
	void DrawTriangle();
	void DrawBackground();

private:
	struct Vertex
	{
		float x, y, z;
		DWORD color;
	};
	IDirect3DVertexBuffer9* Triangle = NULL;
	LPDIRECT3D9  pD3D = NULL;
	LPDIRECT3DDEVICE9 pD3DDevice = NULL;
	LPD3DXLINE pLine = NULL;
	D3DXVECTOR2*  pLineArr = NULL;
};
KX_END


