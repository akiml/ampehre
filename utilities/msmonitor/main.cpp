/*
 * main.cpp
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

#include <cstdlib>

#include "data/CDataHandler.hpp"
#include "gui/CGuiBuilder.hpp"

int main(int argc, char **argv) {
	NData::CDataHandler dataHandler;
	Ui::CGuiBuilder gui(dataHandler, argc, argv);
	
	gui.showApplication();
	
	return EXIT_SUCCESS;
}