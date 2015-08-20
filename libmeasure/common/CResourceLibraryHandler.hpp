/*
 * CResourceLibraryHandler.hpp
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
 * author: Christoph Knorr (cknorr@mail.upb.de)
 * created: 6/11/15
 * version: 0.5.5 - add ResourceLibraryHandler to hide specific libraries in CMgmt
 */

#ifndef __CRESOURCELIBRARYHANDLER_HPP__
#define __CRESOURCELIBRARYHANDLER_HPP__

#include <dlfcn.h>

#include "CMeasureAbstractResource.hpp"
#include "CMeasureAbstractThread.hpp"

namespace NLibMeasure {
	class CResourceLibraryHandler{
		public:
			CResourceLibraryHandler(CLogger& rLogger,const std::string& rLibname, uint64_t *pParams);
			~CResourceLibraryHandler();
			
			void* loadFunction(const std::string& rFunctionname);
			
			void initResourceThread(CSemaphore& rStartSem, MEASUREMENT* pMeasurement);
			CMeasureAbstractThread* getResourceThread();
			void finiResourceThread();
			
			CMeasureAbstractResource* getResource();
			
			CMutex& getMutexStartResource();
			
		private:
			void* mpLibhandler;
			CMeasureAbstractResource* mpResource;
			CMeasureAbstractThread* mpResourceThread;
			CMutex mMutexStartResource;
			CLogger& mrLog;
			
		private:
			void initResource(uint64_t *pParams);
			void finiResource();
			void* openLibrary(const std::string& rLibname);
			void closeLibrary(void **handler);
	};
}

#endif /* __CRESOURCELIBRARYHANDLER_HPP__ */
