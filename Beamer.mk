# 
# Basic strategy is that binary figures (that can't be re-generated)
# go in the LECTURE/figs/ subdirectory and get added to the git
# repository.  Everything else lives in the LECTURE/ subdirectory (for
# each LECTURE).
#

#
# Note:  some of the svg images assume that the computer modern tt
# 12-pt font is available.  Install it by copying cmtt12.pfb in
# directory ~/.fonts and running fc-cache.  (cmtt12.pfb is part of
# texlive, you can find it in a place like:
#
#   .../texmf-dist/fonts/type1/public/amsfonts/cm/cmtt12.pfb
#

BASE := $(notdir $(PWD))
PARENT := ..

all: $(BASE).pdf
.PHONY: all

SVGFIGS  := $(wildcard *.svg)
XFIGS    := $(wildcard *.fig)
EPSONLY  := $(wildcard figs/*.eps)
PDFONLY  := $(filter-out %-eps-converted-to.pdf, $(wildcard figs/*.pdf))

GENFIGS  := $(patsubst %.svg,%.pdf, $(SVGFIGS)) \
            $(patsubst %.fig,%.pdf, $(XFIGS)) \
            $(patsubst %.fig,%.tex, $(XFIGS)) \
	    $(patsubst %.eps,%-eps-converted-to.pdf, $(EPSONLY))
DEPFIGS  := $(filter-out %-eps-converted-to.pdf, $(GENFIGS)) \
	    $(EPSONLY) $(PDFONLY)

DIST      = Beamer.mk lectureb.sty beamerthemelectureb.sty \
	    $(addprefix $(BASE)/, GNUmakefile $(BASE).tex $(EXTRADIST) \
	    $(SVGFIGS) $(XFIGS) $(EPSONLY) $(PDFONLY))

$(BASE).pdf: $(DEPFIGS)
$(BASE).pdf: $(PARENT)/lectureb.sty $(PARENT)/beamerthemelectureb.sty

%.pdf: %.svg
	inkscape --export-area-snap --export-area-drawing --export-pdf=$@ $<

%.pdf: %.fig
	fig2dev -L pdftex -p1 $< > $*.pdf
%.tex: %.fig
	fig2dev -L pdftex_t -p $*.pdf $< > $*.tex

IGNORE = $(BASE).pdf $(GENFIGS) $(IMAGES) *~ .xpdf-running \
         *.aux *.dvi *-eps-converted-to.pdf *.fdb_latexmk *.fls *.log \
	 *.nav *.out *.snm *.toc *.vrb *.synctex.gz \
	 $(EXTRAIGNORE) $(BASE).tar.gz

ignore:
	rm -f .gitignore
	set -f; for file in $(sort $(IGNORE)); do \
	    echo $$file >> .gitignore; done

%.pdf: %.tex $(glob *.sty)
	touch $*.out
	TEXINPUTS="$(PARENT):$$TEXINPUTS" \
	    latexmk -lualatex -latexoption=-halt-on-error \
		-latexoption=-file-line-error \
		-latexoption=-synctex=1 $< \
		&& touch $*.dvi || ! rm -f $@
	if test -f .xpdf-running; then \
		xpdf -remote $(BASE)-server -reload; \
	else \
		pkill -HUP mupdf || :; \
	fi

define PDF6UP
$(1)-print.tex: $(MAKEFILE_LIST) $(EXTRAPRINT)
	rm -f $$@~
	printf "%s\n" '\batchmode' > $$@~
	printf "%s\n" '\documentclass[letterpaper]{article}' >> $$@~
	printf "%s\n" '\usepackage[utf8]{inputenc}' >> $$@~
	printf "%s\n" '\usepackage{pdfpages}' >> $$@~
	printf "%s\n" '\footskip 2.7cm' >> $$@~
	printf "%s\n" '\begin{document}' >> $$@~
	printf "%s\n" '\includepdf[pages=-,offset=0in 0in,nup=2x3,frame=false,noautoscale=false,delta=0.2cm 0.3cm,scale=0.95,pagecommand={\thispagestyle{empty}}]{$(1).pdf}' >> $$@~
	for file in $(EXTRAPRINT); do \
		printf "%s\n" "\includepdf[pages=-]{$$$$file}" >> $$@~; \
	done
	printf "%s\n" '\end{document}' >> $$@~
	mv -f $$@~ $$@

$(1)-print.pdf: $(1)-print.tex $(1).pdf
	pdflatex $$<

EXTRACLEAN += $(1)-print.*
EXTRAIGNORE += '$(1)-print.*'
endef

$(eval $(call PDF6UP,$(BASE)))
.PHONY: print
print: $(BASE)-print.pdf

dist: $(BASE).tar.gz
.PHONY: dist
$(BASE).tar.gz: $(addprefix ../,$(DIST))
	tar -czf $@ -C ../.. $(addprefix notes/,$(DIST))


ifneq ($(origin NOADD),undefined)
GITADD       = : git add
else
GITADD       = git add
endif

DESTDIR=../../www/notes
install: $(BASE).pdf $(BASE).tar.gz $(BASE)-print.pdf $(EXTRAINSTALL)
	mkdir -p $(DESTDIR)
	topic=`sed -ne 's/^Lecture Topic: *//p' $(BASE).log` \
		&& test -n "$$topic" \
		&& ../genindex $(DESTDIR)/index.html $(BASE) "$$topic" \
			$(EXTRAINSTALL)
	cp $^ $(DESTDIR)/
	$(GITADD) $(patsubst %, $(DESTDIR)/%, $^) $(DESTDIR)/index.html

#
# Make random-ish 6-hex-digit \extensioncode value by taking the first
# three bytes (6 hex characters) of the SHA1 hash of a bunch of system
# state.  (extensioncode must be manually added to the git repository)
#
extensioncode:
	(uptime; ps auxwww; netstat -a; netstat -s) \
		| (sha1 || sha1sum) 2> /dev/null \
		| cut -c 1-6 \
		| xargs printf '\\newcommand{\\theextensioncode}{%s}%%\n' > $@
$(BASE)-code.tex:
	echo '\def\exposeextensioncode{}\input{$(BASE).tex}' > $@
$(BASE)-code.pdf: $(BASE)-code.tex $(BASE).pdf
	TEXINPUTS="$(PARENT):$$TEXINPUTS" pdflatex $<

ifeq ($(NEEDCODE), yes)
$(BASE).pdf: extensioncode
all: $(BASE)-code.pdf
$(eval $(call PDF6UP,$(BASE)-code))
print: $(BASE)-code-print.pdf
EXTRACLEAN += $(BASE)-code.*
EXTRAIGNORE += $(BASE)-code.*
endif

.PHONY: clean
clean:
	latexmk -C $(BASE).tex
	rm -f $(GENFIGS) $(EXTRACLEAN)
	rm -f *~ *.dvi *.vrb *.nav *.snm texput.* *.synctex.gz

.PHONY: cleanse
cleanse:
	latexmk -C $(BASE).tex
	rm -f *~ *.dvi *.vrb *.nav *.snm texput.*

.PHONY: preview
preview: $(BASE).pdf
	if test -f .xpdf-running; then			\
		xpdf -remote $(BASE)-server -quit;	\
		! test -f .xpdf-running || sleep 0.1;	\
		! test -f .xpdf-running || sleep 1;	\
	fi
	touch .xpdf-running
	(xpdf -remote $(BASE)-server $(BASE).pdf; rm -f .xpdf-running) &
