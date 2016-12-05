/*
 * CRingBuffer.hpp
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
 * author: Christoph Knorr (cknorr@mail.uni-paderborn.de)
 * created: 10/14/15
 * version: 0.5.11 - add option to control the output to csv file and new RingBuffer to store results to msmonitor
 */

#ifndef __CRINGBUFFER_HPP__
#define __CRINGBUFFER_HPP__

#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
namespace NCommon {
	template <class T>
	class CRingBuffer {
		private:
			uint32_t mSize;
			uint32_t mFirst;
			uint32_t mSecond;
			T*		mpData;
			
		public:
			CRingBuffer(int size) {
				mSize = size;
		
				mpData = (T*) malloc(2 * size * sizeof(T));
				reset();
			}
			
			~CRingBuffer() {
				free(mpData);
			}
			
			void push_back(T rData) {
				mpData[mFirst] = rData;
				mpData[mSecond] = rData;
		
				mFirst = (mFirst+1 == mSize) ? 0 : mFirst+1;
				mSecond = mSize + mFirst;
			}
			
			void reset() {
				mFirst 	=	0;
				mSecond =	mSize;
		
				std::memset(mpData, 0, 2 * mSize * sizeof(T));
			}
			
			T* getDataPtr() {
				return mpData + mFirst+1;
			}
			
			const T& getLast() {
				if(mFirst==0) {
					return mpData[mSize-1];
				} else {
					return mpData[mFirst-1];
				}
			}
	};
}
#endif /* __CRINGBUFFER_HPP__ */
