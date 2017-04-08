AUTOMAKE_OPTIONS = nostdinc

pkgdatadir = $(datadir)/ObjectivelyMVC
pkgincludedir = $(includedir)/ObjectivelyMVC

pkginclude_HEADERS = \
	Action.h \
	Box.h \
	Button.h \
	Checkbox.h \
	CollectionItemView.h \
	CollectionView.h \
	Colors.h \
	ColorSelect.h \
	Config.h \
	Control.h \
	Font.h \
	Image.h \
	ImageView.h \
	Input.h \
	Text.h \
	Label.h \
	Log.h \
	NavigationViewController.h \
	Option.h \
	Panel.h \
	Renderer.h \
	ScrollView.h \
	Select.h \
	Slider.h \
	StackView.h \
	TableCellView.h \
	TableColumn.h \
	TableHeaderCellView.h \
	TableHeaderView.h \
	TableRowView.h \
	TableView.h \
	TextView.h \
	Types.h \
	View.h \
	View+JSON.h \
	ViewController.h \
	WindowController.h

lib_LTLIBRARIES = \
	libObjectivelyMVC.la

libObjectivelyMVC_la_SOURCES = \
	Action.c \
	Box.c \
	Button.c \
	Checkbox.c \
	CollectionItemView.c \
	CollectionView.c \
	Colors.c \
	ColorSelect.c \
	Control.c \
	Font.c \
	Image.c \
	ImageView.c \
	Input.c \
	Label.c \
	NavigationViewController.c \
	Option.c \
	Panel.c \
	Renderer.c \
	ScrollView.c \
	Select.c \
	Slider.c \
	StackView.c \
	TableCellView.c \
	TableColumn.c \
	TableHeaderCellView.c \
	TableHeaderView.c \
	TableRowView.c \
	TableView.c \
	Text.c \
	TextView.c \
	View.c \
	View+JSON.c \
	ViewController.c \
	WindowController.c

libObjectivelyMVC_la_CFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/Sources \
	-DPKGDATADIR=\"$(pkgdatadir)\" \
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
