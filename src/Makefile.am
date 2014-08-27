lib_LTLIBRARIES = \
	libSDL_mvc.la

include_HEADERS = \
	MVC_button.h \
	MVC_color.h \
	MVC_image.h \
	MVC_label.h \
	MVC_main.h \
	MVC_object.h \
	MVC_view.h \
	MVC_viewcontroller.h \
	SDL_mvc.h

libSDL_mvc_la_SOURCES = \
	MVC_button.c \
	MVC_color.c \
	MVC_image.c \
	MVC_label.c \
	MVC_main.c \
	MVC_object.c \
	MVC_view.c \
	MVC_viewcontroller.c
	
libSDL_mvc_la_CFLAGS = \
	@GLIB_CFLAGS@ \
	@OPENGL_CFLAGS@ \
	@SDL2_CFLAGS@

libSDL_mvc_la_LDFLAGS = \
	-shared
