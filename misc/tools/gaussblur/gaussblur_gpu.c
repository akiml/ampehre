/*
 * gaussblur_gpu.c
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
 * created: 1/11/15
 * version: 0.2.5 - add gaussblur example application
 */

#include "gaussblur_gpu.h"

void gaussblur_calc_gpu_compute(uint16_t *image_in, uint16_t *image_out, uint32_t N_GAUSSBLUR_var) {
	int32_t i, j, pos;
	int32_t columns	= N_GAUSSBLUR_var;
	int32_t rows	= N_GAUSSBLUR_var;
	
	PixelPacket *pixpack_source = (PixelPacket *)image_in;
	PixelPacket *pixpack_target = (PixelPacket *)image_out;
	
	#pragma acc kernels deviceptr(pixpack_source, pixpack_target)
	{
		#pragma acc loop independent
		for (i=2; i<rows-2; ++i) {
			int32_t temp_red = 0, temp_green = 0, temp_blue = 0, temp_opacity = 0;
			#pragma acc loop independent private(pos, temp_red, temp_green, temp_blue, temp_opacity)
			for (j=2; j<columns-2; ++j) {
				pos = i*columns+j;
				temp_red		=	   pixpack_source[pos-(2*columns)-2].red
								+	 5*pixpack_source[pos-(2*columns)-1].red
								+	 7*pixpack_source[pos-(2*columns)  ].red
								+	 5*pixpack_source[pos-(2*columns)+1].red
								+	   pixpack_source[pos-(2*columns)+2].red;
				temp_green		=	   pixpack_source[pos-(2*columns)-2].green
								+	 5*pixpack_source[pos-(2*columns)-1].green
								+	 7*pixpack_source[pos-(2*columns)  ].green
								+	 5*pixpack_source[pos-(2*columns)+1].green
								+	   pixpack_source[pos-(2*columns)+2].green;
				temp_blue		=	   pixpack_source[pos-(2*columns)-2].blue
								+	 5*pixpack_source[pos-(2*columns)-1].blue
								+	 7*pixpack_source[pos-(2*columns)  ].blue
								+	 5*pixpack_source[pos-(2*columns)+1].blue
								+	   pixpack_source[pos-(2*columns)+2].blue;
				temp_opacity	=	   pixpack_source[pos-(2*columns)-2].opacity
								+	 5*pixpack_source[pos-(2*columns)-1].opacity
								+	 7*pixpack_source[pos-(2*columns)  ].opacity
								+	 5*pixpack_source[pos-(2*columns)+1].opacity
								+	   pixpack_source[pos-(2*columns)+2].opacity;
				
				temp_red		+=	 5*pixpack_source[pos-(  columns)-2].red
								+	20*pixpack_source[pos-(  columns)-1].red
								+	33*pixpack_source[pos-(  columns)  ].red
								+	20*pixpack_source[pos-(  columns)+1].red
								+	 5*pixpack_source[pos-(  columns)+2].red;
				temp_green		+=	 5*pixpack_source[pos-(  columns)-2].green
								+	20*pixpack_source[pos-(  columns)-1].green
								+	33*pixpack_source[pos-(  columns)  ].green
								+	20*pixpack_source[pos-(  columns)+1].green
								+	 5*pixpack_source[pos-(  columns)+2].green;
				temp_blue		+=	 5*pixpack_source[pos-(  columns)-2].blue
								+	20*pixpack_source[pos-(  columns)-1].blue
								+	33*pixpack_source[pos-(  columns)  ].blue
								+	20*pixpack_source[pos-(  columns)+1].blue
								+	 5*pixpack_source[pos-(  columns)+2].blue;
				temp_opacity	+=	 5*pixpack_source[pos-(  columns)-2].opacity
								+	20*pixpack_source[pos-(  columns)-1].opacity
								+	33*pixpack_source[pos-(  columns)  ].opacity
								+	20*pixpack_source[pos-(  columns)+1].opacity
								+	 5*pixpack_source[pos-(  columns)+2].opacity;
				
				temp_red		+=	 7*pixpack_source[pos-2].red
								+	33*pixpack_source[pos-1].red
								+	55*pixpack_source[pos  ].red
								+	33*pixpack_source[pos+1].red
								+	 7*pixpack_source[pos+2].red;
				temp_green		+=	 7*pixpack_source[pos-2].green
								+	33*pixpack_source[pos-1].green
								+	55*pixpack_source[pos  ].green
								+	33*pixpack_source[pos+1].green
								+	 7*pixpack_source[pos+2].green;
				temp_blue		+=	 7*pixpack_source[pos-2].blue
								+	33*pixpack_source[pos-1].blue
								+	55*pixpack_source[pos  ].blue
								+	33*pixpack_source[pos+1].blue
								+	 7*pixpack_source[pos+2].blue;
				temp_opacity	+=	 7*pixpack_source[pos-2].opacity
								+	33*pixpack_source[pos-1].opacity
								+	55*pixpack_source[pos  ].opacity
								+	33*pixpack_source[pos+1].opacity
								+	 7*pixpack_source[pos+2].opacity;
				
				temp_red		+=	 5*pixpack_source[pos+(  columns)-2].red
								+	20*pixpack_source[pos+(  columns)-1].red
								+	33*pixpack_source[pos+(  columns)  ].red
								+	20*pixpack_source[pos+(  columns)+1].red
								+	 5*pixpack_source[pos+(  columns)+2].red;
				temp_green		+=	 5*pixpack_source[pos+(  columns)-2].green
								+	20*pixpack_source[pos+(  columns)-1].green
								+	33*pixpack_source[pos+(  columns)  ].green
								+	20*pixpack_source[pos+(  columns)+1].green
								+	 5*pixpack_source[pos+(  columns)+2].green;
				temp_blue		+=	 5*pixpack_source[pos+(  columns)-2].blue
								+	20*pixpack_source[pos+(  columns)-1].blue
								+	33*pixpack_source[pos+(  columns)  ].blue
								+	20*pixpack_source[pos+(  columns)+1].blue
								+	 5*pixpack_source[pos+(  columns)+2].blue;
				temp_opacity	+=	 5*pixpack_source[pos+(  columns)-2].opacity
								+	20*pixpack_source[pos+(  columns)-1].opacity
								+	33*pixpack_source[pos+(  columns)  ].opacity
								+	20*pixpack_source[pos+(  columns)+1].opacity
								+	 5*pixpack_source[pos+(  columns)+2].opacity;
				
				temp_red		+=	   pixpack_source[pos+(2*columns)-2].red
								+	 5*pixpack_source[pos+(2*columns)-1].red
								+	 7*pixpack_source[pos+(2*columns)  ].red
								+	 5*pixpack_source[pos+(2*columns)+1].red
								+	   pixpack_source[pos+(2*columns)+2].red;
				temp_green		+=	   pixpack_source[pos+(2*columns)-2].green
								+	 5*pixpack_source[pos+(2*columns)-1].green
								+	 7*pixpack_source[pos+(2*columns)  ].green
								+	 5*pixpack_source[pos+(2*columns)+1].green
								+	   pixpack_source[pos+(2*columns)+2].green;
				temp_blue		+=	   pixpack_source[pos+(2*columns)-2].blue
								+	 5*pixpack_source[pos+(2*columns)-1].blue
								+	 7*pixpack_source[pos+(2*columns)  ].blue
								+	 5*pixpack_source[pos+(2*columns)+1].blue
								+	   pixpack_source[pos+(2*columns)+2].blue;
				temp_opacity	+=	   pixpack_source[pos+(2*columns)-2].opacity
								+	 5*pixpack_source[pos+(2*columns)-1].opacity
								+	 7*pixpack_source[pos+(2*columns)  ].opacity
								+	 5*pixpack_source[pos+(2*columns)+1].opacity
								+	   pixpack_source[pos+(2*columns)+2].opacity;
				
				pixpack_target[pos].red		= temp_red		/ 339;
				pixpack_target[pos].green	= temp_green	/ 339;
				pixpack_target[pos].blue	= temp_blue		/ 339;
				pixpack_target[pos].opacity	= temp_opacity	/ 339;
			}
		}
	}
}
