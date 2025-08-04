
all:
	@echo -n "\nTo build Cadabra, \n\n   mkdir build\n   cd build\n   cmake ..\n   make\n\nThe other targets here are (for maintainer purposes only)\n\n   tarball:     build a tarball cadabra2-latest.tar.gz of current HEAD\n   doc:         generate doxygen docs in doc\n   webup:       build web pages/tutorials/man pages and upload to server\n   updatesnoop: sync snoop repo\n   packages:    create deb/rpm packages on buildbot\n\nIf you need help, email info@cadabra.science\n\n"

.PHONY: doc tarball findclay webbuild claybuild webup format packages updatesnoop

tarball:
	git archive --format=tar --prefix=cadabra2-latest/ HEAD | gzip > ${HOME}/tmp/cadabra2-latest.tar.gz

doc:
	doxygen config/Doxyfile

webbuild:
	cd build; make -f web2/Makefile

claybuild:
	cd web2/cadabra2/source; rm -Rf build; clay build

CMD_NOT_FOUND = $(error $(1) is required for this rule)
CHECK_CMD = $(if $(shell command -v $(1)),,$(call CMD_NOT_FOUND,$(1)))

findclay:
	$(call CHECK_CMD, clay)

webup: findclay webbuild claybuild
	doxygen config/Doxyfile
	rsync -avz --chmod=+rx doxygen/ cadabra_web:/var/www/cadabra2/doxygen/
	cd web2/cadabra2/source; rsync -avz --chmod=+rx build/ cadabra_web:/var/www/cadabra2/;  rsync -avz --chmod=+rx static/styles/ cadabra_web:/var/www/cadabra2/static/styles;   scp static/cadabra_in* cadabra_web:/var/www/cadabra2/static/; rsync -avz --chmod=+rx static/fonts/ cadabra_web:/var/www/cadabra2/static/fonts; rsync -avz --chmod=+rx static/images/ cadabra_web:/var/www/cadabra2/static/images/; rsync -avz --chmod=+rx static/icons/ cadabra_web:/var/www/cadabra2/static/icons/; rsync -avz --chmod=+rx static/pdf/ cadabra_web:/var/www/cadabra2/static/pdf/; rsync -avz --chmod=+rx static/js/ cadabra_web:/var/www/cadabra2/static/js/; rsync -avz --chmod=+r static/robots.txt cadabra_web:/var/www/cadabra2

format:
	astyle --style=k/r --indent=tab=3 --recursive --attach-classes --attach-namespaces --indent-classes --indent-namespaces --indent-switches --break-closing-braces '*.hh'
	astyle --style=k/r --indent=tab=3 --recursive --attach-classes --attach-namespaces --indent-classes --indent-namespaces --indent-switches --break-closing-braces '*.cc'
	find . -name "*.cc" -exec sed -i -e 's/^\([ \t]*\)\([\{\}]\)/\1\t\2/' '{}' ';'
	find . -name "*.hh" -exec sed -i -e 's/^\([ \t]*\)\([\{\}]\)/\1\t\2/' '{}' ';'

packages:
	bash config/buildbot.sh

appimage:
	(mkdir build-appimage; cd build-appimage; cmake -DAPPIMAGE_MODE=ON -DCMAKE_INSTALL_PREFIX=/usr ..; make; make install DESTDIR=AppDir; make appimage)

updatesnoop:
	cp ../snoop/src/Snoop.cc ../snoop/src/SnoopPrivate.hh ../snoop/src/Snoop.hh client_server/
