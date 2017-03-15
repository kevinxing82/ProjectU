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

	D3DXFONT_DESC df;
	ZeroMemory(&df, sizeof(D3DXFONT_DESC));
	df.Height = 25;
	df.Width = 12;
	df.MipLevels = D3DX_DEFAULT;
	df.Italic = false;
	df.CharSet = DEFAULT_CHARSET;
	df.OutputPrecision = 0;
	df.Quality = 0;
	df.PitchAndFamily = 0;

	char* str = "TIME NEW ROMAN";
	size_t len = strlen(str) + 1;
	size_t converted = 0;
	mbstowcs_s(&converted, df.FaceName, len, str, _TRUNCATE);

	if (FAILED(D3DXCreateFontIndirect(pD3DDevice, &df, &font)))
	{
		return E_FAIL;
	}
	return S_OK;
}

void kxDirectX::SetRender(kxRenderer * render)
{
	mRender = render;
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
		DrawBackground();
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
		DrawTxt();
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
	pLine->Draw(lineArr, 4, polyList->color);
}

void kxDirectX::DrawTriangle()
{
	pD3DDevice->CreateVertexBuffer(6 * sizeof(Vertex), D3DUSAGE_WRITEONLY, D3DFVF, D3DPOOL_MANAGED, &Triangle, NULL);

	Vertex vertices[] =
	{
		{ -2.7f, 2.0f, 2.0f , D3DCOLOR_XRGB(89,195,226) },
		{ 2.7f, 2.0f, 2.0f , D3DCOLOR_XRGB(89,195,226) },
		{ -2.7f, 0.0f, 2.0f , D3DCOLOR_XRGB(89,195,226) },
		{ 2.7f, 0.0f, 2.0f , D3DCOLOR_XRGB(89,195,226) },
		{ -2.7f, 0.0f, 2.0f , D3DCOLOR_XRGB(89,195,226) },
		{ 2.7f, 2.0f, 2.0f , D3DCOLOR_XRGB(89,195,226) }
		
	};

	void* pVertices;
	Triangle->Lock(0, 0, (void **)&pVertices, 0);
	memcpy(pVertices, vertices, sizeof(vertices));
	Triangle->Unlock();

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 1.0f, 1000.0f);

	pD3DDevice->SetTransform(D3DTS_PROJECTION, &proj);
	pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	pD3DDevice->SetStreamSource(0, Triangle, 0, sizeof(Vertex));
	pD3DDevice->SetFVF(D3DFVF);
	pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
}

void kxDirectX::DrawBackground()
{
	pD3DDevice->CreateVertexBuffer(6 * sizeof(Vertex), D3DUSAGE_WRITEONLY, D3DFVF, D3DPOOL_MANAGED, &Triangle, NULL);

	Vertex vertices[] =
	{
		{ -2.7f, 2.0f, 2.0f , D3DCOLOR_XRGB(89,195,226) },
		{ 2.7f, 2.0f, 2.0f , D3DCOLOR_XRGB(89,195,226) },
		{ -2.7f, 0.0f, 2.0f , D3DCOLOR_XRGB(89,195,226) },
		{ 2.7f, 0.0f, 2.0f , D3DCOLOR_XRGB(89,195,226) },
		{ -2.7f, 0.0f, 2.0f , D3DCOLOR_XRGB(89,195,226) },
		{ 2.7f, 2.0f, 2.0f , D3DCOLOR_XRGB(89,195,226) }

	};

	void* pVertices;
	Triangle->Lock(0, 0, (void **)&pVertices, 0);
	memcpy(pVertices, vertices, sizeof(vertices));
	Triangle->Unlock();

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 1.0f, 1000.0f);

	pD3DDevice->SetTransform(D3DTS_PROJECTION, &proj);
	pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	pD3DDevice->SetStreamSource(0, Triangle, 0, sizeof(Vertex));
	pD3DDevice->SetFVF(D3DFVF);
	pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
}

void kxDirectX::DrawTxt()
{
	RECT rect = { 0,0,WIN_WIDTH,WIN_HEIGHT };
	char* str = new CHAR[1024];
	sprintf(str, "Cam Pos:[%0.2f,%0.2f,%0.2f] Heading:[%f] Elev:[%f]", 
		mRender->mCamera.pos.x, mRender->mCamera.pos.y, mRender->mCamera.pos.z,
		mRender->mCamera.dir.y,
		mRender->mCamera.dir.x);
	font->DrawTextA(NULL, str, -1, &rect, DT_BOTTOM | DT_LEFT, 0xff00ff00);
}


