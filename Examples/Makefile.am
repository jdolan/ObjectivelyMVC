%.wav.h: %
	xxd -i -s 44 $^ $@

BUILT_SOURCES = \
	click.wav.h \
	clack.wav.h

noinst_HEADERS = \
	HelloViewController.h

noinst_PROGRAMS = \
	Hello

CFLAGS += \
	-I$(top_srcdir)/Assets \
	-I$(top_srcdir)/Sources \
	-DEXAMPLES=\"$(abs_srcdir)\" \
	@HOST_CFLAGS@ \
	@OBJECTIVELY_CFLAGS@ \
	@SDL3_CFLAGS@

LDADD = \
	$(top_builddir)/Sources/ObjectivelyMVC/libObjectivelyMVC.la \
	@HOST_LIBS@ \
	@OBJECTIVELY_LIBS@ \
	@SDL3_LIBS@ \
	-lm

Hello_SOURCES = \
	HelloViewController.c \
	Hello.c
