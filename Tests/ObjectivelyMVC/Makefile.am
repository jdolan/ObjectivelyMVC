
AM_TESTS_ENVIRONMENT = \
	AM_TESTS=1; export AM_TESTS;

DEFAULT_INCLUDES = \
	$(top_srcdir)/Sources

TESTS = \
	Font+Bitmap \
	ImageAtlas \
	Selector \
	Style \
	Stylesheet \
	Text \
	View

CFLAGS += \
	-I$(top_srcdir)/Sources \
	@HOST_CFLAGS@ \
	@CHECK_CFLAGS@ \
	@OBJECTIVELY_CFLAGS@ \
	@SDL3_CFLAGS@

LDADD = \
	$(top_srcdir)/Sources/ObjectivelyMVC/libObjectivelyMVC.la \
	@HOST_LIBS@ \
	@CHECK_LIBS@ \
	@OBJECTIVELY_LIBS@ \
	@SDL3_LIBS@

check_PROGRAMS = \
	$(TESTS)
