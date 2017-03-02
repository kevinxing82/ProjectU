#include "kxDirectX.h"

using namespace KevinX;
HRESULT kxDirectX::InitD3D(HWND hWnd)
{
	if (NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		return E_FAIL;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	// 创建D3D设备
	// 第一个参数：使用默认的显卡适配器
	// 第二个参数：请求使用硬件抽象层（HAL）
	// 第三个参数：窗口句柄
	// 第四个参数：使用软件处理顶点
	// 第五个参数：创建的参数
	// 第六个参数：创建的D3D设备指针
	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&pD3DDevice)))
	{
		E_FAIL;
	}

	if (FAILED(D3DXCreateLine(pD3DDevice, &pLine)))
	{
		return E_FAIL;
	}

	pLineArr = new D3DXVECTOR2[4];
	pLineArr[0].x = pLineArr[0].y = 50;
	pLineArr[1].x = 400;
	pLineArr[1].y = 50;
	pLineArr[2].x = 50;
	pLineArr[2].y = 400;
	pLineArr[3].x = 400;
	pLineArr[3].y = 400;

	return S_OK;
}

void kxDirectX::CleanUp()
{
	if (pLine != NULL)
	{
		pLine->Release();
	}
	if (pD3DDevice != NULL)
	{
		pD3DDevice->Release();
	}
	if (pD3D != NULL)
	{
		pD3D->Release();
	}
}

void kxDirectX::Render(const kxRenderList& renderList)
{
	pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(43, 43, 43), 1.0f, 0);
	if (SUCCEEDED(pD3DDevice->BeginScene()))
	{
		pLine->SetWidth(1.0f);
		pLine->SetAntialias(TRUE);
		for (int poly = 0; poly < renderList.num_polys; poly++)
		{
			if (!(renderList.poly_ptrs[poly]->state&POLY4DV1_STATE_ACTIVE) ||
				(renderList.poly_ptrs[poly]->state&POLY4DV1_STATE_CLIPPED) ||
				(renderList.poly_ptrs[poly]->state&POLY4DV1_STATE_BACKFACE))
			{
				continue;
			}
			DrawPolygon(renderList.poly_ptrs[poly]);
		}
		pD3DDevice->EndScene();
	}
	pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

void kxDirectX::DrawPolygon(const kxPolygonList * polyList)
{
	D3DXVECTOR2* lineArr = new D3DXVECTOR2[4];
	lineArr[0].x = polyList->tlist[0].x;
	lineArr[0].y = polyList->tlist[0].y;
	lineArr[1].x = polyList->tlist[1].x;
	lineArr[1].y = polyList->tlist[1].y;
	lineArr[2].x = polyList->tlist[2].x;
	lineArr[2].y = polyList->tlist[2].y;
	lineArr[3].x = polyList->tlist[0].x;
	lineArr[3].y = polyList->tlist[0].y;
	pLine->Draw(lineArr, 4, 0xffffffff);
}


