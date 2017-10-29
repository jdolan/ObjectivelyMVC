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
	Config.h \
	Constraint.h \
	Control.h \
	Font.h \
	HSVColorPicker.h \
	HueColorPicker.h \
	Image.h \
	ImageView.h \
	Input.h \
	Text.h \
	Label.h \
	Log.h \
	NavigationViewController.h \
	Notification.h \
	Option.h \
	PageView.h \
	Panel.h \
	ProgressBar.h \
	Renderer.h \
	RGBColorPicker.h \
	ScrollView.h \
	Select.h \
	Selector.h \
	SelectorSequence.h \
	SimpleSelector.h \
	Slider.h \
	StackView.h \
	Style.h \
	TableCellView.h \
	TableColumn.h \
	TableHeaderCellView.h \
	TableHeaderView.h \
	TableRowView.h \
	TableView.h \
	TabView.h \
	TabViewController.h \
	TabViewItem.h \
	TextView.h \
	Theme.h \
	Types.h \
	View.h \
	View+JSON.h \
	ViewController.h \
	Window.h \
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
	Constraint.c \
	Control.c \
	Font.c \
	HSVColorPicker.c \
	HueColorPicker.c \
	Image.c \
	ImageView.c \
	Input.c \
	Label.c \
	NavigationViewController.c \
	Notification.c \
	Option.c \
	PageView.c \
	Panel.c \
	ProgressBar.c \
	Renderer.c \
	RGBColorPicker.c \
	ScrollView.c \
	Select.c \
	Selector.c \
	SelectorSequence.c \
	SimpleSelector.c \
	Slider.c \
	StackView.c \
	Style.c \
	TableCellView.c \
	TableColumn.c \
	TableHeaderCellView.c \
	TableHeaderView.c \
	TableRowView.c \
	TableView.c \
	TabView.c \
	TabViewController.c \
	TabViewItem.c \
	Text.c \
	TextView.c \
	Theme.c \
	View.c \
	View+JSON.c \
	ViewController.c \
	Window.c \
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
