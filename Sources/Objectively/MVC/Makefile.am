lib_LTLIBRARIES = \
	libObjectivelyMVC.la

include_HEADERS = \
	Color.h \
	Font.h \
	Image.h \
	Main.h \
	TextField.h \
	View.h \
	Viewcontroller.h

libSDL_la_SOURCES = \
	Color.c \
	Font.c \
	Image.c \
	Main.c \
	TextField.c \
	View.c \
	ViewController.c

libObjectivelyla_CFLAGS = \
	@FONTCONFIG_CFLAGS@ \
	@OBJECTIVELY_CFLAGS@ \
	@OPENGL_CFLAGS@ \
	@SDL2_CFLAGS@

libObjectivelyla_LDFLAGS = \
	-shared

libObjectivelyla_LIBADD = \
	@FONTCONFIG_LIBS@ \
	@OBJECTIVELY_LIBS@ \
	@OPENGL_LIBS@ \
	@SDL2_LIBS@
