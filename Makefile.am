ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
	Assets \
	Sources \
	Tests \
	Examples

# Requires: git clone --depth 1 https://github.com/jothepro/doxygen-awesome-css.git doxygen-awesome-css
html:
	doxygen
	install -m 644 Documentation/*.{gif,jpg} Documentation/html

html-clean:
	rm -rf Documentation/html
