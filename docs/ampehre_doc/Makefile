#
# Makefile
#
# Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
# All rights reserved.
#
# This software may be modified and distributed under the terms
# of the BSD license. See the LICENSE file for details.
#
# encoding: UTF-8
# tab size: 4
#
# author: Achim Lösch (achim.loesch@upb.de)
# created: 7/20/14
# version: 0.5.8 - change project name to ampehre
#

PAPER=ampehre
LATEX_COMPILER=pdflatex
BIBTEX_COMPILER=bibtex
PDF_VIEWER=okular

all:
	$(LATEX_COMPILER)  $(PAPER)
#	$(BIBTEX_COMPILER) $(PAPER)
	$(LATEX_COMPILER)  $(PAPER)
	$(LATEX_COMPILER)  $(PAPER)

clean:
	rm -f *.bbl *.log *.aux *.blg *.*~ *.dvi *.fdb_latexmk *.toc *.soc *.backup

distclean: clean
	rm -f *.pdf

cleanall: distclean

show:
	$(PDF_VIEWER) $(PAPER).pdf &

