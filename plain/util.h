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

#ifndef INCLUDE_UTIL_H
#define INCLUDE_UTIL_H

#include <stdarg.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdlib.h>

void util_privilege_init();
void util_privilege_on();
void util_privilege_off();
int util_privilege_open(char *pathname, int flags);
#ifdef HAVE_IOPERM
int util_privilege_ioperm(unsigned long from, unsigned int num, int turn_on);
#endif
uid_t util_getuid();
void util_privilege_drop();

char *util_strdupC(const char *s);
char *util_sprintfC(const char *fmt, ...);
char *util_vsprintfC(const char *fmt, va_list ap);
#define util_free(p) do {free(p); (p) = NULL;} while (0)

void util_euc_to_sjis(uint8_t * ch, uint8_t * cl);
void util_sjis_to_jis(uint8_t * ch, uint8_t * cl);

int util_search_string(const char *s, const char **array);

#endif /* INCLUDE_UTIL_H */
