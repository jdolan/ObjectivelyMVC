# The compiled .spv/.metal blobs are versioned, so normal builds only re-run
# xxd and never need glslc/shadercross. Run 'make shaders' after editing a .glsl.
EXTRA_DIST = \
	Renderer.frag.dxil \
	Renderer.frag.glsl \
	Renderer.frag.metal \
	Renderer.frag.spv \
	Renderer.vert.dxil \
	Renderer.vert.glsl \
	Renderer.vert.metal \
	Renderer.vert.spv \
	check.png \
	coda.ttf \
	debug.css \
	debug.json \
	resize.png \
	stylesheet.css

%.h: %
	xxd -i -t $< > $@

BUILT_SOURCES = \
	Renderer.frag.dxil.h \
	Renderer.frag.metal.h \
	Renderer.frag.spv.h \
	Renderer.vert.dxil.h \
	Renderer.vert.metal.h \
	Renderer.vert.spv.h \
	check.png.h \
	coda.ttf.h \
	debug.css.h \
	debug.json.h \
	resize.png.h \
	stylesheet.css.h

.PHONY: shaders
shaders:
	$(GLSLC) -fshader-stage=frag Renderer.frag.glsl -o Renderer.frag.spv
	$(GLSLC) -fshader-stage=vert Renderer.vert.glsl -o Renderer.vert.spv
	$(SHADERCROSS) Renderer.frag.spv -s SPIRV -d DXIL -t fragment -o Renderer.frag.dxil
	$(SHADERCROSS) Renderer.frag.spv -s SPIRV -d MSL  -t fragment -o Renderer.frag.metal
	$(SHADERCROSS) Renderer.vert.spv -s SPIRV -d DXIL -t vertex   -o Renderer.vert.dxil
	$(SHADERCROSS) Renderer.vert.spv -s SPIRV -d MSL  -t vertex   -o Renderer.vert.metal
