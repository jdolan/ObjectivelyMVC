
AM_TESTS_ENVIRONMENT = \
	AM_TESTS=1; export AM_TESTS;

DEFAULT_INCLUDES = \
	$(top_srcdir)/Sources

TESTS = \
	Constraint \
	Selector \
	Style

CFLAGS += \
	-I$(top_srcdir)/Sources \
	@HOST_CFLAGS@ \
	@CHECK_CFLAGS@

LDADD = \
	$(top_srcdir)/Sources/ObjectivelyMVC/libObjectivelyMVC.la \
	@HOST_LIBS@ \
	@CHECK_LIBS@

check_PROGRAMS = \
	$(TESTS)
