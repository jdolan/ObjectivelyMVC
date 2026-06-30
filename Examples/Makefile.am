noinst_HEADERS = \
	HelloViewController.h

noinst_PROGRAMS = \
	Hello

CFLAGS += \
	-I$(top_srcdir)/Assets \
	-I$(top_srcdir)/Sources \
	-DEXAMPLES=\"$(abs_srcdir)\" \
	@HOST_CFLAGS@ \
	@OBJECTIVELYGPU_CFLAGS@ \
	@OBJECTIVELY_CFLAGS@ \
	@SDL3_CFLAGS@

LDADD = \
	$(top_builddir)/Sources/ObjectivelyMVC/libObjectivelyMVC.la \
	@HOST_LIBS@ \
	@OBJECTIVELYGPU_LIBS@ \
	@OBJECTIVELY_LIBS@ \
	@SDL3_LIBS@ \
	-lm

Hello_SOURCES = \
	HelloViewController.c \
	Hello.c

# Runtime assets loaded via the Resource API (from the EXAMPLES path), plus the
# versioned shader sources/blobs. Normal builds never need glslc/shadercross;
# run 'make shaders' after editing a .glsl.
EXTRA_DIST = \
	Hello.vert.glsl \
	Hello.frag.glsl \
	Hello.vert.metal \
	Hello.frag.metal \
	Hello.vert.spv \
	Hello.frag.spv \
	click.wav \
	clack.wav


.PHONY: shaders
shaders:
	$(GLSLC) -fshader-stage=vert Hello.vert.glsl -o Hello.vert.spv
	$(GLSLC) -fshader-stage=frag Hello.frag.glsl -o Hello.frag.spv
	$(SHADERCROSS) Hello.vert.spv -s SPIRV -d MSL -t vertex -o Hello.vert.metal
	$(SHADERCROSS) Hello.frag.spv -s SPIRV -d MSL -t fragment -o Hello.frag.metal
