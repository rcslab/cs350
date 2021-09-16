SRC	     = type.c
EXTRADIST    = $(SRC)
EXTRAINSTALL = $(SRC)
EXTRAPRINT   = src.pdf
EXTRAIGNORE  = src.ps src.pdf
EXTRACLEAN   = src.ps src.pdf

include ../Beamer.mk

src.ps: $(SRC)
	enscript -M Letter -1 -p $@ $(SRC)
src.pdf: src.ps
	ps2pdf $<
