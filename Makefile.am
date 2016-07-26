ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
	Assets \
	Sources \
	Examples

html:
	doxygen
	install -m 644 Documentation/*.gif Documentation/*.jpg Documentation/html

html-clean:
	rm -rf Documentation/html

html-release:
	rsync -rz --delete Documentation/html/ jaydolan.com:/var/www/projects/ObjectivelyMVC
