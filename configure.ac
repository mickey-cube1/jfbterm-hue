dnl Process this file with autoconf to produce a configure script.
AC_INIT([jfbterm-hue],[0.4.7])
AC_CONFIG_SRCDIR([fbcommon.c])
AM_INIT_AUTOMAKE

AM_CONFIG_HEADER(config.h)

dnl Checks for programs.
AC_PROG_CC
if test x$CC = xgcc; then
        CFLAGS="$CFLAGS -Wimplicit -Wreturn-type -Wunused -Wswitch -Wcomment -Wtrigraphs -Wformat -Wchar-subscripts -Wuninitialized -Wparentheses -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align"
fi
AC_PROG_INSTALL

dnl Checks for libraries.
dnl Replace `main' with a function in -lm:
AC_CHECK_LIB(m, main)

dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(fcntl.h malloc.h sys/ioctl.h sys/time.h unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_DIAGNOSE([obsolete],[your code should no longer depend upon `HAVE_ST_RDEV', but
`HAVE_STRUCT_STAT_ST_RDEV'.  Remove this warning and
the `AC_DEFINE' when you adjust the code.])dnl
AC_CHECK_MEMBERS([struct stat.st_rdev],[AC_DEFINE(HAVE_ST_RDEV, 1,
			    [Define to 1 if your `struct stat' has `st_rdev'.
			     Deprecated, use `HAVE_STRUCT_STAT_ST_RDEV'
			     instead.])])

AC_HEADER_TIME

dnl Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_HEADER_MAJOR
AC_FUNC_MMAP
AC_DIAGNOSE([obsolete],[your code may safely assume C89 semantics that RETSIGTYPE is void.
Remove this warning and the `AC_CACHE_CHECK' when you adjust the code.])dnl
AC_CACHE_CHECK([return type of signal handlers],[ac_cv_type_signal],[AC_COMPILE_IFELSE(
[AC_LANG_PROGRAM([#include <sys/types.h>
#include <signal.h>
],
		 [return *(signal (0, 0)) (0) == 1;])],
		   [ac_cv_type_signal=int],
		   [ac_cv_type_signal=void])])
AC_DEFINE_UNQUOTED([RETSIGTYPE],[$ac_cv_type_signal],[Define as the return type of signal handlers
		    (`int' or `void').])

AC_FUNC_VPRINTF
AC_CHECK_FUNCS(select strdup strerror ioperm)

dnl setup options.
AC_PATH_PROGS(jfb_gzip_path, gunzip, :)
if test $jfb_gzip_path = ":"; then
	AC_MSG_ERROR(Test for gunzip failed. Please install gunzip program.)
else
	AC_DEFINE_UNQUOTED(JFB_GZIP_PATH, "${jfb_gzip_path}", [zip command path])
fi

AC_ARG_ENABLE(colorgamma, [ --enable-color-gamma     turn on color gamma [default no]],,enable_colorgamma=no)
AC_MSG_CHECKING(whether to enable color gamma)
if test x$enable_colorgamma = xyes; then
        AC_MSG_RESULT(yes)
        AC_DEFINE(JFB_COLOR_GAMMA, 1, [color gamma])
else
        AC_MSG_RESULT(no)
fi
AC_ARG_ENABLE(reverse-video, [ --enable-reverse-video   support reverse cmap [default no]],,enable_reverse_video=no)
AC_MSG_CHECKING(whether to enable reverse-video)
if test x$enable_reverse_video = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(JFB_REVERSEVIDEO, 1, [support reverse cmap])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(debug, [ --enable-debug           turn on debugging [default no]],,enable_debug=no)
AC_MSG_CHECKING(whether to enable debugging)
if test x$enable_debug = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(DEBUG, 1, [debug flag])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(utf8, [ --enable-utf8            support UTF-8 [default yes]],,enable_utf8=yes)
AC_MSG_CHECKING(whether to enable utf8)
if test x$enable_utf8 = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(JFB_UTF8, 1, [support UTF-8])
else
	AC_MSG_RESULT(no)
fi
AC_ARG_ENABLE(other-coding-system, [ --enable-other-coding-system support other-coding-system [default yes]],,enable_other=$enable_utf8)
AC_MSG_CHECKING(whether to enable other coding system)
if test x$enable_utf8 = xyes -a x$enable_other = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(JFB_OTHER_CODING_SYSTEM, 1, [support other-coding-system])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(1bpp, [ --enable-1bpp            support 1bpp [default no]],,enable_1bpp=no)
AC_MSG_CHECKING(whether to enable 1bpp)
if test x$enable_1bpp = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(JFB_1BPP, 1, [support 1bpp])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(2bpp, [ --enable-2bpp            support 2bpp [default yes]],,enable_2bpp=yes)
AC_MSG_CHECKING(whether to enable 2bpp)
if test x$enable_2bpp = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(JFB_2BPP, 1, [support 2bpp])
else
	AC_MSG_RESULT(no)
fi

dnl AC_ARG_ENABLE(4bpp, [ --enable-4bpp            support 4bpp [default no]],,enable_4bpp=no)
dnl AC_MSG_CHECKING(whether to enable 4bpp)
dnl if test x$enable_4bpp = xyes; then
dnl	AC_MSG_RESULT(yes)
dnl	AC_DEFINE(JFB_4BPP, 1, [support 4pp])
dnl else
dnl	AC_MSG_RESULT(no)
dnl fi

AC_ARG_ENABLE(8bpp, [ --enable-8bpp            support 8bpp [default yes]],,enable_8bpp=yes)
AC_MSG_CHECKING(whether to enable 8bpp)
if test x$enable_8bpp = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(JFB_8BPP, 1, [support 8bpp])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(15bpp, [ --enable-15bpp           support 15bpp [default yes]],,enable_15bpp=yes)
AC_MSG_CHECKING(whether to enable 15bpp)
if test x$enable_15bpp = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(JFB_15BPP, 1, [support 15bpp])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(16bpp, [ --enable-16bpp           support 16bpp [default yes]],,enable_16bpp=yes)
AC_MSG_CHECKING(whether to enable 16bpp)
if test x$enable_16bpp = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(JFB_16BPP, 1, [support 16bpp])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(24bpp, [ --enable-24bpp           support 24bpp [default yes]],,enable_24bpp=yes)
AC_MSG_CHECKING(whether to enable 24bpp)
if test x$enable_24bpp = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(JFB_24BPP, 1, [support 24bpp])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(32bpp, [ --enable-32bpp           support 32bpp [default yes]],,enable_32bpp=yes)
AC_MSG_CHECKING(whether to enable 32bpp)
if test x$enable_32bpp = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(JFB_32BPP, 1, [support 32bpp])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(pseudo-color, [ --enable-pseudo-color    support pseudo-color [default yes]],,enable_pseudo_color=yes)
AC_MSG_CHECKING(whether to enable pseudo-color)
if test x$enable_pseudo_color = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(JFB_PSEUDOCOLOR, 1, [support pseudocolor])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(direct-color, [ --enable-direct-color    support direct-color (EXPERMINAL) [default no]],,enable_direct_color=no)
AC_MSG_CHECKING(whether to enable direct-color)
if test x$enable_direct_color = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(JFB_DIRECTCOLOR, 1, [support directcolor])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(true-color, [ --enable-true-color      support true-color [default yes]],,enable_true_color=yes)
AC_MSG_CHECKING(whether to enable true-color)
if test x$enable_true_color = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(JFB_TRUECOLOR, 1, [support turecolor])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(vga16fb, [ --enable-vga16fb         support vga16fb [default yes]],,enable_vga16fb=yes)
AC_MSG_CHECKING(whether to enable vga16fb)
if test x$enable_vga16fb = xyes; then
	AC_MSG_RESULT(yes)
	if test x$ac_cv_func_ioperm = xyes; then
	   AC_DEFINE(JFB_VGA16FB, 1, [support vga16fb])
	else
	   AC_MSG_WARN([ but ioperm() not found on your system, disable vga16fb support])
	   enable_vga16fb=no
	fi
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(packed-pixels, [ --enable-packed-pixels         support packed-pixels [default yes]],,enable_packed_pixels=yes)
AC_MSG_CHECKING(whether to enable packed-pixels)
if test x$enable_packed_pixels = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(JFB_PACKED, 1, [support packed-pixels])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(check-mmio-error, [ --enable-check-mmio-error         Check mmap(mmio) return code [default no]],,enable_check_mmio_error=no)
AC_MSG_CHECKING(whether to enable check-mmio-error)
if test x$enable_check_mmio_error = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(JFB_MMIO_CHECK, 1, [check mmap(mmio)])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(force-8bit, [ --enable-force-8bit         Force 8bit Color [default no]],,enable_force_8bit=no)
AC_MSG_CHECKING(whether to enable force-8bit)
if test x$enable_force_8bit = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(FORCE_8BIT, 1, [force 8bit color])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(dimmer, [ --enable-dimmer          Enable auto dimmer (condole blank) [default no]],,enable_dimmer=no)
AC_MSG_CHECKING(whether to enable dimmer)
if test x$enable_dimmer = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(JFB_ENABLE_DIMMER, 1, [enable auto dimmer])
else
	AC_MSG_RESULT(no)
fi

if test x$sysconfdir = 'x${prefix}/etc'; then
	if test x$prefix = xNONE; then
		AC_DEFINE_UNQUOTED(SYSCONFDIR, "/usr/local/etc", [config file dir])
	else
		AC_DEFINE_UNQUOTED(SYSCONFDIR, "${prefix}/etc", [config file dir])
	fi
else
	AC_DEFINE_UNQUOTED(SYSCONFDIR, "${sysconfdir}", [config file dir])
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
