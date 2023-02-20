// Copyright (c) 2015 YamaArashi

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "global.h"
#include "font.h"
#include "gfx.h"
#include "util.h"

unsigned char gFontPalette[][3] = {
	{0x90, 0xC8, 0xFF}, // bg (saturated blue that contrasts well with the shadow color)
	{0x38, 0x38, 0x38}, // fg (dark grey)
	{0xD8, 0xD8, 0xD8}, // shadow (light grey)
	{0xFF, 0xFF, 0xFF}  // box (white)
};

static void ConvertFromLatinFont(unsigned char *src, unsigned char *dest, unsigned int numRows)
{
	unsigned int srcPixelsOffset = 0;

	for (unsigned int row = 0; row < numRows; row++) {
		for (unsigned int column = 0; column < 16; column++) {
			for (unsigned int glyphTile = 0; glyphTile < 4; glyphTile++) {
				unsigned int pixelsX = (column * 16) + ((glyphTile & 1) * 8);

				for (unsigned int i = 0; i < 8; i++) {
					unsigned int pixelsY = (row * 16) + ((glyphTile >> 1) * 8) + i;
					unsigned int destPixelsOffset = (pixelsY * 64) + (pixelsX / 4);

					dest[destPixelsOffset] = src[srcPixelsOffset + 1];
					dest[destPixelsOffset + 1] = src[srcPixelsOffset];

					srcPixelsOffset += 2;
				}
			}
		}
	}
}

static void ConvertToLatinFont(unsigned char *src, unsigned char *dest, unsigned int numRows)
{
	unsigned int destPixelsOffset = 0;

	for (unsigned int row = 0; row < numRows; row++) {
		for (unsigned int column = 0; column < 16; column++) {
			for (unsigned int glyphTile = 0; glyphTile < 4; glyphTile++) {
				unsigned int pixelsX = (column * 16) + ((glyphTile & 1) * 8);

				for (unsigned int i = 0; i < 8; i++) {
					unsigned int pixelsY = (row * 16) + ((glyphTile >> 1) * 8) + i;
					unsigned int srcPixelsOffset = (pixelsY * 64) + (pixelsX / 4);

					dest[destPixelsOffset] = src[srcPixelsOffset + 1];
					dest[destPixelsOffset + 1] = src[srcPixelsOffset];

					destPixelsOffset += 2;
				}
			}
		}
	}
}

static void SetFontPalette(struct Image *image)
{
	image->hasPalette = true;

	image->palette.numColors = 4;

	for (int i = 0; i < image->palette.numColors; i++) {
		image->palette.colors[i].red = gFontPalette[i][0];
		image->palette.colors[i].green = gFontPalette[i][1];
		image->palette.colors[i].blue = gFontPalette[i][2];
	}

	image->hasTransparency = false;
}

void ReadLatinFont(char *path, struct Image *image)
{
	int fileSize;
	unsigned char *buffer = ReadWholeFile(path, &fileSize);

	int numGlyphs = fileSize / 64;

	if (numGlyphs % 16 != 0)
		FATAL_ERROR("The number of glyphs (%d) is not a multiple of 16.\n", numGlyphs);

	int numRows = numGlyphs / 16;

	image->width = 256;
	image->height = numRows * 16;
	image->bitDepth = 2;
	image->pixels = malloc(fileSize);

	if (image->pixels == NULL)
		FATAL_ERROR("Failed to allocate memory for font.\n");

	ConvertFromLatinFont(buffer, image->pixels, numRows);

	free(buffer);

	SetFontPalette(image);
}

void WriteLatinFont(char *path, struct Image *image)
{
	if (image->width != 256)
		FATAL_ERROR("The width of the font image (%d) is not 256.\n", image->width);

	if (image->height % 16 != 0)
		FATAL_ERROR("The height of the font image (%d) is not a multiple of 16.\n", image->height);

	int numRows = image->height / 16;
	int bufferSize = numRows * 16 * 64;
	unsigned char *buffer = malloc(bufferSize);

	if (buffer == NULL)
		FATAL_ERROR("Failed to allocate memory for font.\n");

	ConvertToLatinFont(image->pixels, buffer, numRows);

	WriteWholeFile(path, buffer, bufferSize);

	free(buffer);
}
