
noinst_PROGRAMS = \
	HelloMVC

CFLAGS += \
	-I$(top_builddir)/Sources \
	@HOST_CFLAGS@ \
	@FONTCONFIG_CFLAGS@ \
	@OBJECTIVELY_CFLAGS@ \
	@SDL2_CFLAGS@

LDADD = \
	$(top_builddir)/Sources/ObjectivelyMVC/libObjectivelyMVC.la \
	@HOST_LIBS@ \
	@FONTCONFIG_LIBS@ \
	@OBJECTIVELY_LIBS@ \
	@SDL2_LIBS@
