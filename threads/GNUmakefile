MAIN	     = l2.ltx
SRC	     = 
EXTRADIST    = $(SRC)
EXTRAINSTALL = $(SRC)
EXTRACLEAN   = src.ps src.pdf
EXTRAIGNORE  = $(EXTRACLEAN)
EXTRAPRINT   = src.pdf

CFLAGS=-Wall -Werror

include ../Beamer.mk

all:

src.ps: $(SRC)
	enscript -M Letter -1 -p $@ $(SRC)
src.pdf: src.ps
	ps2pdf $<
