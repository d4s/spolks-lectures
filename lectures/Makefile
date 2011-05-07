%.dvi: %.tex
	latex $<
	latex $<
	latex $<

%.ps: %.dvi
	dvips -j0 -Ppdf -o $@ $<

%.pdf: %.ps
	ps2pdf $<

%.html: %.tex
	latex2html -split 3 -html_version 4.0,latin1,unicode main.tex

all: main.pdf


