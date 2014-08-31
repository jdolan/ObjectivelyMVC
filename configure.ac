AC_PREREQ(2.68)

AC_INIT([SDL2_mvc], [0.1.0], [jay@jaydolan.com])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/SDL_mvc.h])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_SYSTEM

AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_LIBTOOL

AM_INIT_AUTOMAKE([silent-rules foreign])
AM_SILENT_RULES([yes])

dnl --------------------
dnl Check for FontConfig
dnl --------------------

PKG_CHECK_MODULES([FONTCONFIG], [fontconfig >= 2.10.0])

dnl --------------
dnl Check for GLib
dnl --------------
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.0.0])

dnl ---------------------
dnl Check for Objectively
dnl ---------------------

PKG_CHECK_MODULES([OBJECTIVELY], [objectively >= 0.1.0])

dnl ------------------------
dnl Check for SDL2, SDL2_ttf
dnl ------------------------

PKG_CHECK_MODULES([SDL2], [sdl2 >= 2.0.0 SDL2_image SDL2_ttf])

dnl -----------------------------------
dnl Sort out OpenGL an flags and libraries
dnl -----------------------------------

HOST_OPENGL_CFLAGS=""
HOST_OPENGL_LIBS=""

case "$host" in
	*-apple-*)
		HOST_OPENGL_LIBS="-framework OpenGL"
		;;
	*-mingw*)
		HOST_OPENGL_LIBS="-lopengl32"
		;;
	*)
		HOST_OPENGL_LIBS="-lGL"
		;;
esac

AC_SUBST(HOST_OPENGL_CFLAGS)
AC_SUBST(HOST_OPENGL_LIBS)

OPENGL_CFLAGS="$OPENGL_CFLAGS $HOST_OPENGL_CFLAGS"
AC_SUBST(OPENGL_CFLAGS)

OPENGL_LIBS="$OPENGL_LIBS $HOST_OPENGL_LIBS"
AC_SUBST(OPENGL_LIBS)

dnl ---------------------------------
dnl Check whether to build unit tests
dnl ---------------------------------

BUILD_TESTS=no

AC_ARG_WITH(tests,
	AS_HELP_STRING([--with-tests],
		[build unit tests]
	)
)

AC_MSG_CHECKING(whether to build unit tests)

if test "x${with_tests}" = xyes; then
	BUILD_TESTS=yes
	PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])
fi
AC_MSG_RESULT($BUILD_TESTS)

AM_CONDITIONAL(BUILD_TESTS, test "x$BUILD_TESTS" = xyes)

dnl ---------------
dnl Generate output
dnl ---------------

AC_CONFIG_FILES([
	Makefile
	src/Makefile
])

AC_OUTPUT
