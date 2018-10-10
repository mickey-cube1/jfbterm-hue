/*
 * JFBTERM -
 * Copyright (c) 2003  Fumitoshi UKAI <ukai@debian.or.jp>
 * Copyright (C) 1999  Noritoshi MASUICHI (nmasu@ma3.justnet.ne.jp)
 *
 * KON2 - Kanji ON Console -
 * Copyright (C) 1992-1996 Takashi MANABE (manabe@papilio.tutics.tut.ac.jp)
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
 * THIS SOFTWARE IS PROVIDED BY NORITOSHI MASUICH AND TAKASHI MANABE ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE TERRENCE R. LAMBERT BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 */

#ifndef INCLUDE_VTERM_H
#define INCLUDE_VTERM_H

#include <stdint.h>
#include <sys/types.h>

#include "config.h"
#include "mytypes.h"
#include "pen.h"
#include "font.h"

#ifdef JFB_OTHER_CODING_SYSTEM
#include <iconv.h>
#endif

#define	LEN_REPORT	32

typedef struct Raw_TFontSpec
{
	uint32_t invokedGn;	/* 呼び出さされている Gn : n = 0..3 */
	uint32_t idx;		/* 文字集合のgFont[]での位置 */
	uint32_t type;		/* 文字集合の区分 */
	FONTSET_HALF half;	/* 文字集合のG0,G1 のどちらを使っているか */
} TFontSpec;

#ifdef JFB_OTHER_CODING_SYSTEM
typedef struct Raw_TCodingSystem
{
	uint32_t fch;
	/* iconv state */
	char *fromcode;
	char *tocode;
	iconv_t cd;
#define MAX_MULTIBYTE_CHARLEN	6
	char inbuf[MAX_MULTIBYTE_CHARLEN];
	int inbuflen;
	char outbuf[MAX_MULTIBYTE_CHARLEN];

	/* saved state */
	uint32_t gSavedL;
	uint32_t gSavedR;
	uint32_t gSavedIdx[4];
	uint32_t utf8SavedIdx;
} TCodingSystem;

#endif

#define CUR_STYLE_000	1	//   0% (none)
#define CUR_STYLE_005	2	//   5% (underline)
#define CUR_STYLE_033	3	//  25% (1/3)
#define CUR_STYLE_050	4	//  50% (half)
#define CUR_STYLE_066	5	//  75% (2/3)
#define CUR_STYLE_100	6	// 100% (full)
#define CUR_STYLE_DEFAULT	CUR_STYLE_005

typedef struct Raw_TCursor
{
	uint32_t x;
	uint32_t y;
	TBool on;
	TBool shown;
	TBool wide;
	uint8_t style;
	uint32_t width;
	uint32_t height;
} TCursor;

typedef struct Raw_TVterm
{
	struct Raw_TTerm *term;
	int xmax;
	int ymax;
	int ymin;
	int xcap;		/* ハード的な1行あたり文字数 */
	int ycap;		/* ハード的な行数 */
	uint32_t tab;		/* TAB サイズ */

	TPen pen;
	TPen *savedPen;
	TPen *savedPenSL;	/* ステータスライン用 */
	int scroll;		/* スクロール行数 */
	/* -- */

	uint8_t knj1;		/* first byte of 2 byte code */
	FONTSET_HALF knj1h;
	uint32_t knj1idx;

	/* ISO-2022 対応 */
	uint32_t escSignature;
	uint32_t escGn;
	TFontSpec tgl;		/* 次の文字がGLのときに使う文字集合 */
	TFontSpec tgr;		/* 次の文字がGRのときに使う文字集合 */
	TFontSpec gl;		/* GL に呼び出されている文字集合 */
	TFontSpec gr;		/* GR に呼び出されている文字集合 */
	uint32_t gIdx[4];	/* Gn に指示されている文字集合のgFont[]での位置 */
	/* --- */
	uint32_t gDefaultL;
	uint32_t gDefaultR;
	uint32_t gDefaultIdx[4];
	/* --- */
	enum
	{
		SL_NONE,
		SL_ENTER,
		SL_LEAVE
	} sl;
#ifdef JFB_UTF8
	uint32_t utf8DefaultIdx;
	uint32_t utf8Idx;
	uint32_t utf8remain;
	uint32_t ucs2ch;
#endif
#ifdef JFB_OTHER_CODING_SYSTEM
	TCodingSystem *otherCS;
#endif
	TBool altCs;
	TCaps *caps;

	TBool soft;
	TBool wrap;
	TBool ins;		/* 挿入モード */
	TBool active;		/* このターミナルがアクティブ */
	TBool busy;		/* ビジー状態 */
	TBool sw;
	TBool release;
	TBool textClear;
	void (*esc) (struct Raw_TVterm * p, uint8_t ch);
	/* カーソル */
	TCursor cursor;

	/*  */
	struct winsize win;
	/* ESC Report Buffer */
	char report[LEN_REPORT];
	/* low level half */
	uint32_t textHead;
	uint32_t xcap4;		/* 4 bytes 境界に整合した桁数(xcap + 0 ... 3) */
	uint32_t tsize;		/* == xcap4 * ycap */
	/* */
	uint32_t *text;		/* 1 文字当たり 4 bytes */
	uint8_t *attr;
	uint8_t *flag;
} TVterm;

void tvterm_insert_n_chars(TVterm * p, int n);
void tvterm_delete_n_chars(TVterm * p, int n);
void tvterm_clear_n_chars(TVterm * p, uint32_t n);
void tvterm_text_scroll_down(TVterm * p, int line);
void tvterm_text_scroll_up(TVterm * p, int line);
void tvterm_text_move_down(TVterm * p, int top, int btm, int line);
void tvterm_text_move_up(TVterm * p, int top, int btm, int line);
void tvterm_text_clear_eol(TVterm * p, uint8_t mode);
void tvterm_text_clear_eos(TVterm * p, uint8_t mode);
void tvterm_wput(TVterm * p, uint32_t idx, uint8_t ch1, uint8_t ch2);
void tvterm_sput(TVterm * p, uint32_t idx, uint8_t ch);
#ifdef JFB_UTF8
void tvterm_uput1(TVterm * p, uint32_t idx, uint32_t ch);
void tvterm_uput2(TVterm * p, uint32_t idx, uint32_t ch);
#endif
void tvterm_text_clear_all(TVterm * p);

void tvterm_emulate(TVterm * p, const char *buff, int nchars);
void tvterm_refresh(TVterm * p);

void tvterm_init(TVterm * p, struct Raw_TTerm *tp, uint32_t hx, uint32_t hy, TCaps * caps, const char *en);
void tvterm_start(TVterm * p);
void tvterm_final(TVterm * p);

void tvterm_unregister_signal(void);
void tvterm_register_signal(TVterm * p);

void tvterm_show_sequence(FILE * fp, TCaps * cap, const char *en);

/*

  flagBuff:
  |      7|      6|      5|4||3|2|1|0|
  |CLEAN_S|LATCH_2|LATCH_1| ||<----->|
  |0=latch|  byte2|  byte1| ||   LANG|

  */

#endif /* INCLUDE_VTERM_H */
