#include "kxBitmap.h"
#include <WinBase.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include "kxColor.h"

USING_KX;

int kxBitmap::load(char * filename)
{
	int fileHandle;
	int index;

	UCHAR* tmpBuffer = nullptr;
	OFSTRUCT fileData;

	if (fileHandle = OpenFile(filename, &fileData, OF_READ) == -1)
	{
		return 0;
	 }

	_lread(fileHandle, &bitmapData->bitmapFileHeader, sizeof(BITMAPFILEHEADER));

	if (bitmapData->bitmapFileHeader.bfType != BITMAP_ID)
	{
		_lclose(fileHandle);
		return 0;
	}

	_lread(fileHandle, &bitmapData->bitmapInfoHeadr, sizeof(BITMAPINFOHEADER));

	if (bitmapData->bitmapInfoHeadr.biBitCount == 8)
	{
		_lread(fileHandle, &bitmapData->palette, MAX_COLORS_PALETTE * sizeof(PALETTEENTRY));

		for (index = 0; index < MAX_COLORS_PALETTE; index++)
		{
			int tmpColor = bitmapData->palette[index].peRed;
			bitmapData->palette[index].peRed = bitmapData->palette[index].peBlue;
			bitmapData->palette[index].peBlue = tmpColor;

			bitmapData->palette[index].peFlags = PC_NOCOLLAPSE;
		}
	}

	_lseek(fileHandle, -(int)(bitmapData->bitmapInfoHeadr.biSizeImage), SEEK_END);

	if (bitmapData->bitmapInfoHeadr.biBitCount == 8 || bitmapData->bitmapInfoHeadr.biBitCount == 16)
	{
		if (bitmapData->buffer)
		{
			free(bitmapData->buffer);
		}
		if (!(bitmapData->buffer = (UCHAR*)malloc(bitmapData->bitmapInfoHeadr.biSizeImage)))
		{
			_lclose(fileHandle);
			return 0;
		}
		_lread(fileHandle, bitmapData->buffer, bitmapData->bitmapInfoHeadr.biSizeImage);
	}
	else if (bitmapData->bitmapInfoHeadr.biBitCount == 24)
		{
			if (!(tmpBuffer = (UCHAR*)malloc(bitmapData->bitmapInfoHeadr.biSizeImage)))
			{
				_lclose(fileHandle);
				return 0;
			}
			if (!(bitmapData->buffer = (UCHAR*)malloc(2 * bitmapData->bitmapInfoHeadr.biWidth*bitmapData->bitmapInfoHeadr.biHeight)))
			{
				_lclose(fileHandle);
				free(tmpBuffer);
				return 0;
		   }

			_lread(fileHandle, tmpBuffer, bitmapData->bitmapInfoHeadr.biSizeImage);

			for (index = 0; index < bitmapData->bitmapInfoHeadr.biWidth*bitmapData->bitmapInfoHeadr.biHeight; index++)
			{
				kxColor* color = new kxColor();
				color->setBlue(tmpBuffer[index * 3 + 0] >> 3);
				color->setGreen(tmpBuffer[index * 3 + 1] >> 3);
				color->setRed(tmpBuffer[index * 3 + 2] >> 3);
				((USHORT*)bitmapData->buffer)[index] = (USHORT)color->getRGB();
			}
			bitmapData->bitmapInfoHeadr.biBitCount = 16;
			free(tmpBuffer);
		}
	else
	{
		return 0;
	}

	_lclose(fileHandle);
	flip();
	return 1;
}

int kxBitmap::unload()
{
	if (bitmapData->buffer)
	{
		free(bitmapData->buffer);
		bitmapData->buffer = nullptr;
	}
	return 1;
}

int kxBitmap::flip()
{
	UCHAR* buffer;
	int index;

	int bytes_per_line = (bitmapData->bitmapInfoHeadr.biBitCount / 8)*bitmapData->bitmapInfoHeadr.biWidth;
	int size = bytes_per_line*bitmapData->bitmapInfoHeadr.biHeight;
	if (!(buffer = (UCHAR*)malloc(size)))
	{
		return 0;
	}
	memcpy_s(buffer, size, bitmapData->buffer, size);
	for (index = 0; index < height; index++)
	{
		memcpy_s(&bitmapData->buffer[((height - 1) - index)*bytes_per_line], bytes_per_line, &buffer[index*bytes_per_line], bytes_per_line);
	}
	free(buffer);
	return 1;
}
