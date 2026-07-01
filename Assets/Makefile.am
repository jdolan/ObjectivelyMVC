# The compiled .spv/.metal blobs are versioned, so normal builds only re-run
# xxd and never need glslc/shadercross. Run 'make shaders' after editing a .glsl.
EXTRA_DIST = \
	Renderer.vert.glsl \
	Renderer.frag.glsl \
	Renderer.vert.metal \
	Renderer.frag.metal \
	Renderer.vert.spv \
	Renderer.frag.spv \
	check.png \
	coda.ttf \
	debug.css \
	debug.json \
	resize.png \
	stylesheet.css

%.h: %
	xxd -i -t $< > $@

BUILT_SOURCES = \
	Renderer.vert.metal.h \
	Renderer.frag.metal.h \
	Renderer.vert.spv.h \
	Renderer.frag.spv.h \
	check.png.h \
	coda.ttf.h \
	debug.css.h \
	debug.json.h \
	resize.png.h \
	stylesheet.css.h

.PHONY: shaders
shaders:
	$(GLSLC) -fshader-stage=vert Renderer.vert.glsl -o Renderer.vert.spv
	$(GLSLC) -fshader-stage=frag Renderer.frag.glsl -o Renderer.frag.spv
	$(SHADERCROSS) Renderer.vert.spv -s SPIRV -d MSL -t vertex -o Renderer.vert.metal
	$(SHADERCROSS) Renderer.frag.spv -s SPIRV -d MSL -t fragment -o Renderer.frag.metal
