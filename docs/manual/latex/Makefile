#
# Makefile
# $Log$
# Revision 1.2  2006/12/21 08:08:00  andreas
# kickstart updated
#
# Revision 1.1  2006/12/06 20:29:46  andreas
# forgot some files at the last check in
#
# Revision 1.1  2005/01/18 19:51:19  georgs
# added kfi paper to doc/investigation/kfi-paper/
# sources are in doc/investigation/kfi/...
# furthermore i updated the week report
#
#


# Makefile for generating PDF documents


RM = rm -f
PDFLATEX = $(SETVARS) pdflatex
BIBTEX = bibtex
PDF2PS = pdf2ps
GZ = gzip
BZIP2 = bzip2 -f -k

PDFFILES=$(subst .tex,.pdf,$(TEXFILES))
PSFILES=$(subst .tex,.ps,$(TEXFILES))
PSGZFILES=$(subst .tex,.ps.gz,$(TEXFILES))
PSBZ2FILES=$(subst .tex,.ps.bz2,$(TEXFILES))

all: $(PDFFILES)

ps: $(PSFILES)

html: $(HTMLFILES)

psgz: $(PSGZFILES)

psbzip2: $(PSBZIP2FILES)

%.pdf: %.tex $(DEPS)
	$(PDFLATEX) $*.tex
	$(PDFLATEX) $*.tex

%.ps: %.pdf $(DEPS)
	$(PDF2PS) $*.pdf $*.ps

%.gz: % $(DEPS)
	$(GZ) < $* > $*.gz

%.bz2: % $(DEPS)
	$(BZIP2) $*

bib: $(PDFFILES) $(DEPS)
	$(PDFLATEX) $(TEXFILES)
	$(BIBTEX) $(BIBFILE)
	$(PDFLATEX) $(TEXFILES)
	$(PDFLATEX) $(TEXFILES)

clean:
	$(RM) *.log *.blg *.ilg *.aux *.lof *.lot *.toc *.idx *.ttt \
	*.fff *.out *~ *.bbl *.pdf

strip: clean
	$(RM) *.bbl *.ind *.dvi

realclean: strip
	$(RM) *.ps

distclean: realclean
	$(RM) *.ps.gz *.ps.bz2 *.pdf

