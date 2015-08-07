/*
 * CGuiMarker.hpp
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
 * created: 3/06/15
 * version: 0.3.2 - add a networking component to show influence of a task to measurements in GUI
 */

#ifndef __CGUIMARKER_HPP__
#define __CGUIMARKER_HPP__

#include "../common/CThread.hpp"

#include "../../../include/ushmsmonitor.h"

namespace Ui {
	class QMSMMainWindow;
	
	class CGuiMarker : public NCommon::CThread {
		private:
			pthread_t mThreadID;
			bool mThreadStateRun;
			bool mThreadStateStop;
			
			QMSMMainWindow *mpMainWindow;
			
			USHMS_SERVER *mpServer;
			
		public:
			CGuiMarker(QMSMMainWindow *pMainWindow);
			~CGuiMarker(void);
			
		protected:
			void run(void);
			void exit(void);
			
		public:
			void start(void);
			void stop(void);
			void join(void);
			char *getSocketName(void);
			
		private:
			static void* startThread(void* pThreadObject);
	};
}

#endif /* __CGUIMARKER_HPP__ */
