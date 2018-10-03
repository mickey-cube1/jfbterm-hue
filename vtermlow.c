/*
 * JFBTERM -
 * Copyright (c) 2003 Fumitoshi UKAI <ukai@debian.or.jp>
 * Copyright (C) 1999 Noritoshi MASUICHI (nmasu@ma3.justnet.ne.jp)
 *
 * KON2 - Kanji ON Console -
 * Copyright (C) 1992, 1993 MAEDA Atusi (mad@math.keio.ac.jp)
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
 * THIS SOFTWARE IS PROVIDED BY MASUICHI NORITOSHI AND MAEDA ATUSI AND
 * TAKASHI MANABE ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE TERRENCE R.
 * LAMBERT BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#define __USE_STRING_INLINES
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unistd.h>
#include	<errno.h>
#include	<sys/vt.h>
#include	<fcntl.h>
#include	<signal.h>
#include	<termios.h>
#include	<sys/ioctl.h>
#include	<sys/kd.h>

#include	"util.h"
#include	"vterm.h"
#include	"term.h"
#include	"font.h"
#include	"fbcommon.h"
#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if 0

static int saveTime, saverCount;
static bool saved;
static volatile bool busy;	/* TRUE iff updating screen */
static volatile bool release;	/* delayed VC switch flag */

static void ShowCursor(struct cursorInfo *, bool);

#endif

static void sig_leave_virtual_console(int signum);
static void sig_enter_virtual_console(int signum);

static void tvterm_text_clean_band(TVterm * p, uint32_t top, uint32_t btm);

/*---------------------------------------------------------------------------*/
static inline uint32_t tvterm_coord_to_index(TVterm * p, uint32_t x, uint32_t y)
{
	return (p->textHead + x + y * p->xcap4) % p->tsize;
}

static inline int IsKanji(TVterm * p, uint32_t x, uint32_t y)
{
	return p->flag[tvterm_coord_to_index(p, x, y)] & CODEIS_1;
}

static inline int IsKanji2(TVterm * p, uint32_t x, uint32_t y)
{
	return p->flag[tvterm_coord_to_index(p, x, y)] & CODEIS_2;
}

static inline void KanjiAdjust(TVterm * p, uint32_t * x, uint32_t * y)
{
	if (IsKanji2(p, *x, *y)) {
		--*x;
	}
}

/*---------------------------------------------------------------------------*/
static inline void brmove(void *dst, void *src, int n)
{
	memmove((void *) ((char *) dst - n), (void *) ((char *) src - n), n);
}

static inline void blatch(void *head, int n)
{
	unsigned char *c = (unsigned char *) head;
	unsigned char *e = (unsigned char *) head + n;
	for (; c < e; c++) {
		*c &= 0x7f;
	}
}

static inline void llatch(void *head, int n)
{
	unsigned int *l = (unsigned int *) head;
	unsigned int *e = (unsigned int *) head + (n >> 2);
	for (; l < e; l++) {
		*l &= 0x7f7f7f7f;
	}
}

static inline void tvterm_move(TVterm * p, int dst, int src, int n)
{
	memmove(p->text + dst, p->text + src, n * sizeof(uint32_t));
	memmove(p->attr + dst, p->attr + src, n);
	memmove(p->flag + dst, p->flag + src, n);
}

static inline void tvterm_brmove(TVterm * p, int dst, int src, int n)
{
	brmove(p->text + dst, p->text + src, n * sizeof(uint32_t));
	brmove(p->attr + dst, p->attr + src, n);
	brmove(p->flag + dst, p->flag + src, n);
}

static inline void tvterm_clear(TVterm * p, int top, int n)
{
	bzero(p->text + top, n * sizeof(uint32_t));
	bzero(p->flag + top, n);
	memset(p->attr + top, (p->pen.bcol << 4), n);
}

/*---------------------------------------------------------------------------*/
void tvterm_delete_n_chars(TVterm * p, int n)
{
	uint32_t addr;
	uint32_t dx;

	addr = tvterm_coord_to_index(p, p->pen.x, p->pen.y);
	dx = p->xcap - p->pen.x - n;

	tvterm_move(p, addr, addr + n, dx);	/* bmove */
	blatch(p->flag + addr, dx);

	addr = tvterm_coord_to_index(p, p->xcap - n, p->pen.y);
	tvterm_clear(p, addr, n);
}

void tvterm_insert_n_chars(TVterm * p, int n)
{
	uint32_t addr;
	uint32_t dx;

	addr = tvterm_coord_to_index(p, p->xcap - 1, p->pen.y);
	dx = p->xcap - p->pen.x - n;
	tvterm_brmove(p, addr, addr - n, dx);

	addr = tvterm_coord_to_index(p, p->pen.x, p->pen.y);
	blatch(p->flag + addr + n, dx);
	tvterm_clear(p, addr, n);
}

#if 0				/* ハードウエアスクロールする際に使う。(現在未使用) */
static void tvterm_scroll_up_n_lines(TVterm * p, int n)
{
	int h;
	int t;

	h = p->textHead;
	textHead += n * p->xcap4;
	if (p->textHead > p->tsize) {
		p->textHead -= p->tsize;
		n = p->tsize - h;
		if (p->textHead) {
			tvterm_clear(p, 0, p->textHead);	/* lclear */
		}
	}
	else {
		n = p->textHead - h;
	}
	tvterm_clear(p, h, n);	/* lclear */
}

static void tvterm_scroll_down_n_lines(TVterm * p, int n)
{
	int h;
	int t;

	h = p->textHead;
	p->textHead -= n * p->xcap4;
	if (p->textHead < 0) {
		p->textHead += p->tsize;
		if (h) {
			tvterm_clear(p, 0, h);	/* lclear */
		}
		n = p->tsize - p->textHead;
	}
	else {
		n = h - p->textHead;
	}
	tvterm_clear(p, h, n);	/* lclear */
}
#endif

void tvterm_set_cursor_wide(TVterm * p, TBool b)
{
	p->cursor.wide = b;
}

void tvterm_show_cursor(TVterm * p, TBool b)
{
	if (!p->cursor.on) {
		return;
	}
	if (p->cursor.shown != b) {
#ifdef JFB_REVERSEVIDEO
/* -- kei --
	On this implementation, JFB_REVERSEVIDEO is effective on
	only 2bpp machine.
	On reverse video mode, cursor should be 0x00.
	On non reverse video mode, cursor should be 0xf.
	Yes, I know following is little bit dirty code, but
	temporal fix...
*/
		if (gFramebuffer.cap.bitsPerPixel == 2) {
			gFramebuffer.cap.reverse(&gFramebuffer,
						 gFontsWidth * p->cursor.x,
						 gFontsHeight * p->cursor.y,
						 p->cursor.width + (p->cursor.wide ? p->cursor.width : 0), p->cursor.height, 0x0);
		}
		else {
			gFramebuffer.cap.reverse(&gFramebuffer,
						 gFontsWidth * p->cursor.x,
						 gFontsHeight * p->cursor.y,
						 p->cursor.width + (p->cursor.wide ? p->cursor.width : 0), p->cursor.height, 0xf);
		}
#else
		gFramebuffer.cap.reverse(&gFramebuffer,
					 gFontsWidth * p->cursor.x,
					 gFontsHeight * p->cursor.y,
					 p->cursor.width + (p->cursor.wide ? p->cursor.width : 0), p->cursor.height, 0xf);
#endif
		p->cursor.shown = b;
	}
}

/*---------------------------------------------------------------------------*/
void tvterm_refresh(TVterm * p)
{
	uint32_t i;
	uint32_t x, y;
	uint32_t lang;
	uint32_t chlw;
	uint8_t fc, bc, fg;
	TFont *pf;
	const uint8_t *glyph;
	uint32_t w, gw;

	p->busy = TRUE;
	if (!p->active) {
		p->busy = FALSE;
		return;
	}

	tvterm_show_cursor(p, FALSE);

	if (p->textClear) {
		gFramebuffer.cap.fill(&gFramebuffer, 0, 0, gFramebuffer.width, gFramebuffer.height, 0);
		p->textClear = FALSE;
	}

	for (y = 0; y < p->ycap; y++) {
		for (x = 0; x < p->xcap; x++) {
			w = 1;
			i = tvterm_coord_to_index(p, x, y);
			fg = p->flag[i];
			if (fg & CLEAN_S) {
				continue;	/* already clean */
			}
			fc = p->attr[i] & 0xf;
			bc = p->attr[i] >> 4;
			chlw = p->text[i];
			lang = (chlw >> 24) & 0xFF;
			p->flag[i] |= CLEAN_S;

			if (fg & CODEIS_1) {
				pf = &(gFont[lang]);
				i++;
				p->flag[i] |= CLEAN_S;
				w = 2;
#if 0
				if (p->ins) {
					tvterm_insert_n_chars(p, 2);
				}
#endif
			}
			else {
				pf = &(gFont[lang]);
#if 0
				if (p->ins) {
					tvterm_insert_n_chars(p, 1);
				}
#endif
			}
			/* XXX: multiwidth support (unifont) */
			glyph = pf->conv(pf, chlw, &gw);
			gFramebuffer.cap.fill(&gFramebuffer,
					      gFontsWidth * x, gFontsHeight * y, gFontsWidth * w, gFontsHeight, bc);
			if (chlw == 0)
				continue;
			gFramebuffer.cap.overlay(&gFramebuffer,
						 gFontsWidth * x, gFontsHeight * y, glyph, gw, pf->height, pf->bytew, fc);
		}
	}
	if (p->pen.x < p->xcap && p->pen.y < p->ycap) {
		/* XXX: pen position go out of screen by resize(1) for example */
		tvterm_set_cursor_wide(p, IsKanji(p, p->pen.x, p->pen.y));
		p->cursor.x = p->pen.x;
		p->cursor.y = p->pen.y;
		tvterm_show_cursor(p, TRUE);
	}

	p->busy = FALSE;
	if (p->release) {
		sig_leave_virtual_console(SIGUSR1);
	}
}

/*---------------------------------------------------------------------------*/

static TVterm *sig_obj = NULL;

void tvterm_unregister_signal(void)
{
	int ret;
	struct vt_mode vtm;
#if defined(HAVE_SIGACTION)
	struct sigaction sa;
#endif

#if defined(HAVE_SIGACTION)
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sa.sa_flags |= (SA_NOMASK | SA_ONESHOT);
	if (sigaction(SIGUSR1, &sa, NULL) != 0) {
		fprintf(stderr, "sigaction failed @ tvterm_unregister_signal(): %s\n", strerror(errno));
		exit(1);
	}
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sa.sa_flags |= (SA_NOMASK | SA_ONESHOT);
	if (sigaction(SIGUSR2, &sa, NULL) != 0) {
		fprintf(stderr, "sigaction failed @ tvterm_unregister_signal(): %s\n", strerror(errno));
		exit(1);
	}
#else
	signal(SIGUSR1, SIG_DFL);
	signal(SIGUSR2, SIG_DFL);
#endif

	vtm.mode = VT_AUTO;
	vtm.waitv = 0;
	vtm.relsig = 0;
	vtm.acqsig = 0;
	vtm.frsig = 0;
	ret = ioctl(0, VT_SETMODE, &vtm);
	if (ret == -1) {
		fprintf(stderr, "VT_SETMODE failed @ tvterm_unregister_signal(): %s\n", strerror(errno));
	}

	ret = ioctl(gFramebuffer.tty0fd, TIOCCONS, NULL);
	if (ret == -1) {
		fprintf(stderr, "TIOCCONS failed @ tvterm_unregister_signal(): %s\n", strerror(errno));
	}
}

void tvterm_register_signal(TVterm * p)
{
	int ret;
	struct vt_mode vtm;
#if defined(HAVE_SIGACTION)
	struct sigaction sa;
#endif

	sig_obj = p;

#if defined(HAVE_SIGACTION)
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sig_leave_virtual_console;
	sa.sa_flags |= SA_RESTART;
	if (sigaction(SIGUSR1, &sa, NULL) != 0) {
		fprintf(stderr, "sigaction failed @ tvterm_register_signal(): %s\n", strerror(errno));
		exit(1);
	}
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sig_enter_virtual_console;
	sa.sa_flags |= SA_RESTART;
	if (sigaction(SIGUSR2, &sa, NULL) != 0) {
		fprintf(stderr, "sigaction failed @ tvterm_register_signal(): %s\n", strerror(errno));
		exit(1);
	}
#else
	signal(SIGUSR1, sig_leave_virtual_console);
	signal(SIGUSR2, sig_enter_virtual_console);
#endif

	vtm.mode = VT_PROCESS;
	vtm.waitv = 0;
	vtm.relsig = SIGUSR1;
	vtm.acqsig = SIGUSR2;
	vtm.frsig = 0;
	ret = ioctl(0, VT_SETMODE, &vtm);
	if (ret == -1) {
		fprintf(stderr, "VT_SETMODE failed @ tvterm_register_signal(): %s\n", strerror(errno));
	}

	ret = ioctl(sig_obj->term->ttyfd, TIOCCONS, NULL);
	if (ret == -1) {
		fprintf(stderr, "TIOCCONS failed @ tvterm_register_signal(): %s\n", strerror(errno));
	}

	llatch(p->flag, p->tsize);
	p->textClear = TRUE;
	tvterm_refresh(p);
}

static void sig_leave_virtual_console(int signum)
{
#if !defined(HAVE_SIGACTION)
	signal(SIGUSR1, sig_leave_virtual_console);
#endif
	if (sig_obj->busy) {
		sig_obj->release = TRUE;
		return;
	}
	else {
		sig_obj->release = FALSE;
		sig_obj->active = FALSE;
		/*
		 * SetTextMode();
		 */
		ioctl(0, VT_RELDISP, 1);
	}
}

static void sig_enter_virtual_console(int signum)
{
#if !defined(HAVE_SIGACTION)
	signal(SIGUSR2, sig_enter_virtual_console);
#endif
	if (!sig_obj->active) {
		sig_obj->active = TRUE;
		tvterm_register_signal(sig_obj);
//#if !defined(HAVE_SIGACTION)
//		signal(SIGUSR2, sig_enter_virtual_console);
//#endif
	}
}

void tvterm_wput(TVterm * p, uint32_t idx, uint8_t ch1, uint8_t ch2)
{
	uint32_t a = tvterm_coord_to_index(p, p->pen.x, p->pen.y);

	p->attr[a] = p->pen.fcol | (p->pen.bcol << 4);
	p->text[a] = (idx << 24) | (ch1 << 8) | ch2;
	p->text[a + 1] = 0;
	p->flag[a] = LATCH_1;
	p->flag[a + 1] = LATCH_2;
}

void tvterm_sput(TVterm * p, uint32_t idx, uint8_t ch)
{
	uint32_t a = tvterm_coord_to_index(p, p->pen.x, p->pen.y);

	p->attr[a] = p->pen.fcol | (p->pen.bcol << 4);
	p->text[a] = (idx << 24) | ch;
	p->flag[a] = LATCH_S;
}

#ifdef JFB_UTF8
void tvterm_uput1(TVterm * p, uint32_t idx, uint32_t ch)
{
	uint32_t a = tvterm_coord_to_index(p, p->pen.x, p->pen.y);

	p->attr[a] = p->pen.fcol | (p->pen.bcol << 4);
	p->text[a] = (idx << 24) | ch;
	p->flag[a] = LATCH_S;
}

void tvterm_uput2(TVterm * p, uint32_t idx, uint32_t ch)
{
	uint32_t a = tvterm_coord_to_index(p, p->pen.x, p->pen.y);

	p->attr[a] = p->pen.fcol | (p->pen.bcol << 4);
	p->text[a] = (idx << 24) | ch;
	p->text[a + 1] = 0;
	p->flag[a] = LATCH_1;
	p->flag[a + 1] = LATCH_2;
}
#endif

/**
	行区間 [0, TVterm::ymax) をクリアする。
**/
void tvterm_text_clear_all(TVterm * p)
{
	uint32_t y;
	uint32_t a;

	for (y = 0; y < p->ymax; y++) {
		a = tvterm_coord_to_index(p, 0, y);
		tvterm_clear(p, a, p->xcap4);	/* lclear */
	}
	p->textClear = TRUE;
}

/**
	行内の指定部分をクリアする。
mode
1	行 TVterm::y カラム[0, TVterm::x) をクリアする。
2	行 TVterm::y をクリアする。
ow	行 TVterm::y カラム[TVterm::x, TVterm::xcap) をクリアする。
**/
void tvterm_text_clear_eol(TVterm * p, uint8_t mode)
{
	uint32_t a;
	uint8_t len;
	uint8_t x = 0;

	switch (mode) {
	case 1:
		len = p->pen.x;
		break;
	case 2:
		len = p->xcap;
		break;
	default:
		x = p->pen.x;
		len = p->xcap - p->pen.x;
		break;
	}
	a = tvterm_coord_to_index(p, x, p->pen.y);
	tvterm_clear(p, a, len);
}

/**
	スクリーン内の指定部分をクリアする。
mode
1	行 [0, TVterm::y) をクリアし、さらに
	行 TVterm::y カラム[0, TVterm::x) をクリアする。
2	スクリーン全体をクリアする。
ow	行 [TVterm::y+1, TVterm::ycap) をクリアし、さらに
	行 TVterm::y カラム[TVterm::x, TVterm::xcap) をクリアする。
**/
void tvterm_text_clear_eos(TVterm * p, uint8_t mode)
{
	uint32_t a;
	uint32_t len;

	switch (mode) {
	case 1:
		tvterm_text_clean_band(p, 0, p->pen.y);
		a = tvterm_coord_to_index(p, 0, p->pen.y);
		tvterm_clear(p, a, p->pen.x);
		break;
	case 2:
		tvterm_text_clear_all(p);
		break;
	default:
		tvterm_text_clean_band(p, p->pen.y + 1, p->ycap);
		a = tvterm_coord_to_index(p, p->pen.x, p->pen.y);
		len = p->xcap - p->pen.x;
		tvterm_clear(p, a, len);
		break;
	}
}

/**
	行区間 [top, btm) をクリアする。
**/
static void tvterm_text_clean_band(TVterm * p, uint32_t top, uint32_t btm)
{
	uint32_t y;
	uint32_t a;

	for (y = top; y < btm; y++) {
		a = tvterm_coord_to_index(p, 0, y);
		tvterm_clear(p, a, p->xcap4);	/* lclear */
		/* needless to latch */
	}
}

/**
**/
void tvterm_text_scroll_down(TVterm * p, int line)
{
	tvterm_text_move_down(p, p->ymin, p->ymax, line);
}

void tvterm_text_move_down(TVterm * p, int top, int btm, int line)
{
	uint32_t n;
	uint32_t src;
	uint32_t dst;

	if (btm <= top + line) {
		tvterm_text_clean_band(p, top, btm);
	}
	else {
		for (n = btm - 1; n >= top + line; n--) {
			dst = tvterm_coord_to_index(p, 0, n);
			src = tvterm_coord_to_index(p, 0, n - line);
			tvterm_move(p, dst, src, p->xcap4);	/* lmove */
			llatch(p->flag + dst, p->xcap4);
		}
		tvterm_text_clean_band(p, top, top + line);
	}
}

/**
**/
void tvterm_text_scroll_up(TVterm * p, int line)
{
	tvterm_text_move_up(p, p->ymin, p->ymax, line);
}

void tvterm_text_move_up(TVterm * p, int top, int btm, int line)
{
	uint32_t n;
	uint32_t src;
	uint32_t dst;

	if (btm <= top + line) {
		tvterm_text_clean_band(p, top, btm);
	}
	else {
		for (n = top; n < btm - line; n++) {
			dst = tvterm_coord_to_index(p, 0, n);
			src = tvterm_coord_to_index(p, 0, n + line);
			tvterm_move(p, dst, src, p->xcap4);	/* lmove */
			llatch(p->flag + dst, p->xcap4);
		}
		tvterm_text_clean_band(p, btm - line, btm);
	}
}

void tvterm_text_reverse(TVterm * p, int fx, int fy, int tx, int ty)
{
	uint32_t from, to, y, swp, xx, x;
	uint8_t fc, bc, fc2, bc2;

	KanjiAdjust(p, &fx, &fy);
	KanjiAdjust(p, &tx, &ty);
	if (fy > ty) {
		swp = fy;
		fy = ty;
		ty = swp;
		swp = fx;
		fx = tx;
		tx = swp;
	}
	else if (fy == ty && fx > tx) {
		swp = fx;
		fx = tx;
		tx = swp;
	}
	for (xx = p->xcap, y = fy; y <= ty; y++) {
		if (y == ty)
			xx = tx;
		from = tvterm_coord_to_index(p, fx, y);
		to = tvterm_coord_to_index(p, xx, y);
		if (p->flag[from] & CODEIS_2)
			/* 2nd byte of kanji */
			from--;
		for (x = from; x <= to; x++) {
			if (!p->text[x])
				continue;
			fc = p->attr[x];
			bc = fc >> 4;
			bc2 = (bc & 8) | (fc & 7);
			fc2 = (fc & 8) | (bc & 7);
			p->attr[x] = fc2 | (bc2 << 4);
			p->flag[x] &= ~CLEAN_S;
		}
		fx = 0;
	}
}

#if 0
/* Cursor related routines. */

static void ToggleCursor(struct cursorInfo *c)
{
	c->count = 0;
	if (con.text_mode)
		return;
	c->shown = !c->shown;
	vInfo.cursor(c);
}

static void ShowCursor(struct cursorInfo *c, bool show)
{
	if (!con.active || !c->sw)
		return;
	if (c->shown != show)
		ToggleCursor(c);
}

static void SaveScreen(bool save)
{
	if (saved != save) {
		saved = save;
		vInfo.screen_saver(save);
	}
	saverCount = 0;
}
#endif

#if 0
/* Called when some action was over, or every 1/10 sec when idle. */

void PollCursor(bool wakeup)
{
	if (!con.active)
		return;
	if (wakeup) {
		SaveScreen(FALSE);
		ShowCursor(&cInfo, TRUE);
		return;
	}
	/* Idle. */
	if (saved)
		return;
	if ((saveTime > 0) && (++saverCount == saveTime)) {
		ShowCursor(&cInfo, FALSE);
		SaveScreen(TRUE);
		return;
	}
	if ((cInfo.interval > 0) && (++cInfo.count == cInfo.interval)) {
		ToggleCursor(&cInfo);
	}
}

#endif

#if 0
/* Beep routines. */

#define	COUNTER_ADDR	0x61

static int beepCount;

static int ConfigBeep(const char *confstr)
{
	beepCount = atoi(confstr) * 10000;
	if (beepCount > 0)
		ioperm(COUNTER_ADDR, 1, TRUE);
	return SUCCESS;
}

void Beep(void)
{
	if (!con.active || beepCount <= 0)
		return;
	PortOutb(PortInb(COUNTER_ADDR) | 3, COUNTER_ADDR);
	usleep(beepCount);
	PortOutb(PortInb(COUNTER_ADDR) & 0xFC, COUNTER_ADDR);
}

static int ConfigInterval(const char *confstr)
{
	cInfo.interval = atoi(confstr);
	return SUCCESS;
}

static int ConfigSaver(const char *confstr)
{
	saveTime = atoi(confstr) * 600;	/* convert unit from minitue to 1/10 sec */
	return SUCCESS;
}
#endif
