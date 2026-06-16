%.h: %
	xxd -i $^ | awk '/^};/{print "  , 0x00"}1' > $@

BUILT_SOURCES = \
	check.png.h \
	coda.ttf.h \
	debug.css.h \
	debug.json.h \
	renderer_fs.glsl.h \
	renderer_vs.glsl.h \
	resize.png.h \
	stylesheet.css.h
