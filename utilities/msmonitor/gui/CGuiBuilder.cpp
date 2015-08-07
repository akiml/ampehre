/*
 * CGuiBuilder.cpp
 * 
 * Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author: Achim Lösch (achim.loesch@upb.de)
 * created: 1/16/14
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 */

#include "CGuiBuilder.hpp"

#include "QMSMMainWindow.hpp"
#include <QApplication>

namespace Ui {
	CGuiBuilder::CGuiBuilder(NData::CDataHandler& rDataHandler, int& argc, char **argv) :
		mpApplication(0),
		mpMainWindow(0),
		mrDataHandler(rDataHandler)
		{
		
		mpApplication	= new QApplication(argc, argv);
		mpMainWindow	= new QMSMMainWindow(0, mrDataHandler, mpApplication);
	}

	CGuiBuilder::~CGuiBuilder(void) {
		delete mpMainWindow;
		delete mpApplication;
	}
	
	void CGuiBuilder::showApplication(void) {
		mpMainWindow->show();
		mpMainWindow->setVisible(true);
		mpApplication->exec();
	}
}
