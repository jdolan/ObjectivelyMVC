%.h: %
	xxd -i $^ | awk '/^};/{print "  , 0x00"}1' > $@

Renderer.vert.spv.h: $(srcdir)/Renderer.vert.spv
	xxd -i -t -n Renderer_vert_spv $< > $@

Renderer.vert.msl.h: $(srcdir)/Renderer.vert.msl
	xxd -i -t -n Renderer_vert_msl $< > $@

Renderer.frag.spv.h: $(srcdir)/Renderer.frag.spv
	xxd -i -t -n Renderer_frag_spv $< > $@

Renderer.frag.msl.h: $(srcdir)/Renderer.frag.msl
	xxd -i -t -n Renderer_frag_msl $< > $@

BUILT_SOURCES = \
	check.png.h \
	coda.ttf.h \
	debug.css.h \
	debug.json.h \
	resize.png.h \
	stylesheet.css.h \
	Renderer.vert.spv.h \
	Renderer.vert.msl.h \
	Renderer.frag.spv.h \
	Renderer.frag.msl.h

GLSLC       = glslc
SHADERCROSS = shadercross

GLSL_SHADERS = \
	Renderer.vert.glsl \
	Renderer.frag.glsl

SPV_BLOBS = \
	Renderer.vert.spv \
	Renderer.frag.spv

MSL_BLOBS = \
	Renderer.vert.msl \
	Renderer.frag.msl

EXTRA_DIST = $(GLSL_SHADERS) $(SPV_BLOBS) $(MSL_BLOBS)

# Run 'make shaders' after editing a .glsl file to regenerate the platform blobs.
.PHONY: shaders
shaders: $(SPV_BLOBS) $(MSL_BLOBS)

$(srcdir)/Renderer.vert.spv: $(srcdir)/Renderer.vert.glsl
	$(GLSLC) -fshader-stage=vert $< -o $@

$(srcdir)/Renderer.vert.msl: $(srcdir)/Renderer.vert.spv
	$(SHADERCROSS) $< -s SPIRV -d MSL -t vertex -o $@

$(srcdir)/Renderer.frag.spv: $(srcdir)/Renderer.frag.glsl
	$(GLSLC) -fshader-stage=frag $< -o $@

$(srcdir)/Renderer.frag.msl: $(srcdir)/Renderer.frag.spv
	$(SHADERCROSS) $< -s SPIRV -d MSL -t fragment -o $@
