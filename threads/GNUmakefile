MAIN	     = l2.ltx
SRC	     = minish.c redirsh.c pipesh.c
EXTRADIST    = $(SRC)
EXTRAINSTALL = $(SRC)
EXTRACLEAN   = src.ps src.pdf minish redirsh pipesh
EXTRAIGNORE  = $(EXTRACLEAN)
EXTRAPRINT   = src.pdf

CFLAGS=-Wall -Werror

include ../Beamer.mk

all: minish redirsh pipesh

src.ps: $(SRC)
	enscript -M Letter -1 -p $@ $(SRC)
src.pdf: src.ps
	ps2pdf $<
