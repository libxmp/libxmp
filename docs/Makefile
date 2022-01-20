
DOCS_DFILES	= Makefile CMakeLists.txt           \
		  COPYING.LIB CREDITS Changelog     \
		  formats.txt fixloop.txt           \
		  libxmp.rst manpage.rst pdfdoc.rst \
		  custom.style style.css $(DOCS_FILES)

DOCS_PATH	= docs

DOCS_FILES	= libxmp.html libxmp.3 libxmp.pdf

DOCS += $(addprefix $(DOCS_PATH)/,$(DOCS_FILES))

all-docs: $(DOCS)

$(DOCS): docs/Makefile

install-docs: $(DOCS)
	@echo "Installing manpages in $(DESTDIR)$(MANDIR)"
	@[ -d $(DESTDIR)$(MANDIR) ] || mkdir -p $(DESTDIR)$(MANDIR)
	@$(INSTALL_DATA) docs/libxmp.3 $(DESTDIR)$(MANDIR)

dist-docs:
	mkdir -p $(DIST)/$(DOCS_PATH)
	cp -RPp $(addprefix $(DOCS_PATH)/,$(DOCS_DFILES)) $(DIST)/$(DOCS_PATH)

docs/libxmp.3: docs/libxmp.rst docs/manpage.rst
	rst2man docs/manpage.rst > $@

docs/libxmp.html: docs/libxmp.rst docs/pdfdoc.rst docs/style.css
	rst2html docs/pdfdoc.rst --stylesheet docs/style.css > $@

docs/libxmp.pdf: docs/libxmp.rst docs/pdfdoc.rst docs/custom.style
	rst2pdf docs/pdfdoc.rst -c --smart-quotes=1 -s docs/custom.style --footer="###Page###" -o $@

