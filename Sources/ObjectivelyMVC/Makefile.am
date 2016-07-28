AUTOMAKE_OPTIONS = nostdinc

pkgdatadir = $(datadir)/ObjectivelyMVC
pkgincludedir = $(includedir)/ObjectivelyMVC

pkginclude_HEADERS = \
	Action.h \
	Box.h \
	Button.h \
	Checkbox.h \
	Colors.h \
	Control.h \
	Font.h \
	Image.h \
	ImageView.h \
	Input.h \
	Label.h \
	Text.h \
	Log.h \
	Option.h \
	Panel.h \
	Renderer.h \
	ScrollView.h \
	Select.h \
	Slider.h \
	StackView.h \
	TableCellView.h \
	TableColumnView.h \
	TableView.h \
	TextView.h \
	Types.h \
	View.h \
	ViewController.h \
	WindowController.h

lib_LTLIBRARIES = \
	libObjectivelyMVC.la

libObjectivelyMVC_la_SOURCES = \
	Action.c \
	Box.c \
	Button.c \
	Checkbox.c \
	Colors.c \
	Control.c \
	Font.c \
	Image.c \
	ImageView.c \
	Input.c \
	Label.c \
	Option.c \
	Panel.c \
	Renderer.c \
	ScrollView.c \
	Select.c \
	Slider.c \
	StackView.c \
	TableCellView.c \
	TableColumnView.c \
	TableView.c \
	Text.c \
	TextView.c \
	View.c \
	ViewController.c \
	WindowController.c

libObjectivelyMVC_la_CFLAGS = \
	-I $(top_builddir) -I $(top_builddir)/Sources -DPKGDATADIR=\"$(pkgdatadir)\" \
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
