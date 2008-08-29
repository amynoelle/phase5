#
# Simple makefile.
# Uses pdflatex to produce PDF
#
BASE=sigcse2009

$(BASE).pdf:	$(BASE).tex introduction.tex abstract.tex $(BASE).bib principles.tex assignments.tex figures.tex
	pdflatex $(BASE).tex
	-bibtex $(BASE)
	pdflatex $(BASE).tex
	pdflatex $(BASE).tex

