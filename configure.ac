AC_PREREQ(2.69)

AC_INIT([ObjectivelyMVC], [0.1.0], [jay@jaydolan.com])

AC_CONFIG_HEADERS([Sources/ObjectivelyMVC/Config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([Sources/ObjectivelyMVC.h])

AC_CANONICAL_HOST

AC_USE_SYSTEM_EXTENSIONS

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
		AC_MSG_RESULT(apple)
		;;
	*-mingw*)
		HOST_NAME="MINGW"
		HOST_LDFLAGS="-no-undefined"
		HOST_LIBS="-lopengl32"
		AC_MSG_RESULT(mingw)
		;;
	*)
		HOST_NAME="LINUX"
		HOST_LIBS="-lGL"
		AC_MSG_RESULT(linux)
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

PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])
PKG_CHECK_MODULES([OBJECTIVELY], [Objectively >= 1.0.0])
PKG_CHECK_MODULES([SDL2], [sdl2 >= 2.0.5 SDL2_image SDL2_ttf])

AC_SUBST(SDL2_DLL_LIBS, $(echo $SDL2_LIBS | sed 's/-lmingw32//; s/-lSDL2main//'))

AC_CONFIG_FILES([
	Makefile
	Assets/Makefile
	Sources/Makefile
	Sources/ObjectivelyMVC.pc
	Sources/ObjectivelyMVC/Makefile
	Tests/Makefile
	Tests/ObjectivelyMVC/Makefile
	Examples/Makefile
])

AC_OUTPUT
