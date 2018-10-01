/*
 * JFBTERM -
 * Copyright (C) 1999  Noritoshi MASUICHI (nmasu@ma3.justnet.ne.jp)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
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

#ifndef INCLUDE_FBCOMMON_H
#define INCLUDE_FBCOMMON_H

#include <stdint.h>
#include <sys/types.h>

struct Raw_TFrameBufferMemory;

typedef struct Raw_TFrameBufferCapability
{
	uint32_t bitsPerPixel;
	uint32_t fbType;
	uint32_t fbVisual;
	void (*fill) (struct Raw_TFrameBufferMemory * p, uint32_t x, uint32_t y, uint32_t lx, uint32_t ly, uint32_t color);
	void (*overlay) (struct Raw_TFrameBufferMemory * p, uint32_t xd, uint32_t yd,
			 const uint8_t * ps, uint32_t lx, uint32_t ly, uint32_t gap, uint32_t color);
	void (*clear_all) (struct Raw_TFrameBufferMemory * p, uint32_t color);
	void (*reverse) (struct Raw_TFrameBufferMemory * p, uint32_t x, uint32_t y, uint32_t lx, uint32_t ly, uint32_t color);
} TFrameBufferCapability;

typedef struct Raw_TFrameBufferMemory
{
	uint32_t height;
	uint32_t width;
	uint32_t bytePerLine;
	/* --- */
	int fh;
	uint64_t sstart;
	uint64_t soff;
	uint64_t slen;
	uint64_t mstart;
	uint64_t moff;
	uint64_t mlen;
	uint8_t *smem;
	uint8_t *mmio;
	/* function hooks */
	TFrameBufferCapability cap;
#ifdef JFB_ENABLE_DIMMER
	uint16_t my_vt;
#endif
	int tty0fd;
#if 0
	  (*init) (void),	/* 初期化 */
	  (*text_mode) (void),	/* テキストモードに切替え */
	  (*graph_mode) (void),	/* グラフィックモードに切替え */
	  (*wput) (uint8_t * code, uint8_t fc, uint8_t bc),	/* 漢字出力 */
	  (*sput) (uint8_t * code, uint8_t fc, uint8_t bc),	/* ANK出力 */
	  (*set_cursor_address) (struct cursorInfo * c, uint32_t x, uint32_t y),
		/* カーソル c のアドレスを (x,y) に設定 */
	  (*set_address) (uint32_t i),
		/* 文字書き込みアドレスを i 文字目に設定 */
	  (*cursor) (struct cursorInfo *),	/* カーソルをトグル */
	  (*screen_saver) (bool),	/* スクリーンブランク/アンブランク */
	  (*detatch) (void),	/* ドライバ解放 */
#endif
} TFrameBufferMemory;

extern TFrameBufferMemory gFramebuffer;

void tfbm_init(TFrameBufferMemory * p);
void tfbm_open(TFrameBufferMemory * p);
void tfbm_close(TFrameBufferMemory * p);

uint32_t tfbm_select_32_color(uint32_t);
uint16_t tfbm_select_16_color(uint32_t);

extern float fbgamma;

#endif /* INCLUDE_FBCOMMON_H */
