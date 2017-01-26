#pragma once
#include <d3d9.h>
#include <d3dx9.h>

namespace KevinX
{
	class kxDirectX
	{
	public:
		HRESULT InitD3D(HWND hwnd);
		void CleanUp();
		void Render();

	private:
		LPDIRECT3D9  pD3D = NULL;
		LPDIRECT3DDEVICE9 pD3DDevice = NULL;
		LPD3DXLINE pLine = NULL;
		D3DXVECTOR2*  pLineArr = NULL;
	};
}


