AC_PREREQ(2.69)

AC_INIT([ObjectivelyMVC], [0.1.0], [jay@jaydolan.com])

AC_CONFIG_HEADERS([Sources/ObjectivelyMVC/Config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([Sources/ObjectivelyMVC.h])

AC_CANONICAL_HOST

AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_LIBTOOL

AM_INIT_AUTOMAKE([foreign])
AM_SILENT_RULES([yes])

AH_TOP([#pragma once])

HOST_NAME=""
HOST_CFLAGS=""
HOST_LDFLAGS=""
HOST_LIBS=""

AC_MSG_CHECKING([host type])
case "$host" in
	*-apple-*)
		HOST_NAME="APPLE"
		HOST_LDFLAGS="-headerpad_max_install_names"
		HOST_LIBS="-framework OpenGL"
		;;
	*-mingw*)
		HOST_NAME="MINGW"
		HOST_CFLAGS="-D_GNU_SOURCE"
		HOST_LDFLAGS="-no-undefined"
		HOST_LIBS="-lopengl32 -lglu32"
		;;
	*)
		HOST_NAME="LINUX"
		HOST_CFLAGS="-D_GNU_SOURCE"
		HOST_LIBS="-lGL -lGLU"
		;;
esac

AC_SUBST(HOST_NAME)
AC_SUBST(HOST_CFLAGS)
AC_SUBST(HOST_LDFLAGS)
AC_SUBST(HOST_LIBS)

AM_CONDITIONAL([APPLE], [test "x$HOST_NAME" = "xAPPLE"])
AM_CONDITIONAL([MINGW], [test "x$HOST_NAME" = "xMINGW"])
AM_CONDITIONAL([LINUX], [test "x$HOST_NAME" = "xLINUX"])

AC_CHECK_HEADERS([GL/gl.h OpenGL/gl.h])
AC_CHECK_HEADERS([GL/glu.h OpenGL/glu.h])

PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])
PKG_CHECK_MODULES([FONTCONFIG], [fontconfig >= 2.10.0])
PKG_CHECK_MODULES([OBJECTIVELY], [Objectively >= 0.4.0])
PKG_CHECK_MODULES([SDL2], [sdl2 >= 2.0.4 SDL2_image SDL2_ttf])

AC_CONFIG_FILES([
	Makefile
	Assets/Makefile
	Sources/Makefile
	Sources/ObjectivelyMVC.pc
	Sources/ObjectivelyMVC/Makefile
	Examples/Makefile
])

AC_OUTPUT
