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
	Hello-Scene.c \
	HelloViewController.c \
	Hello.c

# All GLSL sources and compiled blobs are versioned in the repository.
# Normal builds never invoke glslc or shadercross.  Run 'make shaders' after
# editing a .glsl file to regenerate the platform blobs.

GLSLC       = glslc
SHADERCROSS = shadercross

GLSL_SHADERS = \
	Hello-Scene.vert.glsl \
	Hello-Scene.frag.glsl

MSL_BLOBS = \
	Hello-Scene.vert.msl \
	Hello-Scene.frag.msl

SPIRV_BLOBS = \
	Hello-Scene.vert.spv \
	Hello-Scene.frag.spv

EXTRA_DIST = $(GLSL_SHADERS) $(MSL_BLOBS) $(SPIRV_BLOBS)

.PHONY: shaders
shaders: $(SPIRV_BLOBS) $(MSL_BLOBS)

$(srcdir)/%.vert.spv: $(srcdir)/%.vert.glsl
	$(GLSLC) -fshader-stage=vert $< -o $@

$(srcdir)/%.vert.msl: $(srcdir)/%.vert.spv
	$(SHADERCROSS) $< -s SPIRV -d MSL -t vertex -o $@

$(srcdir)/%.frag.spv: $(srcdir)/%.frag.glsl
	$(GLSLC) -fshader-stage=frag $< -o $@

$(srcdir)/%.frag.msl: $(srcdir)/%.frag.spv
	$(SHADERCROSS) $< -s SPIRV -d MSL -t fragment -o $@
