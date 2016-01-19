lib_LTLIBRARIES = \
	libObjectivelyMVC.la

include_HEADERS = \
	Action.h \
	Button.h \
	Colors.h \
	Control.h \
	Font.h \
	ImageView.h \
	Label.h \
	View.h \
	Viewcontroller.h

libObjectivelyMVC_la_SOURCES = \
	Action.c \
	Button.c \
	Colors.c \
	Control.c \
	Font.c \
	ImageView.c \
	Label.c \
	View.c \
	ViewController.c

libObjectivelyMVC_la_CFLAGS = \
	-I .. -I ../../ \
	@HOST_CFLAGS@ \
	@FONTCONFIG_CFLAGS@ \
	@OBJECTIVELY_CFLAGS@ \
	@SDL2_CFLAGS@

libObjectivelyMVC_la_LDFLAGS = \
	@HOST_LDFLAGS@ \
	-shared

libObjectivelyMVC_la_LIBADD = \
	@HOST_LIBS@ \
	@FONTCONFIG_LIBS@ \
	@OBJECTIVELY_LIBS@ \
	@SDL2_LIBS@
