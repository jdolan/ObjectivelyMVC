
noinst_PROGRAMS = \
	Hello

noinst_HEADERS = \
	HelloViewController.h

Hello_SOURCES = \
	HelloViewController.c \
	Hello.c

CFLAGS += \
	-I$(top_srcdir)/Sources \
	-DEXAMPLES=\"$(abs_srcdir)\" \
	@HOST_CFLAGS@ \
	@FONTCONFIG_CFLAGS@ \
	@OBJECTIVELY_CFLAGS@ \
	@SDL2_CFLAGS@

LDADD = \
	$(top_builddir)/Sources/ObjectivelyMVC/libObjectivelyMVC.la \
	@HOST_LIBS@ \
	@FONTCONFIG_LIBS@ \
	@OBJECTIVELY_LIBS@ \
	@SDL2_LIBS@ \
	-lm
