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

void kxDrawer::DrawGouraudTriangle(kxPolygonList* face, UCHAR * dest_buffer, int mem_pitch)
{
	int v0 = 0,
		v1 = 1,
		v2 = 2,
		temp = 0,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx, dy, dy1, dyr,
		u, v, w,
		du, dv, dw,
		xi, yi,
		ui, vi, wi,
		index_x, index_y,
		x, y,
		xstart, xend,
		ystart, yrestart, yend,
		x1,
		dxdy1,
		xr,
		dxdyr,
		dudy1,
		u1,
		dvdy1,
		v1,
		dwdy1,
		w1,
		dudyr,
		ur,
		dvdyr,
		vr,
		dwdyr,
		wr;
	int x0, y0, tu0, tv0, tw0,
		x1, y1, tu1, tv1, tw1,
		x2, y2, tu2, tv2, tw2;
	int r_base0, g_base0, b_base0,
		r_base1, g_base1, b_base1,
		r_base2, g_base2, b_base2;

	UCHAR *screen_ptr = nullptr,
		*screen_line = nullptr,
		*textmap = nullptr;

	if (((face->tlist[0]->position.y < min_clip_y) && (face->tlist[1]->position.y < min_clip_y) && (face->tlist[2]->position.y < min_clip_y)) ||
		((face->tlist[0]->position.y > max_clip_y) && (face->tlist[1]->position.y > max_clip_y) && (face->tlist[2]->position.y > max_clip_y)) ||
		((face->tlist[0]->position.x < min_clip_x) && (face->tlist[1]->position.x < min_clip_x) && (face->tlist[2]->position.x < min_clip_x)) ||
		((face->tlist[0]->position.x > max_clip_x) && (face->tlist[1]->position.x > max_clip_x) && (face->tlist[2]->position.x > max_clip_x)))
	{
		return;
	}

	if (((face->tlist[0]->position.x == face->tlist[1]->position.x) && (face->tlist[1]->position.x == face->tlist[2]->position.x)) ||
		((face->tlist[0]->position.y == face->tlist[1]->position.y) && (face->tlist[1]->position.y == face->tlist[2]->position.y)))
	{
		return;
	}

	if (face->tlist[v1]->position.y < face->tlist[v0]->position.y)
	{
		SWAP(v0, v1, temp);
	}

	if (face->tlist[v2]->position.y < face->tlist[v0]->position.y)
	{
		SWAP(v0, v2, temp);
	}

	if (face->tlist[v2]->position.y < face->tlist[v1]->position.y)
	{
		SWAP(v1, v2, temp);
	}

	if (face->tlist[v0]->position.y == face->tlist[v1]->position.y)
	{
		tri_type = TRI_TYPE_FLAT_TOP;
		if (face->tlist[v1]->position.x < face->tlist[v0]->position.x)
		{
			SWAP(v0, v1, temp);
		}
	}
	else if (face->tlist[v1]->position.y == face->tlist[v2]->position.y)
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;
		if (face->tlist[v2]->position.x < face->tlist[v1]->position.x)
		{
			SWAP(v1, v2, temp);
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	r_base0 = face->lit_color[v0].getRed();
	g_base0 = face->lit_color[v0].getGreen();
	b_base0 = face->lit_color[v0].getBlue();

	r_base1 = face->lit_color[v1].getRed();
	g_base1 = face->lit_color[v1].getGreen();
	b_base1 = face->lit_color[v1].getBlue();

	r_base2 = face->lit_color[v2].getRed();
	g_base2 = face->lit_color[v2].getGreen();
	g_base2 = face->lit_color[v2].getBlue();

	x0 = (int)(face->tlist[v0]->position.x + 0.5);
	y0 = (int)(face->tlist[v0]->position.y + 0.5);

	tu0 = r_base0;
	tv0 = g_base0;
	tw0 = b_base0;

	x1 = (int)(face->tlist[v1]->position.x + 0.5);
	y1 = (int)(face->tlist[v1]->position.y + 0.5);

	tu1 = r_base1;
	tv1 = g_base1;
	tw1 = b_base1;

	x2 = (int)(face->tlist[v2]->position.x + 0.5);
	y2 = (int)(face->tlist[v2]->position.y + 0.5);

	tu2 = r_base2;
	tv0 = g_base2;
	tw2 = b_base2;
    
	yrestart = y1;
	if (tri_type&TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = (y2 - y0);

			dxdy1 = (x2 - x0) / dy;
			dudy1 = (tu2 - tu0) / dy;
			dvdy1 = (tv2 - tv0) / dy;
			dwdy1 = (tw2 - tw0) / dy;

			dxdyr = (x2 - x1) / dy;
			dudyr = (tu2 - tu1) / dy;
			dvdyr = (tv2 - tv1) / dy;
			dwdyr = (tw2 - tw1) / dy;

			if (y0 < min_clip_y)
			{
				dy = min_clip_y - y0;

				x1 = dxdy1*dy + x0;
				u1 = dudy1*dy + tu0;
				v1 = dvdy1*dy + tv0;
				w1 = dwdy1*dy + tw0;

				xr = dxdyr*dy + x1;
				ur = dudyr*dy + tu1;
				vr = dvdyr*dy + tv1;
				wr = dwdyr*dy + tw1;

				ystart = min_clip_y;
			}
			else
			{
				x1 = x0;
				xr = x1;

				u1 = tu0;
				v1 = tv0;
				w1 = tw0;

				ur = tu1;
				vr = tv1;
				wr = tw1;
				ystart = y0;
			}
		}
		else
		{
			dy = y1 - y0;

			dxdy1 = (x1 - x0) / dy;
			dudy1 = (tu1 - tu0) / dy;
			dvdy1 = (tv1 - tv0) / dy;
			dwdy1 = (tw1 - tw0) / dy;

			dxdyr = (x2 - x0) / dy;
			dudyr = (tu2 - tu0) / dy;
			dvdyr = (tv2 - tv0) / dy;
			dwdyr = (tw2 - tw0) / dy;

			if (y0 < min_clip_y)
			{
				dy = min_clip_y - y0;

				x1 = dxdy1*dy + x0;
				u1 = dudy1*dy + tu0;
				v1 = dvdy1*dy + tv0;
				w1 = dwdy1*dy + tw0;

				xr = dxdyr*dy + x0;
				ur = dudyr*dy + tu0;
				vr = dvdyr*dy + tv0;
				wr = dwdyr*dy + tw0;

				ystart = min_clip_y;
			}
			else
			{
				x1 = x0;
				xr = x0;

				u1 = tu0;
				v1 = tv0;
				w1 = tw0;

				ur = tu0;
				vr = tv0;
				wr = tw0;

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
			//screen_ptr = dest_buffer + (ystart * mem_pitch);

			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = x1;
				xend = xr;

				ui = u1;
				vi = v1;
				wi = w1;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (ur - u1) / dx;
					dv = (vr - v1) / dx;
					dw = (wr - w1) / dx;
				}
				else
				{
					du = ur - u1;
					dv = vr - v1;
					dw = wr - w1;
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

				for (xi = xstart; xi <= xend; xi++)
				{
					// write textel assume 5.6.5
					//screen_ptr[xi] = ((ui >> (FIXP16_SHIFT + 3)) << 11) + ((vi >> (FIXP16_SHIFT + 2)) << 5) + (wi >> (FIXP16_SHIFT + 3));
					ui += du;
					vi += dv;
					wi += dw;
				}

				x1 += dxdy1;
				u1 += dudy1;
				v1 += dvdy1;
				w1 += dwdy1;

				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;

				// advance screen ptr
				//screen_ptr += mem_pitch;
			}
		}
		else
		{
			// point screen ptr to starting line
			//screen_ptr = dest_buffer + (ystart * mem_pitch);

			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = x1;
				xend = xr;

				ui = u1;
				vi = v1;
				wi = w1;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (ur - u1) / dx;
					dv = (vr - v1) / dx;
					dw = (wr - w1) / dx;
				}
				else
				{
					du = ur - u1;
					dv = vr - v1;
					dw = wr - w1;
				}

				for (xi = xstart; xi <= xend; xi++)
				{
					// write textel 5.6.5
					//screen_ptr[xi] = ((ui >> (FIXP16_SHIFT + 3)) << 11) + ((vi >> (FIXP16_SHIFT + 2)) << 5) + (wi >> (FIXP16_SHIFT + 3));

					ui += du;
					vi += dv;
					wi += dw;
				}

				x1 += dxdy1;
				u1 += dudy1;
				v1 += dvdy1;
				w1 += dwdy1;

				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;

				// advance screen ptr
				//screen_ptr += mem_pitch;
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
			dy1 = y2 - y1;

			dxdy1 = (x2 - x1) / dy1;
			dudy1 = (tu2 - tu1) / dy1;
			dvdy1 = (tv2 - tv1) / dy1;

			dyr = y2 - y0;
			
			dxdyr = (x2 - x0) / dyr;
			dudyr = (tu2 - tu0) / dyr;
			dvdyr = (tv2 - tv0) / dyr;

			dyr = min_clip_y - y0;
			dy1 = min_clip_y - y1;

			x1 = dxdy1*dy1 + x1;
			u1 = dudy1*dy1 + tu1;
			v1 = dvdy1*dy1 + tv1;

			xr = dxdyr*dyr + x0;
			ur = dudyr*dyr + tu0;
			vr = dvdyr*dyr + tv0;

			ystart = min_clip_y;

			if (dxdyr > dxdy1)
			{
				SWAP(dxdy1, dxdyr, temp);
				SWAP(dudy1, dudyr, temp);
				SWAP(dvdy1, dvdyr, temp);
				SWAP(x1, xr, temp);
				SWAP(u1, ur, temp);
				SWAP(v1, vr, temp);
				SWAP(x1, x2, temp);
				SWAP(y1, y2, temp);
				SWAP(tu1, tu2, temp);
				SWAP(tv1, tv2, temp);

				irestart = INTERP_RHS;
			}
		}
		else if (y0 < min_clip_y)
		{
			dy1 = y1 - y0;

			dxdy1 = (x1 - x0) / dy;
			dudy1 = (tu1 - tu0) / dy;
			dvdy1 = (tv1 - tv0) / dy;

			dyr = y2 - y0;

			dxdyr = (x2 - x0) / dyr;
			dudyr = (tu2 - tu0) / dyr;
			dvdyr = (tv2 - tv0) / dyr;

			dy = (min_clip_y - y0);

			x1 = dxdy1*dy + x0;
			u1 = dudy1*dy+tu0;
			v1 = dvdy1*dy + tv0;

			xr = dxdyr*dy + x0;
			ur = dudyr*dy + tu0;
			vr = dvdyr*dy + tv0;

			ystart = min_clip_y;

			if (dxdyr < dxdy1)
			{
				SWAP(dxdy1, dxdyr, temp);
				SWAP(dudy1, dudyr, temp);
				SWAP(dvdy1, dvdyr, temp);
				SWAP(x1, xr, temp);
				SWAP(u1, ur, temp);
				SWAP(v1, vr, temp);
				SWAP(x1, x2, temp);
				SWAP(y1, y2, temp);
				SWAP(tu1, tu2, temp);
				SWAP(tv1, tv2, temp);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dy1 = y1 - y0;

			dxdy1 = (x1 - x0) / dy1;
			dudy1 = (tu1 - tu0) / dy1;
			dvdy1 = (tv1 - tv0) / dy1;

			dyr = y2 - y0;

			dxdyr = (x2 - x0) / dyr;
			dudyr = (tu2 - tu0) / dyr;
			dvdyr = (tv2 - tv0) / dyr;

			x1 = x0;
			xr = x0;

			u1 = tu0;
			v1 = tv0;

			ur = tu0;
			vr = tv0;
			
			ystart = y0;

			if (dxdyr < dxdy1)
			{
				SWAP(dxdy1, dxdyr, temp);
				SWAP(dudy1, dudyr, temp);
				SWAP(dvdy1, dvdyr, temp);
				SWAP(x1, xr, temp);
				SWAP(u1, ur, temp);
				SWAP(v1, vr, temp);
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
			// point screen ptr to starting line
			//screen_ptr = dest_buffer + (ystart * mem_pitch);

			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = x1;
				xend = xr;

				ui = u1;
				vi = v1;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (ur - u1) / dx;
					dv = (vr - v1) / dx;
				}
				else
				{
					du = ur - u1;
					dv = vr - v1;
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
					//screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

					ui += du;
					vi += dv;
				}

				x1 += dxdy1;
				u1 += dudy1;
				v1 += dvdy1;

				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;

				// advance screen ptr
				//screen_ptr += mem_pitch;

				if (yi == yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dy1 = y2 - y1;

						dxdy1 = (x2 - x1) / dy1;
						dudy1 = (tu2 - tu1) / dy1;
						dvdy1 = (tv2 - tv1) / dy1;

						x1 = x1;
						u1 = tu1;
						v1 = tv1;

						x1 += dxdy1;
						u1 += dudy1;
						v1 += dvdy1;
					}
					else
					{
						dyr = y1 - y2;

						dxdyr = (x1 - x2) / dyr;
						dudyr = (tu1 - tu2) / dyr;
						dvdyr = (tv1 - tv2) / dyr;

						xr = x2;
						ur = tu2;
						vr = tv2;

						xr += dxdyr;
						ur += dudyr;
						vr += dvdyr;
					}
				}
			}
		}
		else
		{
			// no x clipping
			// point screen ptr to starting line
			//screen_ptr = dest_buffer + (ystart * mem_pitch);

			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = x1;
				xend = xr;

				ui = u1;
				vi = v1;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (ur - u1) / dx;
					dv = (vr - v1) / dx;
				}
				else
				{
					du = ur - u1;
					dv = vr - v1;
				}

				for (xi = xstart; xi < xend; xi++)
				{
					// write textel
					//screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

					ui += du;
					vi += dv;
				}

				x1 += dxdy1;
				u1 += dudy1;
				v1 += dvdy1;

				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;

				// advance screen ptr
				//screen_ptr += mem_pitch;

				if (yi == yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dy1 = y2 - y1;

						dxdy1 = (x2 - x1) / dy1;
						dudy1 = (tu2 - tu1) / dy1;
						dvdy1 = (tv2 - tv1) / dy1;

						x1 = x1;
						u1 = tu1;
						v1 = tv1;

						x1 += dxdy1;
						u1 += dudy1;
						v1 += dvdy1;
					}
					else
					{
						dyr = y1 - y2;

						dxdyr = (x1 - x2) / dyr;
						dudyr = (tu1 - tu2) / dyr;
						dvdyr = (tv1 - tv2) / dyr;

						xr = x2;
						ur = tu2;
						vr = tv2;

						xr += dxdyr;
						ur += dudyr;
						vr += dvdyr;
					}
				}
			}
		}
	}
}

void kxDrawer::DrawTextureTriangle(kxPolygonList * face, UCHAR * dest_buffer, int men_pitch)
{
	int v0 = 0,
		v1 = 1,
		v2 = 2,
		temp = 0,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy, dy1, dyr,
		u, v,
		du, dv,
		xi, yi,
		ui, vi,
		index_x, index_y,
		x, y,
		xstart, xend,
		ystart, yrestart, yend,
		x1,
		dxdy1,
		xr,
		dxdyr,
		dudy1,
		u1,
		dvdy1,
		v1,
		dudyr,
		ur,
		dvdyr,
		vr;

	int x0, y0, tu0, tv0,
		x1, y1, tu1, tv1,
		x2, y2, tu2, tv2;

	USHORT *screen_ptr = nullptr,
		*screen_line = nullptr,
		*textmap = nullptr;
	//*dest_buffer = (USHORT*)_dest_buffer;

	if (((face->tlist[0]->position.y < min_clip_y) &&
		(face->tlist[0]->position.y < min_clip_y) &&
		(face->tlist[0]->position.y < min_clip_y)) ||

		((face->tlist[0]->position.y > max_clip_y) &&
		 (face->tlist[1]->position.y > max_clip_y) &&
		 (face->tlist[2]->position.y > max_clip_y))||

		((face->tlist[0]->position.x<min_clip_x)&&
		(face->tlist[1]->position.x<min_clip_x)&&
		(face->tlist[2]->position.x<min_clip_x))||

		((face->tlist[0]->position.x>max_clip_x)&&
		 (face->tlist[1]->position.x>max_clip_x)&&
		 (face->tlist[2]->position.x>max_clip_x)))
	{
		return;
	}

	if (((face->tlist[0]->position.x == face->tlist[1]->position.x) && (face->tlist[1]->position.x == face->tlist[2]->position.x)) ||
		((face->tlist[0]->position.y == face->tlist[1]->position.y) && (face->tlist[1]->position.y == face->tlist[2]->position.y)))
	{
		return;
	}

	if (face->tlist[v1]->position.y < face->tlist[v0]->position.y)
	{
		SWAP(v0, v1, temp);
	}

	if (face->tlist[v2]->position.y < face->tlist[v0]->position.y)
	{
		SWAP(v0, v2, temp);
	}

	if (face->tlist[v2]->position.y < face->tlist[v1]->position.y)
	{
		SWAP(v1, v2, temp);
	}

	if (face->tlist[v0]->position.y == face->tlist[v1]->position.y)
	{
		tri_type = TRI_TYPE_FLAT_TOP;
		if (face->tlist[v1]->position.x < face->tlist[v0]->position.x)
		{
			SWAP(v0, v1, temp);
		}
	}
	else if(face->tlist[v1]->position.y==face->tlist[v2]->position.y)
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;
		if (face->tlist[v2]->position.x < face->tlist[v1]->position.x)
		{
			SWAP(v1, v2, temp);
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	x0 = (int)(face->tlist[v0]->position.x + 0.5);
	y0 = (int)(face->tlist[v0]->position.y + 0.5);
	tu0 = (int)(face->tlist[v0]->textureUV.x);
	tv0 = (int)(face->tlist[v0]->textureUV.y);

	x1 = (int)(face->tlist[v1]->position.x + 0.5);
	y1 = (int)(face->tlist[v1]->position.y + 0.5);
	tu1 = (int)(face->tlist[v1]->textureUV.x);
	tv1 = (int)(face->tlist[v1]->textureUV.y);

	x2 = (int)(face->tlist[v2]->position.x + 0.5);
	y2 = (int)(face->tlist[v2]->position.y + 0.5);
	tu2 = (int)(face->tlist[v2]->textureUV.x);
	tv2 = (int)(face->tlist[v2]->textureUV.y);

	yrestart = y1;

	if (tri_type&TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = y2 - y0;

			dxdy1 = (x2 - x0) / dy;
			dudy1 = (tu2 - tu0) / dy;
			dvdy1 = (tv2 - tv0) / dy;

			dxdyr = (x2 - x1) / dy;
			dudyr = (tu2 - tu1) / dy;
			dvdyr = (tv2 - tv1) / dy;

			if (y0 < min_clip_y)
			{
				dy = min_clip_y - y0;

				x1 = dxdy1*dy + x0;
				u1 = dudy1*dy + tu0;
				v1 = dvdy1*dy + tv0;

				xr = dxdyr*dy + x1;
				ur = dudyr*dy + tu1;
				vr = dvdyr*dy + tv1;

				ystart = min_clip_y;
			}
			else
			{
				x1 = x0;
				xr = x1;

				u1 = tu0;
				v1 = tv0;

				ur = tu1;
				vr = tv1;

				ystart = y0;
			}
		}
		else
		{
			dy = y1 - y0;

			dxdy1 = (x1 - x0) / dy;
			dudy1 = (tu1 - tu0) / dy;
			dvdy1 = (tv1 - tv0) / dy;

			dxdyr = (x2 - x0) / dy;
			dudyr = (tu2 - tu0) / dy;
			dvdyr = (tv2 - tv0) / dy;

			if (y0 < min_clip_y)
			{
				dy = min_clip_y - y0;

				x1 = dxdy1*dy + x0;
				u1 = dudy1*dy + tu0;
				v1 = dvdy1*dy + tv0;

				xr = dxdyr*dy + x0;
				ur = dudyr*dy + tu0;
				vr = dvdyr*dy + tv0;

				ystart = min_clip_y;
			}
			else
			{
				x1 = x0;
				xr = x0;

				u1 = tu0;
				v1 = tv0;

				ur = tu0;
				vr = tv0;

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
			// point screen ptr to starting line
			//screen_ptr = dest_buffer + (ystart * mem_pitch);

			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = x1;
				xend = xr;

				ui = u1;
				vi = v1;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (ur - u1) / dx;
					dv = (vr - v1) / dx;
				}
				else
				{
					du = ur - u1;
					dv = vr - v1;
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
					//screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

					ui += du;
					vi += dv;
				}

				x1 += dxdy1;
				u1 += dudy1;
				v1 += dvdy1;

				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;

				// advance screen ptr
				//screen_ptr += mem_pitch;
			}
		}
		else
		{
			// point screen ptr to starting line
			//screen_ptr = dest_buffer + (ystart * mem_pitch);

			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = x1;
				xend = xr;

				ui = u1;
				vi = v1;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (ur - u1) / dx;
					dv = (vr - v1) / dx;
				}
				else
				{
					du = ur - u1;
					dv = vr - v1;
				}

				for (xi = xstart; xi <= xend; xi++)
				{
					// write textel
					//screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

					ui += du;
					vi += dv;
				}

				x1 += dxdy1;
				u1 += dudy1;
				v1 += dvdy1;

				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;

				// advance screen ptr
				//screen_ptr += mem_pitch;
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
			dy1 = y2 - y1;

			dxdy1 = (x2 - x1) / dy1;
			dudy1 = (tu2 - tu1) / dy1;
			dvdy1 = (tv2 - tv1) / dy1;

			dyr = y2 - y0;
			dxdyr = (x2 - x0) / dyr;
			dudyr = (tu2 - tu0) / dyr;
			dvdyr = (tv2 - tv0) / dyr;

			dyr = min_clip_y - y0;
			dy1 = min_clip_y - y1;

			x1 = dxdy1*dy1 + x1;
			u1 = dxdy1*dy1 + tu1;
			v1 = dvdy1*dy1 + tv1;

			xr = dxdyr*dyr + x0;
			ur = dudyr*dyr + tu0;
			vr = dvdyr*dyr + tv0;

			ystart = min_clip_y;

			if (dxdyr > dxdy1)
			{
				SWAP(dxdy1, dxdyr, temp);
				SWAP(dudy1, dudyr, temp);
				SWAP(dvdy1, dvdyr, temp);
				SWAP(x1, xr, temp);
				SWAP(u1, ur, temp);
				SWAP(v1, vr, temp);
				SWAP(x1, x2, temp);
				SWAP(y1, y2, temp);
				SWAP(tu1, tu2, temp);
				SWAP(tv1, tv2, temp);

				irestart = INTERP_RHS;
			}
		}
		else if (y0 < min_clip_y)
		{
			dy1 = y1 - y0;

			dxdy1 = (x1 - x0) / dy1;
			dudy1 = (tu1 - tu0) / dy1;
			dvdy1 = (tv1 - tv0) / dy1;

			dyr = y2 - y0;

			dxdyr = (x2 - x0) / dyr;
			dudyr = (tu2 - tu0) / dyr;
			dvdyr = (tv2 - tv0) / dyr;

			dy = min_clip_y - y0;

			x1 = dxdy1*dy + x0;
			u1 = dudy1*dy + tu0;
			v1 = dvdy1*dy + tv0;

			xr = dxdyr*dy + x0;
			ur = dudyr*dy + tu0;
			vr = dvdyr*dy + tv0;

			ystart = min_clip_y;

			if (dxdyr < dxdy1)
			{
				SWAP(dxdy1, dxdyr, temp);
				SWAP(dudy1, dudyr, temp);
				SWAP(dvdy1, dvdyr, temp);
				SWAP(x1, xr, temp);
				SWAP(u1, ur, temp);
				SWAP(v1, vr, temp);
				SWAP(x1, x2, temp);
				SWAP(y1, y2, temp);
				SWAP(tu1, tu2, temp);
				SWAP(tv1, tv2, temp);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dy1 = y1 - y0;

			dxdy1 = (x1 - x0) / dy1;
			dudy1 = (tu1 - tu0) / dy1;
			dvdy1 = (tv1 - tv0) / dy1;

			dyr = y2 - y0;
			dxdyr = (x2 - x0) / dyr;
			dudyr = (tu2 - tu0) / dyr;
			dvdyr = (tv2 - tv0) / dyr;

			x1 = x0;
			xr = x0;

			u1 = tu0;
			v1 = tv0;

			ur = tu0;
			vr = tv0;

			ystart = y0;

			if (dxdyr < dxdy1)
			{
				SWAP(dxdy1, dxdyr, temp);
				SWAP(dudy1, dudyr, temp);
				SWAP(dvdy1, dvdyr, temp);
				SWAP(x1, xr, temp);
				SWAP(u1, ur, temp);
				SWAP(v1, vr, temp);
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
			// point screen ptr to starting line
			//screen_ptr = dest_buffer + (ystart * mem_pitch);

			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = x1;
				xend = xr;

				ui = u1;
				vi = v1;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (ur - u1) / dx;
					dv = (vr - v1) / dx;
				}
				else
				{
					du = ur - u1;
					dv = vr - v1;
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
					//screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

					ui += du;
					vi += dv;
				}

				x1 += dxdy1;
				u1 += dudy1;
				v1 += dvdy1;

				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;

				// advance screen ptr
				//screen_ptr += mem_pitch;

				if (yi == yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dy1 = y2 - y1;

						dxdy1 = (x2 - x1) / dy1;
						dudy1 = (tu2 - tu1) / dy1;
						dvdy1 = (tv2 - tv1) / dy1;

						x1 = x1;
						u1 = tu1;
						v1 = tv1;

						x1 += dxdy1;
						u1 += dudy1;
						v1 += dvdy1;
					}
					else
					{
						dyr = y1 - y2;

						dxdyr = (x1 - x2) / dyr;
						dudyr = (tu1 - tu2) / dyr;
						dvdyr = (tv1 - tv2) / dyr;

						xr = x2;
						ur = tu2;
						vr = tv2;

						xr += dxdyr;
						ur += dudyr;
						vr += dvdyr;
					}
				}
			}
		}
		else
		{
			// point screen ptr to starting line
			//screen_ptr = dest_buffer + (ystart * mem_pitch);
			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = x1;
				xend = xr;

				ui = u1;
				vi = v1;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (ur - u1) / dx;
					dv = (vr - v1) / dx;
				}
				else
				{
					du = ur - u1;
					dv = vr - v1;
				}

				for (xi = xstart; xi <= xend; xi++)
				{
					// write textel
					//screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

					ui += du;
					vi += dv;
				}

				x1 + dxdy1;
				u1 += dudy1;
				v1 += dvdy1;

				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;

				// advance screen ptr
				//screen_ptr += mem_pitch;

				if (yi == yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dy1 = y2 - y1;

						dxdy1 = (x2 - x1) / dy1;
						dudy1 = (tu2 - tu1) / dy1;
						dvdy1 = (tv2 - tv1) / dy1;

						x1 = x1;
						u1 = tu1;
						v1 = tv1;

						x1 += dxdy1;
						u1 += dudy1;
						v1 += dvdy1;
					}
					else
					{
						dyr = y1 - y2;

						dxdyr = (x1 - x2) / dyr;
						dudyr = (tu1 - tu2) / dyr;
						dvdyr = (tv1 - tv2) / dyr;

						xr = x2;
						ur = tu2;
						vr = tv2;

						xr += dxdyr;
						ur += dudyr;
						vr += dvdyr;
					}
				}
			}
		}
	}
}

void kxDrawer::DrawTextureTriangleFS(kxPolygonList * face, UCHAR * _dest_buffer, int men_pitch)
{
	int v0 = 0,
		v1 = 1,
		v2 = 2,
		temp = 0,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx, dy, dy1, dyr,
		u, v,
		du, dv,
		xi, yi,
		ui, vi,
		index_x, index_y,
		x, y,
		xstart, xend,
		ystart, yrestart, yend,
		x1,
		dxdy1,
		xr,
		dxdyr,
		dudy1,
		u1,
		dvdy1,
		v1,
		dudyr,
		ur,
		dvdyr,
		vr;

	USHORT r_base, g_base, b_base,
		r_textel, g_textel, b_textel, textel;

	int x0, y0, tu0, tv0,
		x1, y1, tu1, tv1,
		x2, y2, tu2, tv2;

	USHORT *screen_ptr = nullptr,
		*screen_line = nullptr,
		*textmap = nullptr,
		*dest_buffer = (USHORT*)_dest_buffer;

	textmap = (USHORT*)face->texture->bitmapData->buffer;

	if (((face->tlist[0]->position.y < min_clip_y) &&
		 (face->tlist[1]->position.y < min_clip_y) &&
		 (face->tlist[2]->position.y < min_clip_y)) ||

		((face->tlist[0]->position.y > max_clip_y) &&
		 (face->tlist[1]->position.y > max_clip_y)&&
		 (face->tlist[2]->position.y > max_clip_y))||

		((face->tlist[0]->position.x < min_clip_x)&&
		 (face->tlist[1]->position.x < min_clip_x)&&
		 (face->tlist[2]->position.x < min_clip_x))||

		((face->tlist[0]->position.x > max_clip_x)&&
		 (face->tlist[1]->position.x > max_clip_x)&&
		 (face->tlist[2]->position.x > max_clip_x)))
	{
		return;
	}

	if (((face->tlist[0]->position.x == face->tlist[1]->position.x) && (face->tlist[1]->position.x == face->tlist[2]->position.x)) ||
		((face->tlist[0]->position.y == face->tlist[1]->position.y) && (face->tlist[1]->position.y == face->tlist[2]->position.y)))
	{
		return;
	}

	if (face->tlist[v1]->position.y < face->tlist[v0]->position.y)
	{
		SWAP(v0, v1, temp);
	}

	if (face->tlist[v2]->position.y < face->tlist[v0]->position.y)
	{
		SWAP(v0, v2, temp);
	}

	if (face->tlist[v2]->position.y < face->tlist[v1]->position.y)
	{
		SWAP(v1, v2, temp);
	}

	if (face->tlist[v0]->position.y == face->tlist[v1]->position.y)
	{
		tri_type = TRI_TYPE_FLAT_TOP;
		if (face->tlist[v1]->position.x < face->tlist[v0]->position.x)
		{
			SWAP(v0, v1, temp);
		}
	}
	else if (face->tlist[v1]->position.y == face->tlist[v2]->position.y)
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;
		if (face->tlist[v2]->position.x == face->tlist[v1]->position.x)
		{
			SWAP(v1, v2, temp);
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	r_base = face->lit_color[0].getRed();
	g_base = face->lit_color[0].getGreen();
	b_base = face->lit_color[0].getBlue();

	x0 = (int)(face->tlist[v0]->position.x + 0.5);
	y0 = (int)(face->tlist[v0]->position.y + 0.5);
	tu0 = (int)(face->tlist[v0]->textureUV.x);
	tv0 = (int)(face->tlist[v0]->textureUV.y);

	x1 = (int)(face->tlist[v1]->position.x + 0.5);
	y1 = (int)(face->tlist[v1]->position.y + 0.5);
	tu1 = (int)(face->tlist[v1]->textureUV.x);
	tv1 = (int)(face->tlist[v1]->textureUV.y);

	x2 = (int)(face->tlist[v2]->position.x + 0.5);
	y2 = (int)(face->tlist[v2]->position.y + 0.5);
	tu2 = (int)(face->tlist[v2]->textureUV.x);
	tv2 = (int)(face->tlist[v2]->textureUV.y);

	yrestart = y1;

	if (tri_type&TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = y2 - y0;

			dxdy1 = (x2 - x0) / dy;
			dudy1 = (tu2 - tu0) / dy;
			dvdy1 = (tv2 - tv0) / dy;

			dxdyr = (x2 - x1) / dy;
			dudyr = (tu2 - tu1) / dy;
			dvdyr = (tv2 - tv1) / dy;

			if (y0 < min_clip_y)
			{
				dy = min_clip_y - y0;

				x1 = dxdy1*dy + x0;
				u1 = dudy1*dy + tu0;
				v1 = dvdy1*dy + tv0;

				xr = dxdyr*dy + x1;
				ur = dudyr*dy + tu1;
				vr = dvdyr*dy + tv1;
				
				ystart = min_clip_y;
			}
			else
			{
				x1 = x0;
				xr = x1;

				u1 = tu0;
				v1 = tv0;

				ur = tu1;
				vr = tv1;
				
				ystart = y0;
			}
		}
		else
		{
			dy = y1 - y0;

			dxdy1 = (x1 - x0) / dy;
			dudy1 = (tu1 - tu0) / dy;
			dvdy1 = (tv1 - tv0) / dy;

			dxdyr = (x2 - x0) / dy;
			dudyr = (tu2 - tu0) / dy;
			dvdyr = (tv2 - tv0) / dy;

			if (y0 < min_clip_y)
			{
				dy = min_clip_y - y0;

				x1 = dxdy1*dy + x0;
				u1 = dudy1*dy + tu0;
				v1 = dvdy1*dy + tv0;

				xr = dxdyr*dy + x0;
				ur = dudyr*dy + tu0;
				vr = dvdyr*dy + tv0;

				ystart = min_clip_y;
			}
			else
			{
				x1 = x0;
				xr = x0;

				u1 = tu0;
				v1 = tv0;

				ur = tu0;
				vr = tv0;

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
			// point screen ptr to starting line
			//screen_ptr = dest_buffer + (ystart * mem_pitch);

			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = x1;
				xend = xr;

				ui = u1;
				vi = v1;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (ur - u1) / dx;
					dv = (vr - v1) / dx;
				}
				else
				{
					du = ur - u1;
					dv = vr - v1;
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
					//screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

					ui += du;
					vi += dv;
				}

				x1 += dxdy1;
				u1 += dudy1;
				v1 += dvdy1;

				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;

				// advance screen ptr
				//screen_ptr += mem_pitch;
			}
		 }
		else
		{
			// point screen ptr to starting line
			//screen_ptr = dest_buffer + (ystart * mem_pitch);

			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = x1;
				xend = xr;

				ui = u1;
				vi = v1;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (ur - u1) / dx;
					dv = (vr - v1) / dx;
				}
				else
				{
					du = ur - u1;
					dv = vr - v1;
				}

				for (xi = xstart; xi <= xend; xi++)
				{
					// write textel
					//screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

					ui += du;
					vi += dv;
				}

				x1 += dxdy1;
				u1 += dudy1;
				v1 += dvdy1;

				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;

				// advance screen ptr
				//screen_ptr += mem_pitch;
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
			dy1 = y2 - y1;

			dxdy1 = (x2 - x1) / dy1;
			dudy1 = (tu2 - tu1) / dy1;
			dvdy1 = (tv2 - tv1) / dy1;

			dyr = y2 - y0;
			dxdyr = (x2 - x0) / dyr;
			dudyr = (tu2 - tu0) / dyr;
			dvdyr = (tv2 - tv0) / dyr;

			dyr = min_clip_y - y0;
			dy1 = min_clip_y - y1;

			x1 = dxdy1*dy1 + x1;
			u1 = dudy1*dy1 + tu1;
			v1 = dvdy1*dy1 + tv1;

			xr = dxdyr*dyr + x0;
			ur = dudyr*dyr + tu0;
			vr = dvdyr*dyr + tv0;

			ystart = min_clip_y;

			if (dxdyr > dxdy1)
			{
				SWAP(dxdy1, dxdyr, temp);
				SWAP(dudy1, dudyr, temp);
				SWAP(dvdy1, dvdyr, temp);
				SWAP(x1, xr, temp);
				SWAP(u1, ur, temp);
				SWAP(v1, vr, temp);
				SWAP(x1, x2, temp);
				SWAP(y1, y2, temp);
				SWAP(tu1, tu2, temp);
				SWAP(tv1, tv2, temp);

				irestart = INTERP_RHS;
			}
		}
		else if (y0< min_clip_y)
		{
			dy1 = y1 - y0;

			dxdy1 = (x1 - x0) / dy1;
			dudy1 = (tu1 - tu0) / dy1;
			dvdy1 = (tv1 - tv0) / dy1;

			dyr = y2 - y0;

			dxdyr = (x2 - x0) / dyr;
			dudyr = (tu2 - tu0) / dyr;
			dvdyr = (tv2 - tv0) / dyr;

			dy = min_clip_y - y0;

			x1 = dxdy1*dy + x0;
			u1 = dxdy1*dy + tu0;
			v1 = dvdy1*dy + tv0;

			xr = dxdyr*dy + x0;
			ur = dudyr*dy + tu0;
			vr = dvdyr*dy + tv0;

			ystart = min_clip_y;

			if (dxdyr < dxdy1)
			{
				SWAP(dxdy1, dxdyr, temp);
				SWAP(dudy1, dudyr, temp);
				SWAP(dvdy1, dvdyr, temp);
				SWAP(x1, xr, temp);
				SWAP(u1, ur, temp);
				SWAP(v1, vr, temp);
				SWAP(x1, x2, temp);
				SWAP(y1, y2, temp);
				SWAP(tu1, tu2, temp);
				SWAP(tv1, tv2, temp);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dy1 = y1 - y0;

			dxdy1 = (x1 - x0) / dy1;
			dudy1 = (tu1 - tu0) / dy1;
			dvdy1 = (tv1 - tv0) / dy1;

			dyr = y2 - y0;

			dxdyr = (x2 - x0) / dyr;
			dudyr = (tu2 - tu0) / dyr;
			dvdyr = (tv2 - tv0) / dyr;

			x1 = x0;
			xr = x0;

			u1 = tu0;
			v1 = tv0;

			ur = tu0;
			vr = tv0;

			ystart = y0;

			if (dxdyr < dxdy1)
			{
				SWAP(dxdy1, dxdyr, temp);
				SWAP(dudy1, dudyr, temp);
				SWAP(dvdy1, dvdyr, temp);
				SWAP(x1, xr, temp);
				SWAP(u1, ur, temp);
				SWAP(v1, vr, temp);
				SWAP(x1, x2, temp);

				irestart = INTERP_RHS;
			}
		}

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// point screen ptr to starting line
			//screen_ptr = dest_buffer + (ystart * mem_pitch);

			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = x1;
				xend = xr;

				ui = u1;
				vi = v1;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (ur - u1) / dx;
					dv = (vr - v1) / dx;
				}
				else
				{
					du = ur - u1;
					dv = vr - v1;
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
					//screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

					ui += du;
					vi += dv;
				}

				x1 += dxdy1;
				u1 += dudy1;
				v1 += dvdy1;

				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;

				// advance screen ptr
				//screen_ptr += mem_pitch;

				if (yi = yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dy1 = y2 - y1;
						
						dxdy1 = (x2 - x1) / dy1;
						dudy1 = (tu2 - tu1) / dy1;
						dvdy1 = (tv2 - tv1) / dy1;

						x1 = x1;
						u1 = tu1;
						v1 = tv1;

						x1 += dxdy1;
						u1 += dudy1;
						v1 += dvdy1;
					}
					else
					{
						dyr = y1 - y2;

						dxdyr = (x1 - x2) / dyr;
						dudyr = (tu1 - tu2) / dyr;
						dvdyr = (tv1 - tv2) / dyr;

						xr = x2;
						ur = tu2;
						vr = tv2;

						xr += dxdyr;
						ur += dudyr;
						vr += dvdyr;
					}
				}
			}
		}
		else
		{
			// point screen ptr to starting line
			//screen_ptr = dest_buffer + (ystart * mem_pitch);

			for (yi = ystart; yi <= yend; yi++)
			{
				xstart = x1;
				xend = xr;

				ui = u1;
				vi = v1;

				if ((dx = (xend - xstart)) > 0)
				{
					du = (ur - u1) / dx;
					dv = (vr - v1) / dx;
				}
				else
				{
					du = ur - u1;
					dv = vr - v1;
				}

				for (xi = xstart; xi <= xend; xi++)
				{
					// write textel
					//screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

					ui += du;
					vi += dv;
				}
				x1 += dxdy1;
				u1 += dudy1;
				v1 += dvdy1;

				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;

				// advance screen ptr
				//screen_ptr += mem_pitch;

				if (yi == yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dy1 = y2 - y1;
						dxdy1 = (x2 - x1) / dy1;
						dudy1 = (tu2 - tu1) / dy1;
						dvdy1 = (tv2 - tv1) / dy1;

						x1 = x1;
						u1 = tu1;
						v1 = tv1;
						
						x1 += dxdy1;
						u1 += dudy1;
						v1 += dvdy1;
					}
					else
					{
						dyr = y1 - y2;
						dxdyr = (x1 - x2) / dyr;
						dudyr = (tu1 - tu2) / dyr;
						dvdyr = (tv1 - tv2) / dyr;

						xr = x2;
						ur = tu2;
						vr = tv2;

						xr += dxdyr;
						ur += dudyr;
						vr += dvdyr;
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
	for (int poly = 0; poly < renderList.num_polys; poly++)
	{
		if (!(renderList.poly_ptrs[poly]->state&POLY4D_STATE_ACTIVE) ||
			(renderList.poly_ptrs[poly]->state&POLY4D_STATE_CLIPPED) ||
			(renderList.poly_ptrs[poly]->state&POLY4D_STATE_BACKFACE))
		{
			continue;
		}
		DrawTriangle(renderList.poly_ptrs[poly]->tlist[0]->position.x, renderList.poly_ptrs[poly]->tlist[0]->position.y,
			renderList.poly_ptrs[poly]->tlist[1]->position.x, renderList.poly_ptrs[poly]->tlist[1]->position.y,
			renderList.poly_ptrs[poly]->tlist[2]->position.x, renderList.poly_ptrs[poly]->tlist[2]->position.y, renderList.poly_ptrs[poly]->color->getRGB());
	}
	Update();
}

void kxDrawer::Update()
{
	HDC hDC = GetDC(this->hwnd);
	BitBlt(hDC, 0, 0, width, height, this->screenHDC, 0, 0, SRCCOPY);
	ReleaseDC(this->hwnd, hDC);
}
