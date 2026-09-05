AUTOMAKE_OPTIONS = nostdinc
AM_CPPFLAGS = -I$(top_srcdir)/Sources -iquote$(top_builddir)/Sources/ObjectivelyMVC

noinst_HEADERS = \
	Config.h

pkgincludedir = $(includedir)/$(PACKAGE_NAME)

pkginclude_HEADERS = \
	AtlasImage.h \
	Box.h \
	Button.h \
	Checkbox.h \
	CollectionItemView.h \
	CollectionView.h \
	Colors.h \
	Control.h \
	DebugViewController.h \
	Font.h \
	Font+Bitmap.h \
	HSVColorPicker.h \
	HueColorPicker.h \
	Image.h \
	ImageAtlas.h \
	ImageView.h \
	Input.h \
	Text.h \
	Label.h \
	Log.h \
	NavigationViewController.h \
	Option.h \
	PageView.h \
	Panel.h \
	ProgressBar.h \
	Renderer.h \
	RGBColorPicker.h \
	ScrollBar.h \
	ScrollHandle.h \
	ScrollView.h \
	Select.h \
	Selector.h \
	SelectorSequence.h \
	SimpleSelector.h \
	Slider.h \
	SlideShowView.h \
	StackView.h \
	Style.h \
	Stylesheet.h \
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
	Warning.h \
	Window.h \
	WindowController.h

lib_LTLIBRARIES = \
	libObjectivelyMVC.la

libObjectivelyMVC_la_SOURCES = \
	AtlasImage.c \
	Box.c \
	Button.c \
	Checkbox.c \
	CollectionItemView.c \
	CollectionView.c \
	Colors.c \
	Control.c \
	DebugViewController.c \
	Font.c \
	Font+Bitmap.c \
	HSVColorPicker.c \
	HueColorPicker.c \
	Image.c \
	ImageAtlas.c \
	ImageView.c \
	Input.c \
	Label.c \
	NavigationViewController.c \
	Option.c \
	PageView.c \
	Panel.c \
	ProgressBar.c \
	Renderer.c \
	RGBColorPicker.c \
	ScrollBar.c \
	ScrollHandle.c \
	ScrollView.c \
	Select.c \
	Selector.c \
	SelectorSequence.c \
	SimpleSelector.c \
	Slider.c \
	SlideShowView.c \
	StackView.c \
	Style.c \
	Stylesheet.c \
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
	Warning.c \
	Window.c \
	WindowController.c

libObjectivelyMVC_la_CFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/Assets \
	-I$(top_srcdir)/Sources \
	@HOST_CFLAGS@ \
	@OBJECTIVELYGPU_CFLAGS@ \
	@OBJECTIVELY_CFLAGS@ \
	@SDL3_CFLAGS@

libObjectivelyMVC_la_LDFLAGS = \
	-release @RELEASE_VERSION@ \
	@HOST_LDFLAGS@

libObjectivelyMVC_la_LIBADD = \
	@HOST_LIBS@ \
	@OBJECTIVELYGPU_LIBS@ \
	@OBJECTIVELY_LIBS@ \
	@SDL3_DLL_LIBS@
