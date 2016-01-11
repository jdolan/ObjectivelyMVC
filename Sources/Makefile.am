lib_LTLIBRARIES = \
	libSDL_mvc.la

include_HEADERS = \
	Color.h \
	Font.h \
	Image.h \
	Main.h \
	TextField.h \
	View.h \
	Viewcontroller.h \
	SDL_mvc.h

libSDL_mvc_la_SOURCES = \
	Color.c \
	Font.c \
	Image.c \
	Main.c \
	TextField.c \
	View.c \
	ViewController.c
	
libSDL_mvc_la_CFLAGS = \
	@FONTCONFIG_CFLAGS@ \
	@OBJECTIVELY_CFLAGS@ \
	@OPENGL_CFLAGS@ \
	@SDL2_CFLAGS@

libSDL_mvc_la_LDFLAGS = \
	-shared

libSDL_mvc_la_LIBADD = \
	@FONTCONFIG_LIBS@ \
	@OBJECTIVELY_LIBS@ \
	@OPENGL_LIBS@ \
	@SDL2_LIBS@
