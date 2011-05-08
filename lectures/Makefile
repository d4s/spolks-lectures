TEXFILES=clientserver.tex sockets.tex  spodesign.tex  tcpip.tex

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


