#include "kxDrawer.h"
#include <math.h>
#include <assert.h>
#include <iostream>

USING_KX
kxDrawer::kxDrawer()
{
}

kxDrawer::~kxDrawer()
{
	if (framebuffer)
	{
		delete(framebuffer);
	}
	framebuffer = NULL;
	zbuffer = NULL;
	texture = NULL;
}

int kxDrawer::Init(HWND hwnd, int width, int height)
{
	this->hwnd = hwnd;

	BITMAPINFO bi = { {sizeof(BITMAPINFOHEADER),width,-height,1,32,BI_RGB,width*height * 4,0,0,0,0} };
	LPVOID lptr;
	HDC	 hDC = GetDC(hwnd);
	screenHDC = CreateCompatibleDC(hDC);
	ReleaseDC(hwnd, hDC);

	screenHB = CreateDIBSection(screenHDC, &bi, DIB_RGB_COLORS, &lptr, 0, 0);
	if (screenHB == NULL)
	{
		return -3;
	}

	screenOB = (HBITMAP)SelectObject(screenHDC, screenHB);
	screenFB = (unsigned char*)lptr;
	screenPitch = width * 4;
	memset(screenFB, 0, width*height * 4);


	float need = sizeof(void*)*(height * 2 + 1024) + width*height * 8 + 64;
	char *ptr = new char[need];
	char *framebuf, *zbuf;
	int j;
	assert(ptr);
	this->framebuffer = (IUINT32**)ptr;
	this->zbuffer = (float**)(ptr + sizeof(void*)*height);
	ptr += sizeof(void*)*height * 2;
	this->texture = (IUINT32**)ptr;
	ptr += sizeof(void*) * 1024;
	//framebuf = (char*)ptr;
	zbuf = (char*)ptr + width*height * 4;
	ptr += width*height * 8;
	framebuf = (char*)screenFB;

	for (j = 0; j < height; j++)
	{
		this->framebuffer[j] = (IUINT32*)(framebuf + width * 4 * j);
		this->zbuffer[j] = (float*)(zbuf + width * 4 * j);
	}

	this->texture[0] = (IUINT32*)ptr;
	this->texture[1] = (IUINT32*)(ptr + 16);
	memset(this->texture[0], 0, 64);
	this->tex_width = 2;
	this->tex_height = 2;
	this->max_u = 1.0f;
	this->max_v = 1.0f;
	this->width = width;
	this->height = height;
	this->background = 0x0c0c0;
	this->foreground = 0;
	render_state = RENDER_STATE_WIREFRAME;
	return E_NOTIMPL;
}

void kxDrawer::SetTexture(void * bits, long pitch, int w, int h)
{
	char* ptr = (char*)bits;
	int j;
	assert(w <= 1024 && h <= 1024);
	for (j = 0; j < h; ptr += pitch, j++)
	{
		texture[j] = (IUINT32*)ptr;
	}
	this->tex_width = w;
	this->tex_height = h;
	this->max_u = (float)(w - 1);
	this->max_v = (float)(h - 1);

}

void kxDrawer::Clear(int mode)
{
	int y, x, height = this->height;
	for (y = 0; y < this->height; y++)
	{
		IUINT32 *dst = this->framebuffer[y];
		IUINT32 cc = (height - 1 - y) * 230 / (height - 1);
		cc = (cc << 16) | (cc << 8) | cc;
		if (mode == 0)
		{
			cc = this->background;
		}
		for (x = this->width; x > 0; dst++, x--)
		{
			dst[0] = 0.0f;
		}
	}
}

void kxDrawer::SetPixel(int x, int y, IUINT32 color)
{
	if (((IUINT32)x) < (IUINT32)this->width && ((IUINT32)y) < (IUINT32)this->height)
	{
		this->framebuffer[y][x] = color;
	}
}

void kxDrawer::DrawLine(int x1, int y1, int x2, int y2, IUINT32 c)
{
	int x, y, rem = 0;
	if (x1 == x2&&y1 == y2)
	{
		SetPixel(x1, y1, c);
	}
	else if (x1 == x2)
	{
		int inc = (y1 <= y2) ? 1 : -1;
		for (y = y1; y != y2; y += inc)
		{
			SetPixel(x1, y, c);
		}
		SetPixel(x2, y2, c);
	}
	else if (y1 == y2)
	{
		int inc = (x1 <= x2) ? 1 : -1;
		for (x = x1; x != x2; x += inc)
		{
			SetPixel(x, y1, c);
		}
		SetPixel(x2, y2, c);
	}
	else
	{
		int dx = (x1 < x2) ? x2 - x1 : x1 - x2;
		int dy = (y1 < y2) ? y2 - y1 : y1 - y2;
		if (dx >= dy)
		{
			if (x2 < x1)x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
			for (x = x1, y = y1; x <= x2; x++)
			{
				SetPixel(x, y, c);
				rem += dy;
				if (rem >= dx)
				{
					rem -= dx;
					y += (y2 >= y1) ? 1 : -1;
					SetPixel(x, y, c);
				}
			}
			SetPixel(x2, y2, c);
		}
		else
		{
			if (y2 < y1)x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
			for (x = x, y = y1; y <= y2; y++)
			{
				SetPixel(x, y, c);
				rem += dx;
				if (rem >= dy)
				{
					rem -= dy;
					x += (x2 >= x1) ? 1 : -1;
					SetPixel(x, y, c);
				}
			}
			SetPixel(x2, y2, c);
		}
	}
}

void kxDrawer::DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, int color)
{
	float tmp_x;
	float tmp_y;
	float new_x;
	if ((FCMP(x1, x2) && FCMP(x2, x3)) || (FCMP(y1, y2) && FCMP(y2, y3)))
	{
		return;
	}

	if (y2 < y1)
	{
		SWAP(x1, x2, tmp_x);
		SWAP(y1, y2, tmp_y);
	}

	if (y3 < y1)
	{
		SWAP(x1, x3, tmp_x);
		SWAP(y1, y3, tmp_y);
	}

	if (y3 < y2)
	{
		SWAP(x2, x3, tmp_x);
		SWAP(y2, y3, tmp_y);
	}

	if (y3<min_clip_y || y1>max_clip_y ||
		(x1 < min_clip_x&&x2 < min_clip_x&&x3 < min_clip_x) ||
		(x1 > max_clip_x&&x2 > max_clip_x&&x3 > max_clip_x))
		return;

	if (FCMP(y1, y2))
	{
		DrawTopTriFP(x1, y1, x2, y2, x3, y3, color);
	}
	else if (FCMP(y2, y3))
	{
		DrawBottomTriFP(x1, y1, x2, y2, x3, y3, color);
	}
	else
	{
		new_x = x1 + (int)(0.5 + (float)(y2 - y1)*(float)(x3 - x1) / (float)(y3 - y1));
		DrawBottomTriFP(x1, y1, new_x, y2, x2, y2, color);
		DrawTopTriFP(x2, y2, new_x, y2, x3, y3, color);
	}
}

void kxDrawer::DrawTopTriFP(float x1, float y1, float x2, float y2, float x3, float y3, int color)
{
	float dx_right,
		dx_left,
		xs, xe,
		height,
		temp_x,
		temp_y,
		right,
		left;

	int iy1, iy3, loop_y;

	if (x2 < x1)
	{
		SWAP(x1, x2, temp_x);
	}

	height = y3 - y1;

	dx_left = (x3 - x1) / height;
	dx_right = (x3 - x2) / height;

	xs = x1;
	xe = x2;

#if(RASTERIZER_MODE==RASTERIZER_ACCURATE)
	if (y1 < min_clip_y)
	{
		xs = xs + dx_left*(-y1 + min_clip_y);
		xe = xe + dx_right*(-y1 + min_clip_y);

		y1 = min_clip_y;

		iy1 = y1;
	}
	else
	{
		iy1 = ceil(y1);

		xs = xs + dx_left*(iy1 - y1);
		xe = xe + dx_right*(iy1 - y1);
	}

	if (y3 > max_clip_y)
	{
		y3 = max_clip_y;
		iy3 = y3 - 1;
	}
	else
	{
		iy3 = ceil(y3) - 1;
	}
#endif

#if((RASTERIZER_MODE==RASTERIZER_FAST)||(RASTERIZER_MODE==RASTERIZER_FASTEST))
	if (y1 < min_clip_y)
	{
		xs = xs + dx_left*(-y1 + min_clip_y);
		xe = xe + dx_right*(-y1 + min_clip_y);
	}

	if (y3 > max_clip_y)
		y3 = max_clip_y;

	iy1 = ceil(y1);
	iy3 = ceil(y3) - 1;
#endif

	if (x1 >= min_clip_x&&x1 <= max_clip_x&&
		x2 >= min_clip_x&&x2 <= max_clip_x&&
		x3 >= min_clip_x&&x3 <= max_clip_x)
	{
		for (loop_y = iy1; loop_y <= iy3; loop_y++)
		{
			DrawLine(xs, loop_y, xe, loop_y, color);
			xs += dx_left;
			xe += dx_right;
		}
	}
	else
	{
		for (loop_y = iy1; loop_y <= iy3; loop_y++)
		{
			left = xs;
			right = xe;

			xs += dx_left;
			xe += dx_right;

			if (left < min_clip_x)
			{
				left = min_clip_x;
				if (right < min_clip_x)
					continue;
			}

			if (right > max_clip_x)
			{
				right = max_clip_x;
				if (left > max_clip_x)
					continue;
			}
			DrawLine(xs, loop_y, xe, loop_y, color);
		}
	}
}

void kxDrawer::DrawBottomTriFP(float x1, float y1, float x2, float y2, float x3, float y3, int color)
{
	float dx_right,
		dx_left,
		xs, xe,
		height,
		temp_x,
		temp_y,
		right,
		left;
	int iy1, iy3, loop_y;

	if (x3 < x2)
	{
		SWAP(x2, x3, temp_x);
	}

	height = y3 - y1;

	dx_left = (x2 - x1) / height;
	dx_right = (x3 - x1) / height;

	xs = x1;
	xe = x1;

#if(RASTERIZER_MODE==RASTERIZER_ACCURATE)
	if (y1 < min_clip_y)
	{
		xs = xs + dx_left*(-y1 + min_clip_y);
		xe = xe + dx_right*(-y1 + min_clip_y);

		y1 = min_clip_y;
		iy1 = y1;
	}
	else
	{
		iy1 = ceil(y1);

		xs = xs + dx_left*(iy1 - y1);
		xe = xe + dx_right*(iy1 - y1);
	}
	if (y3 > max_clip_y)
	{
		y3 = max_clip_y;
		iy3 = y3 - 1;
	}
	else
	{
		iy3 = ceil(y3) - 1;
	}
#endif

#if((RASTERIZER_MODE==RASTERIZER_FAST)||(RASTERIZER_MODE==RASTERIZER_FASTEST))
	if (y1 < min_clip_y)
	{
		xs = xs + dx_left*(-y1 + min_clip_y);
		xe = xe + dx_right*(-y1 + min_clip_y);
	}

	if (y3 > max_clip_y)
		y3 = max_clip_y;

	iy1 = ceil(y1);
	iy3 = ceil(y3) - 1;
#endif

	if (x1 >= min_clip_x&&x1 <= max_clip_x&&
		x2 >= min_clip_x&&x2 <= max_clip_x&&
		x3 >= min_clip_x&&x3 <= max_clip_x)
	{
		for (loop_y = iy1; loop_y <= iy3; loop_y++)
		{
			DrawLine(xs, loop_y, xe, loop_y, color);
			xs += dx_left;
			xe += dx_right;
		}
	}
	else
	{
		for (loop_y = iy1; loop_y <= iy3; loop_y++)
		{
			left = xs;
			right = xe;

			xs += dx_left;
			xe += dx_right;

			if (left < min_clip_x)
			{
				left = min_clip_x;
				if (right < min_clip_x)
					continue;
			}

			if (right > max_clip_x)
			{
				right = max_clip_x;
				if (left > max_clip_x)
					continue;
			}
			DrawLine(xs, loop_y, xe, loop_y, color);
		}
	}
}

void kxDrawer::DrawGouraudTriangle(float arg_x0, float arg_y0, kxColor* arg_color0,
	float arg_x1, float arg_y1, kxColor* arg_color1,
	float arg_x2, float arg_y2, kxColor* arg_color2)
{
	int temp = 0,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	kxColor* tempColor = nullptr;

	int dx, dy, dyL, dyR,
		u, v, w,
		du, dv, dw,
		xi, yi,
		ui, vi, wi,
		index_x, index_y,
		x, y,
		xstart, xend,
		ystart, yrestart, yend,
		xL,
		dxdyL,
		xR,
		dxdyR,
		dudyL,
		uL,
		dvdyL,
		vL,
		dwdyL,
		wL,
		dudyR,
		uR,
		dvdyR,
		vR,
		dwdyR,
		wR;
	int x0, y0, tu0, tv0, tw0,
		x1, y1, tu1, tv1, tw1,
		x2, y2, tu2, tv2, tw2;
	int r_base0, g_base0, b_base0,
		r_base1, g_base1, b_base1,
		r_base2, g_base2, b_base2;

	if (((arg_y0 < min_clip_y) && (arg_y1 < min_clip_y) && (arg_y2< min_clip_y)) ||
		((arg_y0 > max_clip_y) && (arg_y1 > max_clip_y) && (arg_y2 > max_clip_y)) ||
		((arg_x0 < min_clip_x) && (arg_x1 < min_clip_x) && (arg_x2 < min_clip_x)) ||
		((arg_x0 > max_clip_x) && (arg_x1 > max_clip_x) && (arg_x2 > max_clip_x)))
	{
		return;
	}

	if (((arg_x0 == arg_x1) && (arg_x1== arg_x2)) ||
		((arg_y0 == arg_y1) && (arg_y1 == arg_y2)))
	{
		return;
	}

	if (arg_y1 < arg_y0)
	{
		SWAP(arg_x0, arg_x1, temp);
		SWAP(arg_y0, arg_y1, temp);
		SWAP(arg_color0, arg_color1, tempColor);
	}

	if (arg_y2 < arg_y0)
	{
		SWAP(arg_x0, arg_x2, temp);
		SWAP(arg_y0, arg_y2, temp);		
		SWAP(arg_color0, arg_color2, tempColor);
	}

	if (arg_y2 < arg_y1)
	{
		SWAP(arg_x1, arg_x2, temp);
		SWAP(arg_y1, arg_y2, temp);
		SWAP(arg_color1, arg_color2, tempColor);
	}

	if (arg_y0 == arg_y1)
	{
		tri_type = TRI_TYPE_FLAT_TOP;
		if (arg_x1 < arg_x0)
		{
			SWAP(arg_x0, arg_x1, temp);
			SWAP(arg_y0, arg_y1, temp);
			SWAP(arg_color0, arg_color1, tempColor);
		}
	}
	else if (arg_y1 == arg_y2)
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;
		if (arg_x2 <arg_x1)
		{
			SWAP(arg_x1, arg_x2, temp);
			SWAP(arg_y1, arg_y2, temp);
			SWAP(arg_color1, arg_color2, tempColor);
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	r_base0 = arg_color0->getRed();
	g_base0 = arg_color0->getGreen();
	b_base0 = arg_color0->getBlue();

	r_base1 = arg_color1->getRed();
	g_base1 = arg_color1->getGreen();
	b_base1 = arg_color1->getBlue();

	r_base2 = arg_color2->getRed();
	g_base2 = arg_color2->getGreen();
	g_base2 = arg_color2->getBlue();

	x0 = (int)(arg_x0 + 0.5);
	y0 = (int)(arg_y0+ 0.5);

	tu0 = r_base0;
	tv0 = g_base0;
	tw0 = b_base0;

	x1 = (int)(arg_x1 + 0.5);
	y1 = (int)(arg_y1 + 0.5);

	tu1 = r_base1;
	tv1 = g_base1;
	tw1 = b_base1;

	x2 = (int)(arg_x2 + 0.5);
	y2 = (int)(arg_y2 + 0.5);

	tu2 = r_base2;
	tv2 = g_base2;
	tw2 = b_base2;
    
	yrestart = y1;
	if (tri_type&TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = (y2 - y0);

			dxdyL = (x2 - x0) / dy;
			dudyL = (tu2 - tu0) / dy;
			dvdyL = (tv2 - tv0) / dy;
			dwdyL = (tw2 - tw0) / dy;

			dxdyR = (x2 - x1) / dy;
			dudyR = (tu2 - tu1) / dy;
			dvdyR = (tv2 - tv1) / dy;
			dwdyR = (tw2 - tw1) / dy;

			if (y0 < min_clip_y)
			{
				dy = min_clip_y - y0;

				xL = dxdyL*dy + x0;
				uL = dudyL*dy + tu0;
				vL = dvdyL*dy + tv0;
				wL = dwdyL*dy + tw0;

				xR = dxdyR*dy + x1;
				uR = dudyR*dy + tu1;
				vR = dvdyR*dy + tv1;
				wR = dwdyR*dy + tw1;

				ystart = min_clip_y;
			}
			else
			{
				xL = x0;
				xR = x1;

				uL = tu0;
				vL = tv0;
				wL = tw0;

				uR = tu1;
				vR = tv1;
				wR = tw1;
				ystart = y0;
			}
		}
		else
		{
			dy = y1 - y0;

			dxdyL = (x1 - x0) / dy;
			dudyL = (tu1 - tu0) / dy;
			dvdyL = (tv1 - tv0) / dy;
			dwdyL = (tw1 - tw0) / dy;

			dxdyR = (x2 - x0) / dy;
			dudyR = (tu2 - tu0) / dy;
			dvdyR = (tv2 - tv0) / dy;
			dwdyR = (tw2 - tw0) / dy;

			if (y0 < min_clip_y)
			{
				dy = min_clip_y - y0;

				xL = dxdyL*dy + x0;
				uL = dudyL*dy + tu0;
				vL = dvdyL*dy + tv0;
				wL = dwdyL*dy + tw0;

				xR = dxdyR*dy + x0;
				uR = dudyR*dy + tu0;
				vR = dvdyR*dy + tv0;
				wR = dwdyR*dy + tw0;

				ystart = min_clip_y;
			}
			else
			{
				xL = x0;
				xR = x0;

				uL = tu0;
				vL = tv0;
				wL = tw0;

				uR = tu0;
				vR = tv0;
				wR = tw0;

				ystart = y0;
			}
		}
		if ((yend = y2) > max_clip_y)
		{
			yend = max_clip_y;
		}

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = xL;
				xend = xR;

				ui = uL;
				vi = vL;
				wi = wL;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (uR - uL) / dx;
					dv = (vR - vL) / dx;
					dw = (wR - wL) / dx;
				}
				else
				{
					du = uR - uL;
					dv = vR - vL;
					dw = wR - wL;
				}

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui += dx*du;
					vi += dx*dv;
					wi += dx*dw;

					xstart = min_clip_x;
				}

				if (xend > max_clip_x)
				{
					xend = max_clip_x;
				}

				kxColor* color = new kxColor();
				for (xi = xstart; xi <= xend; xi++)
				{
					color->setRGBA(ui, vi, wi, 1);
					SetPixel(xi, yi, color->getRGB());
					ui += du;
					vi += dv;
					wi += dw;
				}
				delete color;

				xL += dxdyL;
				uL += dudyL;
				vL += dvdyL;
				wL += dwdyL;

				xR += dxdyR;
				uR += dudyR;
				vR += dvdyR;
				wR += dwdyR;
			}
		}
		else
		{
			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = xL;
				xend = xR;

				ui = uL;
				vi = vL;
				wi = wL;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (uR - uL) / dx;
					dv = (vR - vL) / dx;
					dw = (wR - wL) / dx;
				}
				else
				{
					du = uR - uL;
					dv = vR - vL;
					dw = wR - wL;
				}

				kxColor* color = new kxColor();
				for (xi = xstart; xi <= xend; xi++)
				{
					color->setRGBA(ui, vi, wi, 1);
					SetPixel(xi, yi, color->getRGB());

					ui += du;
					vi += dv;
					wi += dw;
				}
				delete color;

				xL += dxdyL;
				uL += dudyL;
				vL += dvdyL;
				wL += dwdyL;

				xR += dxdyR;
				uR += dudyR;
				vR += dvdyR;
				wR += dwdyR;
			}
		}
	}
	else if (tri_type == TRI_TYPE_GENERAL)
	{
		if ((yend = y2) > max_clip_y)
		{
			yend = max_clip_y;
		}

		if (y1 < min_clip_y)
		{
			dyL = y2 - y1;

			dxdyL = (x2 - x1) / dyL;
			dudyL = (tu2 - tu1) / dyL;
			dvdyL = (tv2 - tv1) / dyL;
			dwdyL = tw2 - tw1 / dyL;

			dyR = y2 - y0;
			
			dxdyR = (x2 - x0) / dyR;
			dudyR = (tu2 - tu0) / dyR;
			dvdyR = (tv2 - tv0) / dyR;
			dwdyR = (tw2 - tw0) / dyR;

			dyR = min_clip_y - y0;
			dyL = min_clip_y - y1;

			xL = dxdyL*dyL + x1;
			uL = dudyL*dyL + tu1;
			vL = dvdyL*dyL + tv1;
			wL = dwdyL*dyL + tw1;

			xR = dxdyR*dyR + x0;
			uR = dudyR*dyR + tu0;
			vR = dvdyR*dyR + tv0;
			wR = dwdyR*dyR + tw0;

			ystart = min_clip_y;

			if (dxdyR > dxdyL)
			{
				SWAP(dxdyL, dxdyR, temp);
				SWAP(dudyL, dudyR, temp);
				SWAP(dvdyL, dvdyR, temp);
				SWAP(dwdyL, dwdyR, temp);
				SWAP(xL, xR, temp);
				SWAP(uL, uR, temp);
				SWAP(vL, vR, temp);
				SWAP(wL, wR, temp);
				SWAP(x1, x2, temp);
				SWAP(y1, y2, temp);
				SWAP(tu1, tu2, temp);
				SWAP(tv1, tv2, temp);
				SWAP(tw1, tw2, temp);

				irestart = INTERP_RHS;
			}
		}
		else if (y0 < min_clip_y)
		{
			dyL = y1 - y0;

			dxdyL = (x1 - x0) / dy;
			dudyL = (tu1 - tu0) / dy;
			dvdyL = (tv1 - tv0) / dy;
			dwdyL = (tw1 - tw0) / dy;

			dyR = y2 - y0;

			dxdyR = (x2 - x0) / dyR;
			dudyR = (tu2 - tu0) / dyR;
			dvdyR = (tv2 - tv0) / dyR;
			dwdyR = (tw2 - tw0) / dyR;

			dy = (min_clip_y - y0);

			xL = dxdyL*dy + x0;
			uL = dudyL*dy+tu0;
			vL = dvdyL*dy + tv0;
			wL = dwdyL*dy + tw0;

			xR = dxdyR*dy + x0;
			uR = dudyR*dy + tu0;
			vR = dvdyR*dy + tv0;
			wR = dwdyR*dy + tw0;

			ystart = min_clip_y;

			if (dxdyR < dxdyL)
			{
				SWAP(dxdyL, dxdyR, temp);
				SWAP(dudyL, dudyR, temp);
				SWAP(dvdyL, dvdyR, temp);
				SWAP(dwdyL, dwdyR, temp);
				SWAP(xL, xR, temp);
				SWAP(uL, uR, temp);
				SWAP(vL, vR, temp);
				SWAP(wL, wR, temp);
				SWAP(x1, x2, temp);
				SWAP(y1, y2, temp);
				SWAP(tu1, tu2, temp);
				SWAP(tv1, tv2, temp);
				SWAP(tw1, tw2, temp);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyL = y1 - y0;

			dxdyL = (x1 - x0) / dyL;
			dudyL = (tu1 - tu0) / dyL;
			dvdyL = (tv1 - tv0) / dyL;				   
			dwdyL = (tw1 - tw0) / dyL;

			dyR = y2 - y0;

			dxdyR = (x2 - x0) / dyR;
			dudyR = (tu2 - tu0) / dyR;
			dvdyR = (tv2 - tv0) / dyR;
			dwdyR = (tw2 - tw0) / dyR;

			xL = x0;
			xR = x0;

			uL = tu0;
			vL = tv0;
			wL = tw0;

			uR = tu0;
			vR = tv0;
			wR = tw0;
			
			ystart = y0;

			if (dxdyR < dxdyL)
			{
				SWAP(dxdyL, dxdyR, temp);
				SWAP(dudyL, dudyR, temp);
				SWAP(dvdyL, dvdyR, temp);
				SWAP(dwdyL, dwdyR, temp);
				SWAP(xL, xR, temp);
				SWAP(uL, uR, temp);
				SWAP(vL, vR, temp);
				SWAP(wL, wR, temp);
				SWAP(x1, x2, temp);
				SWAP(y1, y2, temp);
				SWAP(tu1, tu2, temp);
				SWAP(tv1, tv2, temp);
				SWAP(tw1, tw2, temp);

				irestart = INTERP_RHS;
			}
		}

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = xL;
				xend = xR;

				ui = uL;
				vi = vL;
				wi = wL;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (uR - uL) / dx;
					dv = (vR - vL) / dx;
					dw = (wR - wL) / dx;
				}
				else
				{
					du = uR - uL;
					dv = vR - vL;
					dw = wR - wL;
				}

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui += dx*du;
					vi += dx*dv;
					wi += dx*dw;

					xstart = min_clip_x;
				}

				if (xend > max_clip_x)
				{
					xend = max_clip_x;
				}

				kxColor* color = new kxColor();
				for (xi = xstart; xi <= xend; xi++)
				{
					color->setRGBA(ui, vi, wi, 1);
					SetPixel(xi, yi, color->getRGB());

					ui += du;
					vi += dv;
					wi += dw;
				}
				delete color;

				xL += dxdyL;
				uL += dudyL;
				vL += dvdyL;
				wL += dwdyL;

				xR += dxdyR;
				uR += dudyR;
				vR += dvdyR;
				wR += dwdyR;

				if (yi == yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyL = y2 - y1;

						dxdyL = (x2 - x1) / dyL;
						dudyL = (tu2 - tu1) / dyL;
						dvdyL = (tv2 - tv1) / dyL;
						dwdyL = (tw2 - tw1) / dyL;

						xL = x1;
						uL = tu1;
						vL = tv1;
						wL = tw1;

						xL += dxdyL;
						uL += dudyL;
						vL += dvdyL;
						wL += dwdyL;
					}
					else
					{
						dyR = y1 - y2;

						dxdyR = (x1 - x2) / dyR;
						dudyR = (tu1 - tu2) / dyR;
						dvdyR = (tv1 - tv2) / dyR;
						dwdyR = (tw1 - tw2) / dyR;

						xR = x2;
						uR = tu2;
						vR = tv2;
						wR = tw2;

						xR += dxdyR;
						uR += dudyR;
						vR += dvdyR;
						wR += dwdyR;
					}
				}
			}
		}
		else
		{
			// no x clipping
			// point screen ptr to starting line
			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = xL;
				xend = xR;

				ui = uL;
				vi = vL;
				wi = wL;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (uR - uL) / dx;
					dv = (vR - vL) / dx;
					dw = (wR - wL) / dx;
				}
				else
				{
					du = uR - uL;
					dv = vR - vL;
					dw = wR - wL;
				}

				kxColor* color = new kxColor();
				for (xi = xstart; xi < xend; xi++)
				{
					color->setRGBA(ui, vi, wi, 1);
					SetPixel(xi, yi, color->getRGB());

					ui += du;
					vi += dv;
					wi += dw;
				}
				delete color;

				xL += dxdyL;
				uL += dudyL;
				vL += dvdyL;
				wL += dwdyL;

				xR += dxdyR;
				uR += dudyR;
				vR += dvdyR;
				wR += dwdyR;

				if (yi == yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyL = y2 - y1;

						dxdyL = (x2 - x1) / dyL;
						dudyL = (tu2 - tu1) / dyL;
						dvdyL = (tv2 - tv1) / dyL;
						dwdyL = (tw2 - tw1) / dyL;

						xL = x1;
						uL = tu1;
						vL = tv1;
						wL = tw1;

						xL += dxdyL;
						uL += dudyL;
						vL += dvdyL;
						wL += dwdyL;
					}
					else
					{
						dyR = y1 - y2;

						dxdyR = (x1 - x2) / dyR;
						dudyR = (tu1 - tu2) / dyR;
						dvdyR = (tv1 - tv2) / dyR;
						dwdyR = (tw1 - tw2) / dyR;

						xR = x2;
						uR = tu2;
						vR = tv2;
						wR = tw2;

						xR += dxdyR;
						uR += dudyR;
						vR += dvdyR;
						wR += dwdyR;
					}
				}
			}
		}
	}
}

void kxDrawer::DrawTextureTriangle(float arg_x0, float arg_y0, float arg_tu0, float arg_tv0,
	float arg_x1, float arg_y1, float arg_tu1, float arg_tv1,
	float arg_x2, float arg_y2, float arg_tu2, float arg_tv2,kxBitmap* textmap)
{
	int temp = 0,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy, dyL, dyR,
		u, v,
		du, dv,
		xi, yi,
		ui, vi,
		index_x, index_y,
		x, y,
		xstart, xend,
		ystart, yrestart, yend,
		xL,
		dxdyL,
		xR,
		dxdyR,
		dudyL,
		uL,
		dvdyL,
		vL,
		dudyR,
		uR,
		dvdyR,
		vR;

	int x0, y0, tu0, tv0,
		x1, y1, tu1, tv1,
		x2, y2, tu2, tv2;
	int texture_shift2 = logbase2ofx[textmap->width];

	if (((arg_y0 < min_clip_y) &&(arg_y1 < min_clip_y) &&(arg_y2 < min_clip_y)) ||
		((arg_y0 > max_clip_y) && (arg_y1 > max_clip_y) &&(arg_y2 > max_clip_y))||
		((arg_x0 < min_clip_x) && (arg_x1< min_clip_x) && (arg_x2 < min_clip_x))||
		((arg_x0 > max_clip_x) && (arg_x1> max_clip_x) && (arg_x2 > max_clip_x)))
	{
		return;
	}

	if (((arg_x0 == arg_x1) && (arg_x1 == arg_x2)) ||((arg_y0 == arg_y1) && (arg_y1 == arg_y2)))
	{
		return;
	}

	if (arg_y1 < arg_y0)
	{
		SWAP(arg_x0, arg_x1, temp);
		SWAP(arg_y0, arg_y1, temp);
		SWAP(arg_tu0, arg_tu1, temp);
		SWAP(arg_tv0, arg_tv1, temp);
	}

	if (arg_y2 < arg_y0)
	{
		SWAP(arg_x0, arg_x2, temp);
		SWAP(arg_y0, arg_y2, temp);
		SWAP(arg_tu0, arg_tu2, temp);
		SWAP(arg_tv0, arg_tv2, temp);
	}

	if (arg_y2 < arg_y1)
	{
		SWAP(arg_x1, arg_x2, temp);
		SWAP(arg_y1, arg_y2, temp);
		SWAP(arg_tu1, arg_tu2, temp);
		SWAP(arg_tv1, arg_tv2, temp);
	}

	if (arg_y0 == arg_y1)
	{
		tri_type = TRI_TYPE_FLAT_TOP;
		if (arg_x1 < arg_x0)
		{
			SWAP(arg_x0, arg_x1, temp);
			SWAP(arg_y0, arg_y1, temp);
			SWAP(arg_tu0, arg_tu1, temp);
			SWAP(arg_tv0, arg_tv1, temp);
		}
	}
	else if(arg_y1 == arg_y2)
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;
		if (arg_x2 < arg_x1)
		{
			SWAP(arg_x1, arg_x2, temp);
			SWAP(arg_y1, arg_y2, temp);
			SWAP(arg_tu1, arg_tu2, temp);
			SWAP(arg_tv1, arg_tv2, temp);
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	x0 = (int)(arg_x0+0.5);
	y0 = (int)(arg_y0 + 0.5);
	tu0 = (int)(arg_tu0);
	tv0 = (int)(arg_tv0);

	x1 = (int)(arg_x1 + 0.5);
	y1 = (int)(arg_y1 + 0.5);
	tu1 = (int)(arg_tu1);
	tv1 = (int)(arg_tv1);

	x2 = (int)(arg_x2 + 0.5);
	y2 = (int)(arg_y2 + 0.5);
	tu2 = (int)(arg_tu2);
	tv2 = (int)(arg_tv2);

	yrestart = y1;

	if (tri_type&TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = y2 - y0;

			dxdyL = (x2 - x0) / dy;
			dudyL = (tu2 - tu0) / dy;
			dvdyL = (tv2 - tv0) / dy;

			dxdyR = (x2 - x1) / dy;
			dudyR = (tu2 - tu1) / dy;
			dvdyR = (tv2 - tv1) / dy;

			if (y0 < min_clip_y)
			{
				dy = min_clip_y - y0;

				xL = dxdyL*dy + x0;
				uL = dudyL*dy + tu0;
				vL = dvdyL*dy + tv0;

				xR = dxdyR*dy + x1;
				uR = dudyR*dy + tu1;
				vR = dvdyR*dy + tv1;

				ystart = min_clip_y;
			}
			else
			{
				xL = x0;
				xR = x1;

				uL = tu0;
				vL = tv0;

				uR = tu1;
				vR = tv1;

				ystart = y0;
			}
		}
		else
		{
			dy = y1 - y0;

			dxdyL = (x1 - x0) / dy;
			dudyL = (tu1 - tu0) / dy;
			dvdyL = (tv1 - tv0) / dy;

			dxdyR = (x2 - x0) / dy;
			dudyR = (tu2 - tu0) / dy;
			dvdyR = (tv2 - tv0) / dy;

			if (y0 < min_clip_y)
			{
				dy = min_clip_y - y0;

				xL = dxdyL*dy + x0;
				uL = dudyL*dy + tu0;
				vL = dvdyL*dy + tv0;

				xR = dxdyR*dy + x0;
				uR = dudyR*dy + tu0;
				vR = dvdyR*dy + tv0;

				ystart = min_clip_y;
			}
			else
			{
				xL = x0;
				xR = x0;

				uL = tu0;
				vL = tv0;

				uR = tu0;
				vR = tv0;

				ystart = y0;
			}
		}

		if ((yend = y2) > max_clip_y)
		{
			yend = max_clip_y;
		}

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{

			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = xL;
				xend = xR;

				ui = uL;
				vi = vL;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (uR - uL) / dx;
					dv = (vR - vL) / dx;
				}
				else
				{
					du = uR - uL;
					dv = vR - vL;
				}

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui += dx*du;
					vi += dx*dv;

					xstart = min_clip_x;
				}

				if (xend > max_clip_x)
				{
					xend = max_clip_x;
				}

				for (xi = xstart; xi <= xend; xi++)
				{
					// write textel
					SetPixel(xi, yi, textmap->bitmapData->buffer[ui + (vi << texture_shift2)]);

					ui += du;
					vi += dv;
				}

				xL += dxdyL;
				uL += dudyL;
				vL += dvdyL;

				xR += dxdyR;
				uR += dudyR;
				vR += dvdyR;
			}
		}
		else
		{
			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = xL;
				xend = xR;

				ui = uL;
				vi = vL;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (uR - uL) / dx;
					dv = (vR - vL) / dx;
				}
				else
				{
					du = uR - uL;
					dv = vR - vL;
				}

				for (xi = xstart; xi <= xend; xi++)
				{
					// write textel
					SetPixel(xi, yi, textmap->bitmapData->buffer[ui + (vi << texture_shift2)]);

					ui += du;
					vi += dv;
				}

				xL += dxdyL;
				uL += dudyL;
				vL += dvdyL;

				xR += dxdyR;
				uR += dudyR;
				vR += dvdyR;
			}
		}
	}
	else if(tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend = y2) > max_clip_y)
		{
			yend = max_clip_y;
		}

		if (y1 < min_clip_y)
		{
			dyL = y2 - y1;

			dxdyL = (x2 - x1) / dyL;
			dudyL = (tu2 - tu1) / dyL;
			dvdyL = (tv2 - tv1) / dyL;

			dyR = y2 - y0;
			dxdyR = (x2 - x0) / dyR;
			dudyR = (tu2 - tu0) / dyR;
			dvdyR = (tv2 - tv0) / dyR;

			dyR = min_clip_y - y0;
			dyL = min_clip_y - y1;

			xL = dxdyL*dyL + x1;
			uL = dxdyL*dyL + tu1;
			vL = dvdyL*dyL + tv1;

			xR = dxdyR*dyR + x0;
			uR = dudyR*dyR + tu0;
			vR = dvdyR*dyR + tv0;

			ystart = min_clip_y;

			if (dxdyR > dxdyL)
			{
				SWAP(dxdyL, dxdyR, temp);
				SWAP(dudyL, dudyR, temp);
				SWAP(dvdyL, dvdyR, temp);
				SWAP(xL, xR, temp);
				SWAP(uL, uR, temp);
				SWAP(vL, vR, temp);
				SWAP(x1, x2, temp);
				SWAP(y1, y2, temp);
				SWAP(tu1, tu2, temp);
				SWAP(tv1, tv2, temp);

				irestart = INTERP_RHS;
			}
		}
		else if (y0 < min_clip_y)
		{
			dyL = y1 - y0;

			dxdyL = (x1 - x0) / dyL;
			dudyL = (tu1 - tu0) / dyL;
			dvdyL = (tv1 - tv0) / dyL;

			dyR = y2 - y0;

			dxdyR = (x2 - x0) / dyR;
			dudyR = (tu2 - tu0) / dyR;
			dvdyR = (tv2 - tv0) / dyR;

			dy = min_clip_y - y0;

			xL = dxdyL*dy + x0;
			uL = dudyL*dy + tu0;
			vL = dvdyL*dy + tv0;

			xR = dxdyR*dy + x0;
			uR = dudyR*dy + tu0;
			vR = dvdyR*dy + tv0;

			ystart = min_clip_y;

			if (dxdyR < dxdyL)
			{
				SWAP(dxdyL, dxdyR, temp);
				SWAP(dudyL, dudyR, temp);
				SWAP(dvdyL, dvdyR, temp);
				SWAP(xL, xR, temp);
				SWAP(uL, uR, temp);
				SWAP(vL, vR, temp);
				SWAP(x1, x2, temp);
				SWAP(y1, y2, temp);
				SWAP(tu1, tu2, temp);
				SWAP(tv1, tv2, temp);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyL = y1 - y0;

			dxdyL = (x1 - x0) / dyL;
			dudyL = (tu1 - tu0) / dyL;
			dvdyL = (tv1 - tv0) / dyL;

			dyR = y2 - y0;
			dxdyR = (x2 - x0) / dyR;
			dudyR = (tu2 - tu0) / dyR;
			dvdyR = (tv2 - tv0) / dyR;

			xL = x0;
			xR = x0;

			uL = tu0;
			vL = tv0;

			uR = tu0;
			vR = tv0;

			ystart = y0;

			if (dxdyR < dxdyL)
			{
				SWAP(dxdyL, dxdyR, temp);
				SWAP(dudyL, dudyR, temp);
				SWAP(dvdyL, dvdyR, temp);
				SWAP(xL, xR, temp);
				SWAP(uL, uR, temp);
				SWAP(vL, vR, temp);
				SWAP(x1, x2, temp);
				SWAP(y1, y2, temp);
				SWAP(tu1, tu2, temp);
				SWAP(tv1, tv2, temp);

				irestart = INTERP_RHS;
			}
		}

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = xL;
				xend = xR;

				ui = uL;
				vi = vL;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (uR - uL) / dx;
					dv = (vR - vL) / dx;
				}
				else
				{
					du = uR - uL;
					dv = vR - vL;
				}

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui += dx*du;
					vi += dx*dv;

					xstart = min_clip_x;
				}

				if (xend > max_clip_x)
				{
					xend = max_clip_x;
				}

				for (xi = xstart; xi <= xend; xi++)
				{
					// write textel
					SetPixel(xi, yi, textmap->bitmapData->buffer[ui + (vi << texture_shift2)]);

					ui += du;
					vi += dv;
				}

				xL += dxdyL;
				uL += dudyL;
				vL += dvdyL;

				xR += dxdyR;
				uR += dudyR;
				vR += dvdyR;

				if (yi == yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyL = y2 - y1;

						dxdyL = (x2 - x1) / dyL;
						dudyL = (tu2 - tu1) / dyL;
						dvdyL = (tv2 - tv1) / dyL;

						xL = x1;
						uL = tu1;
						vL = tv1;

						xL += dxdyL;
						uL += dudyL;
						vL += dvdyL;
					}
					else
					{
						dyR = y1 - y2;

						dxdyR = (x1 - x2) / dyR;
						dudyR = (tu1 - tu2) / dyR;
						dvdyR = (tv1 - tv2) / dyR;

						xR = x2;
						uR = tu2;
						vR = tv2;

						xR += dxdyR;
						uR += dudyR;
						vR += dvdyR;
					}
				}
			}
		}
		else
		{
			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = xL;
				xend = xR;

				ui = uL;
				vi = vL;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (uR - uL) / dx;
					dv = (vR - vL) / dx;
				}
				else
				{
					du = uR - uL;
					dv = vR - vL;
				}

				for (xi = xstart; xi <= xend; xi++)
				{
					// write textel
					SetPixel(xi, yi, textmap->bitmapData->buffer[ui + (vi << texture_shift2)]);

					ui += du;
					vi += dv;
				}

				xL + dxdyL;
				uL += dudyL;
				vL += dvdyL;

				xR += dxdyR;
				uR += dudyR;
				vR += dvdyR;

				if (yi == yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyL = y2 - y1;

						dxdyL = (x2 - x1) / dyL;
						dudyL = (tu2 - tu1) / dyL;
						dvdyL = (tv2 - tv1) / dyL;

						xL = x1;
						uL = tu1;
						vL = tv1;

						xL += dxdyL;
						uL += dudyL;
						vL += dvdyL;
					}
					else
					{
						dyR = y1 - y2;

						dxdyR = (x1 - x2) / dyR;
						dudyR = (tu1 - tu2) / dyR;
						dvdyR = (tv1 - tv2) / dyR;

						xR = x2;
						uR = tu2;
						vR = tv2;

						xR += dxdyR;
						uR += dudyR;
						vR += dvdyR;
					}
				}
			}
		}
	}
}

void kxDrawer::DrawTextureTriangleFS(float arg_x0, float arg_y0, float arg_tu0, float arg_tv0,
	float arg_x1, float arg_y1, float arg_tu1, float arg_tv1,
	float arg_x2, float arg_y2, float arg_tu2, float arg_tv2,kxColor* litColor, kxBitmap* textmap)
{
	int temp = 0,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx, dy, dyL, dyR,
		u, v,
		du, dv,
		xi, yi,
		ui, vi,
		index_x, index_y,
		x, y,
		xstart, xend,
		ystart, yrestart, yend,
		xL,
		dxdyL,
		xR,
		dxdyR,
		dudyL,
		uL,
		dvdyL,
		vL,
		dudyR,
		uR,
		dvdyR,
		vR;

	USHORT r_base, g_base, b_base,
		r_textel, g_textel, b_textel, textel;

	int x0, y0, tu0, tv0,
		x1, y1, tu1, tv1,
		x2, y2, tu2, tv2;
	int texture_shift2 = logbase2ofx[textmap->width];

	if (((arg_y0 < min_clip_y) &&(arg_y1 < min_clip_y) &&(arg_y2 < min_clip_y)) ||
		((arg_y0 > max_clip_y) &&(arg_y1 > max_clip_y)&&(arg_y2 > max_clip_y))||
		((arg_x0 < min_clip_x)&&(arg_x1 < min_clip_x)&&(arg_x2 < min_clip_x))||
		((arg_x0 > max_clip_x)&&(arg_x1 > max_clip_x)&&(arg_x2 > max_clip_x)))
	{
		return;
	}

	if (((arg_x0 == arg_x1) && (arg_x1 == arg_x2)) ||((arg_y0 == arg_y1) && (arg_y1 == arg_y2)))
	{
		return;
	}

	if (arg_y1 < arg_y0)
	{
		SWAP(arg_x0, arg_x1, temp);
		SWAP(arg_y0, arg_y1, temp);
		SWAP(arg_tu0, arg_tu1, temp);
		SWAP(arg_tv0, arg_tv1, temp);
	}

	if (arg_y2 < arg_y0)
	{
		SWAP(arg_x0, arg_x2, temp);
		SWAP(arg_y0, arg_y2, temp);
		SWAP(arg_tu0, arg_tu2, temp);
		SWAP(arg_tv0, arg_tv2, temp);
	}

	if (arg_y2 < arg_y1)
	{
		SWAP(arg_x1, arg_x2, temp);
		SWAP(arg_y1, arg_y2, temp);
		SWAP(arg_tu1, arg_tu2, temp);
		SWAP(arg_tv1, arg_tv2, temp);
	}

	if (arg_y0 == arg_y1)
	{
		tri_type = TRI_TYPE_FLAT_TOP;
		if (arg_x1 < arg_x0)
		{
			SWAP(arg_x0, arg_x1, temp);
			SWAP(arg_y0, arg_y1, temp);
			SWAP(arg_tu0, arg_tu1, temp);
			SWAP(arg_tv0, arg_tv1, temp);
		}
	}
	else if (arg_y1 == arg_y2)
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;
		if (arg_x2 == arg_x1)
		{												
			SWAP(arg_x1, arg_x2, temp);
			SWAP(arg_y1, arg_y2, temp);
			SWAP(arg_tu1, arg_tu2, temp);
			SWAP(arg_tv1, arg_tv2, temp);
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	r_base = litColor->getRed();
	g_base = litColor->getGreen();
	b_base = litColor->getBlue();

	x0 = (int)(arg_x0 + 0.5);
	y0 = (int)(arg_y0 + 0.5);
	tu0 = (int)(arg_tu0);
	tv0 = (int)(arg_tv0);

	x1 = (int)(arg_x1 + 0.5);
	y1 = (int)(arg_y1 + 0.5);
	tu1 = (int)(arg_tu1);
	tv1 = (int)(arg_tv1);

	x2 = (int)(arg_x2 + 0.5);
	y2 = (int)(arg_y2 + 0.5);
	tu2 = (int)(arg_tu2);
	tv2 = (int)(arg_tv2);

	yrestart = y1;

	if (tri_type&TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = y2 - y0;

			dxdyL = (x2 - x0) / dy;
			dudyL = (tu2 - tu0) / dy;
			dvdyL = (tv2 - tv0) / dy;

			dxdyR = (x2 - x1) / dy;
			dudyR = (tu2 - tu1) / dy;
			dvdyR = (tv2 - tv1) / dy;

			if (y0 < min_clip_y)
			{
				dy = min_clip_y - y0;

				xL = dxdyL*dy + x0;
				uL = dudyL*dy + tu0;
				vL = dvdyL*dy + tv0;

				xR = dxdyR*dy + x1;
				uR = dudyR*dy + tu1;
				vR = dvdyR*dy + tv1;
				
				ystart = min_clip_y;
			}
			else
			{
				xL = x0;
				xR = x1;

				uL = tu0;
				vL = tv0;

				uR = tu1;
				vR = tv1;
				
				ystart = y0;
			}
		}
		else
		{
			dy = y1 - y0;

			dxdyL = (x1 - x0) / dy;
			dudyL = (tu1 - tu0) / dy;
			dvdyL = (tv1 - tv0) / dy;

			dxdyR = (x2 - x0) / dy;
			dudyR = (tu2 - tu0) / dy;
			dvdyR = (tv2 - tv0) / dy;

			if (y0 < min_clip_y)
			{
				dy = min_clip_y - y0;

				xL = dxdyL*dy + x0;
				uL = dudyL*dy + tu0;
				vL = dvdyL*dy + tv0;

				xR = dxdyR*dy + x0;
				uR = dudyR*dy + tu0;
				vR = dvdyR*dy + tv0;

				ystart = min_clip_y;
			}
			else
			{
				xL = x0;
				xR = x0;

				uL = tu0;
				vL = tv0;

				uR = tu0;
				vR = tv0;

				ystart = y0;
			}
		}

		if ((yend = y2) > max_clip_y)
		{
			yend = max_clip_y;
		}

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = xL;
				xend = xR;

				ui = uL;
				vi = vL;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (uR - uL) / dx;
					dv = (vR - vL) / dx;
				}
				else
				{
					du = uR - uL;
					dv = vR - vL;
				}

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui += dx*du;
					vi += dx*dv;

					xstart = min_clip_x;
				}

				if (xend > max_clip_x)
				{
					xend = max_clip_x;
				}

				for (xi = xstart; xi < xend; xi++)
				{
					// write textel
					SetPixel(xi, yi, textmap->bitmapData->buffer[ui + (vi << texture_shift2)]);

					ui += du;
					vi += dv;
				}

				xL += dxdyL;
				uL += dudyL;
				vL += dvdyL;

				xR += dxdyR;
				uR += dudyR;
				vR += dvdyR;
			}
		 }
		else
		{
			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = xL;
				xend = xR;

				ui = uL;
				vi = vL;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (uR - uL) / dx;
					dv = (vR - vL) / dx;
				}
				else
				{
					du = uR - uL;
					dv = vR - vL;
				}

				for (xi = xstart; xi <= xend; xi++)
				{
					// write textel
					SetPixel(xi, yi, textmap->bitmapData->buffer[ui + (vi << texture_shift2)]);

					ui += du;
					vi += dv;
				}

				xL += dxdyL;
				uL += dudyL;
				vL += dvdyL;

				xR += dxdyR;
				uR += dudyR;
				vR += dvdyR;
			}
		}
	}
	else if (tri_type == TRI_TYPE_GENERAL)
	{
		if ((yend = y2) > max_clip_y)
		{
			yend = max_clip_y;
		}

		if (y1 < min_clip_y)
		{
			dyL = y2 - y1;

			dxdyL = (x2 - x1) / dyL;
			dudyL = (tu2 - tu1) / dyL;
			dvdyL = (tv2 - tv1) / dyL;

			dyR = y2 - y0;
			dxdyR = (x2 - x0) / dyR;
			dudyR = (tu2 - tu0) / dyR;
			dvdyR = (tv2 - tv0) / dyR;

			dyR = min_clip_y - y0;
			dyL = min_clip_y - y1;

			xL = dxdyL*dyL + x1;
			uL = dudyL*dyL + tu1;
			vL = dvdyL*dyL + tv1;

			xR = dxdyR*dyR + x0;
			uR = dudyR*dyR + tu0;
			vR = dvdyR*dyR + tv0;

			ystart = min_clip_y;

			if (dxdyR > dxdyL)
			{
				SWAP(dxdyL, dxdyR, temp);
				SWAP(dudyL, dudyR, temp);
				SWAP(dvdyL, dvdyR, temp);
				SWAP(xL, xR, temp);
				SWAP(uL, uR, temp);
				SWAP(vL, vR, temp);
				SWAP(x1, x2, temp);
				SWAP(y1, y2, temp);
				SWAP(tu1, tu2, temp);
				SWAP(tv1, tv2, temp);

				irestart = INTERP_RHS;
			}
		}
		else if (y0< min_clip_y)
		{
			dyL = y1 - y0;

			dxdyL = (x1 - x0) / dyL;
			dudyL = (tu1 - tu0) / dyL;
			dvdyL = (tv1 - tv0) / dyL;

			dyR = y2 - y0;

			dxdyR = (x2 - x0) / dyR;
			dudyR = (tu2 - tu0) / dyR;
			dvdyR = (tv2 - tv0) / dyR;

			dy = min_clip_y - y0;

			xL = dxdyL*dy + x0;
			uL = dxdyL*dy + tu0;
			vL = dvdyL*dy + tv0;

			xR = dxdyR*dy + x0;
			uR = dudyR*dy + tu0;
			vR = dvdyR*dy + tv0;

			ystart = min_clip_y;

			if (dxdyR < dxdyL)
			{
				SWAP(dxdyL, dxdyR, temp);
				SWAP(dudyL, dudyR, temp);
				SWAP(dvdyL, dvdyR, temp);
				SWAP(xL, xR, temp);
				SWAP(uL, uR, temp);
				SWAP(vL, vR, temp);
				SWAP(x1, x2, temp);
				SWAP(y1, y2, temp);
				SWAP(tu1, tu2, temp);
				SWAP(tv1, tv2, temp);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyL = y1 - y0;

			dxdyL = (x1 - x0) / dyL;
			dudyL = (tu1 - tu0) / dyL;
			dvdyL = (tv1 - tv0) / dyL;

			dyR = y2 - y0;

			dxdyR = (x2 - x0) / dyR;
			dudyR = (tu2 - tu0) / dyR;
			dvdyR = (tv2 - tv0) / dyR;

			xL = x0;
			xR = x0;

			uL = tu0;
			vL = tv0;

			uR = tu0;
			vR = tv0;

			ystart = y0;

			if (dxdyR < dxdyL)
			{
				SWAP(dxdyL, dxdyR, temp);
				SWAP(dudyL, dudyR, temp);
				SWAP(dvdyL, dvdyR, temp);
				SWAP(xL, xR, temp);
				SWAP(uL, uR, temp);
				SWAP(vL, vR, temp);
				SWAP(x1, x2, temp);

				irestart = INTERP_RHS;
			}
		}

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = xL;
				xend = xR;

				ui = uL;
				vi = vL;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (uR - uL) / dx;
					dv = (vR - vL) / dx;
				}
				else
				{
					du = uR - uL;
					dv = vR - vL;
				}

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui += dx*du;
					vi += dx*dv;

					xstart = min_clip_x;
				}

				if (xend > max_clip_x)
				{
					xend = max_clip_x;
				}

				for (xi = xstart; xi <= xend; xi++)
				{
					// write textel
					SetPixel(xi, yi, textmap->bitmapData->buffer[ui + (vi << texture_shift2)]);

					ui += du;
					vi += dv;
				}

				xL += dxdyL;
				uL += dudyL;
				vL += dvdyL;

				xR += dxdyR;
				uR += dudyR;
				vR += dvdyR;

				if (yi = yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyL = y2 - y1;
						
						dxdyL = (x2 - x1) / dyL;
						dudyL = (tu2 - tu1) / dyL;
						dvdyL = (tv2 - tv1) / dyL;

						xL = x1;
						uL = tu1;
						vL = tv1;

						xL += dxdyL;
						uL += dudyL;
						vL += dvdyL;
					}
					else
					{
						dyR = y1 - y2;

						dxdyR = (x1 - x2) / dyR;
						dudyR = (tu1 - tu2) / dyR;
						dvdyR = (tv1 - tv2) / dyR;

						xR = x2;
						uR = tu2;
						vR = tv2;

						xR += dxdyR;
						uR += dudyR;
						vR += dvdyR;
					}
				}
			}
		}
		else
		{
			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = xL;
				xend = xR;

				ui = uL;
				vi = vL;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (uR - uL) / dx;
					dv = (vR - vL) / dx;
				}
				else
				{
					du = uR - uL;
					dv = vR - vL;
				}

				for (xi = xstart; xi <= xend; xi++)
				{
					// write textel
					SetPixel(xi, yi, textmap->bitmapData->buffer[ui + (vi << texture_shift2)]);

					ui += du;
					vi += dv;
				}
				xL += dxdyL;
				uL += dudyL;
				vL += dvdyL;

				xR += dxdyR;
				uR += dudyR;
				vR += dvdyR;

				if (yi == yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyL = y2 - y1;
						dxdyL = (x2 - x1) / dyL;
						dudyL = (tu2 - tu1) / dyL;
						dvdyL = (tv2 - tv1) / dyL;

						xL = x1;
						uL = tu1;
						vL = tv1;
						
						xL += dxdyL;
						uL += dudyL;
						vL += dvdyL;
					}
					else
					{
						dyR = y1 - y2;
						dxdyR = (x1 - x2) / dyR;
						dudyR = (tu1 - tu2) / dyR;
						dvdyR = (tv1 - tv2) / dyR;

						xR = x2;
						uR = tu2;
						vR = tv2;

						xR += dxdyR;
						uR += dudyR;
						vR += dvdyR;
					}
				}
			}
		}
	}
}

void kxDrawer::DrawBackground()
{
	float  h = WIN_HEIGHT;
	float w = WIN_WIDTH;
	float halfH = h *0.5;
	float halfW = w*0.5;
	//sky
	DrawTriangle(0, halfH, 0, 0, w, halfH, 0x4876FF);
	DrawTriangle(0, 0, w, 0, w, halfH, 0x4876FF);
	//ground
	DrawTriangle(0, halfH, 0, h, w, h, 0xD4D4D4);
	DrawTriangle(0, halfH, w, halfH, w, h, 0xD4D4D4);
}

void kxDrawer::Render(const kxRenderList & renderList)
{
	Clear(0);
	DrawBackground();
	kxPolygonList* face;
	for (int poly = 0; poly < renderList.num_polys; poly++)
	{
		if (!(renderList.poly_ptrs[poly]->state&POLY4D_STATE_ACTIVE) ||
			(renderList.poly_ptrs[poly]->state&POLY4D_STATE_CLIPPED) ||
			(renderList.poly_ptrs[poly]->state&POLY4D_STATE_BACKFACE))
		{
			continue;
		}
		if (renderList.poly_ptrs[poly]->attr&POLY4D_ATTR_SHADE_MODE_TEXTURE)
		{
			if (renderList.poly_ptrs[poly]->attr&POLY4D_ATTR_SHADE_MODE_CONSTANT)
			{
				DrawTextureTriangle(renderList.poly_ptrs[poly]->tlist[0]->position.x,
					renderList.poly_ptrs[poly]->tlist[0]->position.y,
					renderList.poly_ptrs[poly]->tlist[0]->textureUV.x,
					renderList.poly_ptrs[poly]->tlist[0]->textureUV.y,
					renderList.poly_ptrs[poly]->tlist[1]->position.x,
					renderList.poly_ptrs[poly]->tlist[1]->position.y,
					renderList.poly_ptrs[poly]->tlist[1]->textureUV.x,
					renderList.poly_ptrs[poly]->tlist[1]->textureUV.y,
					renderList.poly_ptrs[poly]->tlist[2]->position.x,
					renderList.poly_ptrs[poly]->tlist[2]->position.y,
					renderList.poly_ptrs[poly]->tlist[2]->textureUV.x,
					renderList.poly_ptrs[poly]->tlist[2]->textureUV.y,
					renderList.poly_ptrs[poly]->texture);
			}
			else
			{
				DrawTextureTriangleFS(renderList.poly_ptrs[poly]->tlist[0]->position.x,
					renderList.poly_ptrs[poly]->tlist[0]->position.y,
					renderList.poly_ptrs[poly]->tlist[0]->textureUV.x,
					renderList.poly_ptrs[poly]->tlist[0]->textureUV.y,
					renderList.poly_ptrs[poly]->tlist[1]->position.x,
					renderList.poly_ptrs[poly]->tlist[1]->position.y,
					renderList.poly_ptrs[poly]->tlist[1]->textureUV.x,
					renderList.poly_ptrs[poly]->tlist[1]->textureUV.y,
					renderList.poly_ptrs[poly]->tlist[2]->position.x,
					renderList.poly_ptrs[poly]->tlist[2]->position.y,
					renderList.poly_ptrs[poly]->tlist[2]->textureUV.x,
					renderList.poly_ptrs[poly]->tlist[2]->textureUV.y,
					renderList.poly_ptrs[poly]->lit_color[0],
					renderList.poly_ptrs[poly]->texture);
			}
		}
		else if((renderList.poly_ptrs[poly]->attr&POLY4D_ATTR_SHADE_MODE_FLAT)||
			(renderList.poly_ptrs[poly]->attr&POLY4D_ATTR_SHADE_MODE_CONSTANT))
		{
			DrawTriangle(renderList.poly_ptrs[poly]->tlist[0]->position.x, renderList.poly_ptrs[poly]->tlist[0]->position.y,
				renderList.poly_ptrs[poly]->tlist[1]->position.x, renderList.poly_ptrs[poly]->tlist[1]->position.y,
				renderList.poly_ptrs[poly]->tlist[2]->position.x, renderList.poly_ptrs[poly]->tlist[2]->position.y, renderList.poly_ptrs[poly]->lit_color[0]->getRGBA());
		}
		else if (renderList.poly_ptrs[poly]->attr&POLY4D_ATTR_SHADE_MODE_GOURAUD)
		{
			DrawGouraudTriangle(renderList.poly_ptrs[poly]->tlist[0]->position.x, renderList.poly_ptrs[poly]->tlist[0]->position.y,renderList.poly_ptrs[poly]->lit_color[0],
				renderList.poly_ptrs[poly]->tlist[1]->position.x, renderList.poly_ptrs[poly]->tlist[1]->position.y, renderList.poly_ptrs[poly]->lit_color[1],
				renderList.poly_ptrs[poly]->tlist[2]->position.x, renderList.poly_ptrs[poly]->tlist[2]->position.y, renderList.poly_ptrs[poly]->lit_color[2]);
		}
	}
	Update();
}

void kxDrawer::Update()
{
	HDC hDC = GetDC(this->hwnd);
	BitBlt(hDC, 0, 0, width, height, this->screenHDC, 0, 0, SRCCOPY);
	ReleaseDC(this->hwnd, hDC);
}
