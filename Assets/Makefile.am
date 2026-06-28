%.h: %
	xxd -i -t $< > $@

BUILT_SOURCES = \
	check.png.h \
	coda.ttf.h \
	debug.css.h \
	debug.json.h \
	resize.png.h \
	stylesheet.css.h

GLSLC       = glslc
SHADERCROSS = shadercross

SHADER_H = \
	Renderer.vert.spv.h \
	Renderer.vert.msl.h \
	Renderer.frag.spv.h \
	Renderer.frag.msl.h

EXTRA_DIST = \
	Renderer.vert.glsl \
	Renderer.frag.glsl \
	$(SHADER_H)

# Run 'make shaders' after editing a .glsl file to regenerate blobs and headers.
.PHONY: shaders
shaders: $(SHADER_H)

Renderer.vert.spv: Renderer.vert.glsl
	$(GLSLC) -fshader-stage=vert $< -o $@

Renderer.vert.msl: Renderer.vert.spv
	$(SHADERCROSS) $< -s SPIRV -d MSL -t vertex -o $@

Renderer.frag.spv: Renderer.frag.glsl
	$(GLSLC) -fshader-stage=frag $< -o $@

Renderer.frag.msl: Renderer.frag.spv
	$(SHADERCROSS) $< -s SPIRV -d MSL -t fragment -o $@
