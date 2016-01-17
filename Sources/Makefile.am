lib_LTLIBRARIES = \
	libSDL_mvc.la

include_HEADERS = \
	Color.h \
	Font.h \
	ImageView.h \
	Main.h \
	Label.h \
	View.h \
	Viewcontroller.h \
	SDL_mvc.h

libSDL_la_SOURCES = \
	Color.c \
	Font.c \
	ImageView.c \
	Main.c \
	Label.c \
	View.c \
	ViewController.c
	
libSDL_la_CFLAGS = \
	@FONTCONFIG_CFLAGS@ \
	@OBJECTIVELY_CFLAGS@ \
	@OPENGL_CFLAGS@ \
	@SDL2_CFLAGS@

libSDL_la_LDFLAGS = \
	-shared

libSDL_la_LIBADD = \
	@FONTCONFIG_LIBS@ \
	@OBJECTIVELY_LIBS@ \
	@OPENGL_LIBS@ \
	@SDL2_LIBS@
