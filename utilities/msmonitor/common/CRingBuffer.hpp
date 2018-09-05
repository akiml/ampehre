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
		
				mpData = (T*) malloc(3 * size * sizeof(T));
				reset();
			}
			
			~CRingBuffer() {
				free(mpData);
			}
			
			void push_back(T rData) {
				mpData[mSecond] = rData;
				if (mSecond >= mSize+mSize) {
					mpData[mFirst] = rData;
				}
				mSecond = (mSecond+1 == mSize+mSize+mSize) ? mSize : mSecond+1;
				mFirst = mSecond-mSize-mSize;
			}
			
			void reset() {
				mFirst 	=	0;
				mSecond =	mSize+mSize;
		
				std::memset(mpData, 0, 3 * mSize * sizeof(T));
			}
			
			T* getDataPtr() {
				return mpData + mSecond - mSize;
			}
			
			const T& getLast() {
				return mpData[mSecond-1];
			}
	};
}
#endif /* __CRINGBUFFER_HPP__ */
