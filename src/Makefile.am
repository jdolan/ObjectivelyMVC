lib_LTLIBRARIES = \
	libSDL_mvc.la

include_HEADERS = \
	SDL_button.h \
	SDL_label.h \
	SDL_mvc.h \
	SDL_view.h \
	SDL_viewcontroller.h

libSDL_mvc_la_SOURCES = \
	SDL_button.c \
	SDL_label.c \
	SDL_mvc.c \
	SDL_view.c \
	SDL_viewcontroller.c
	
libSDL_mvc_la_CFLAGS = \
	@OPENGL_CFLAGS@ \
	@SDL2_CFLAGS@

libSDL_mvc_la_LDFLAGS = \
	-shared
