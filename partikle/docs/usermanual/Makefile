##
## Makefile
##  
## Made by Ismael Ripoll
## Login   <iripollfredes.disca.upv.es>
##
## Started on  Fri Nov 17 11:42:24 2006 Ismael Ripoll
## Last update Fri Nov 17 11:45:51 2006 Ismael Ripoll
## 
SOURCES= partikle_usermanual.tex
TARGETS= $(patsubst %.tex, %.pdf, $(SOURCES))

all: $(TARGETS)
%.pdf: %.tex
	pdflatex $<
	pdflatex $<

distclean: clean
clean:
	rm -rf $(TARGETS) *.aux *.log *.out *.toc *~
