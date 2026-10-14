FILE = main
LATEX = pdflatex
FLAGS = -interaction=nonstopmode -file-line-error

all: $(FILE).pdf

$(FILE).pdf: $(FILE).tex
	-$(LATEX) $(FLAGS) $(FILE).tex
	-$(LATEX) $(FLAGS) $(FILE).tex

clean:
	rm -f *.aux *.log *.out *.toc *.fls *.fdb_latexmk *.bbl *.bcf *.dvi *.blg *.run.xml $(FILE).pdf
