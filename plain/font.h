/*
 * JFBTERM - 
 * Copyright (c) 2003 Fumitoshi UKAI <ukai@debian.or.jp>
 * Copyright (c) 1999 Noritoshi Masuichi (nmasu@ma3.justnet.ne.jp)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in the
 *	documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NORITOSHI MASUICHI ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL NORITOSHI MASUICHI BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 */

#ifndef INCLUDE_FONT_H
#define INCLUDE_FONT_H

#include <stdint.h>
#include <sys/types.h>

#include "getcap.h"

#define TAPP_ERR_FLDD	1	/* 指定のフォントセットは設定済み */

#define TFONT_FT_SINGLE		0x00000000	/* 1 byte character set */
#define TFONT_FT_DOUBLE		0x01000000	/* 2 byte character set */
#define TFONT_FT_94CHAR		0x00000000	/* 94 or 94^n */
#define TFONT_FT_96CHAR		0x02000000	/* 96 or 96^n */
#define TFONT_FT_OTHER		0x10000000	/* other coding system */

#define TFONT_OWNER		0x00	/* この構造体がglyphを支配 */
#define TFONT_ALIAS		0x01	/* 別の構造体のglyphを参照 */

typedef enum
{
	FH_LEFT,
	FH_RIGHT,
	FH_UNI,
} FONTSET_HALF;

//
//
//
typedef struct Raw_TFontGlyphWidth
{
	int8_t cols;			// glyph width in cols. (0, 1, 2)
	uint16_t pixels;		// width in pixels.
} TFontGlyphWidth;

//
//
//
typedef struct Raw_TFont
{
	const uint8_t *(*conv) (struct Raw_TFont * p, uint32_t c, TFontGlyphWidth * width);
	/* --- */
	const char *name;
	uint32_t width;		/* 一文字あたりの水平ドット数 */
	uint32_t height;	/* 一文字あたりの垂直ドット数 */
	/* */
	uint32_t fsignature;
	FONTSET_HALF fhalf;
	uint8_t aliasF;
	/* */
	uint8_t **glyph;	/* ビットマップ中の各glyph の先頭 */
	TFontGlyphWidth *glyph_width;	//
	uint8_t *dglyph;	/* 対応するglyphが存在しない時のglyph */
	uint8_t *bitmap;	/* ビットマップ */
	uint32_t colf;
	uint32_t coll;
	uint32_t rowf;
	uint32_t rowl;
	uint32_t colspan;	/* = coll-colf+1; */
	uint32_t bytew;		/* 一文字の水平１ラインのバイト数 */
	uint32_t bytec;		/* 一文字あたりのバイト数 */
} TFont;

const uint8_t *tfont_default_glyph(TFont * p, uint32_t c, TFontGlyphWidth * width);
const uint8_t *tfont_standard_glyph(TFont * p, uint32_t c, TFontGlyphWidth * width);
void tfont_final(TFont * p);
void tfont_ary_final(void);
void tfont_init(TFont * p);

void tfont_setup_fontlist(TCapValue * values);
int tfont_is_valid(TFont * p);
int tfont_is_loaded(TFont * p);

int tfont_ary_search_idx(const char *na);
void tfont_ary_show_list(FILE * fp);

extern TFont gFont[];
extern uint32_t gFontsWidth;
extern uint32_t gFontsHeight;

#endif /* INCLUDE_FONT_H */
