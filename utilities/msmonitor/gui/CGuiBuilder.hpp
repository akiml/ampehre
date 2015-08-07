/*
 * CGuiBuilder.hpp
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

#ifndef __CGUIBUILDER_HPP__
#define __CGUIBUILDER_HPP__

#include "../data/CDataHandler.hpp"

class QApplication;

namespace Ui {
	class QMSMMainWindow;
	
	class CGuiBuilder {
		private:
			QApplication* mpApplication;
			QMSMMainWindow* mpMainWindow;
			NData::CDataHandler& mrDataHandler;
			
		public:
			CGuiBuilder(NData::CDataHandler& rDataHandler, int& argc, char **argv);
			~CGuiBuilder(void);
			
			void showApplication(void);
	};
}

#endif /* __QMSMGUIBUILDER_HPP__ */
