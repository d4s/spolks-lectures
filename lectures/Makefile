TEXFILES=clientserver/clientserver.tex sockets/sockets.tex  spodesign/spodesign.tex  tcpip/tcpip.tex

%.html: %.tex $(TEXFILES)
	latex2html -rootdir HTML -split 4 -html_version 4.0,latin1,unicode $<

%.dvi: %.tex $(TEXFILES)
	latex $<
	latex $<
	latex $<

%.ps: %.dvi
	dvips -j0 -Ppdf -o $@ $<

%.pdf: %.ps
	ps2pdf $<


all: main.pdf

bib: main.aux
	bibtex8 -s main.aux

