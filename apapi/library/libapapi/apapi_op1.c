/*                                                                                                                                              
 * apapi_op1.c
 * 
 * Copyright (C) 2017, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>, Alex Wiens <awiens@mail.uni-paderborn.de>  * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author: Alex Wiens (awiens@mail.uni-paderborn.de)
 * created: 6/01/17
 * version: 1.0.0 - initial implementation
 */

/** Internal
 *	@class _apapi_exec_op1
 *	@brief process operation on current values to compute derived value
 *
 *	@param enum APAPI_op1 op1
 *		operation to execute
 *	@param long long sample0
 *		previous value
 *	@param long long sample1
 *		current value
 *	@param long long time0
 *		previous timestamp in ns
 *	@param long long time1
 *		current timestamp in ns
 *	@param double *value1
 *		Output parameter, result of the operation
 */
void _apapi_exec_op1(enum APAPI_op1 op1, long long sample0, long long sample1, long long time0, long long time1, long double *value1, int data_type) {

	union {
		long long ll;
		double fp;
	} usample0;
	union {
		long long ll;
		double fp;
	} usample1;

	switch(data_type) {

		case PAPI_DATATYPE_FP64:
			switch(op1) {
				case APAPI_OP1_SAMPLE_DIFF:
					usample0.ll = sample0;
					usample1.ll = sample1;
					*value1 = usample1.fp - usample0.fp;
				break;
				case APAPI_OP1_SAMPLE1_MUL_DIFF_TIME:
					usample1.ll = sample1;
					*value1 = usample1.fp * (long double) (time1 - time0);
				break;
				case APAPI_OP1_AVG_SAMPLE_MUL_DIFF_TIME:
					usample0.ll = sample0;
					usample1.ll = sample1;
					*value1 = (long double)( usample0.fp + usample1.fp ) / 2.0 * (time1 - time0);
				break;
				case APAPI_OP1_DIV_DIFF_TIME:
					usample1.ll = sample1;
					*value1 = usample1.fp / (long double) (time1 - time0);
				break;
				default:
				break;
			}
		break;

		case PAPI_DATATYPE_BIT64:
		case PAPI_DATATYPE_UINT64:
			switch(op1) {
				case APAPI_OP1_SAMPLE_DIFF:
					*value1 = (unsigned long long)sample1 - (unsigned long long)sample0;
				break;
				case APAPI_OP1_SAMPLE1_MUL_DIFF_TIME:
					*value1 = (long double) ((unsigned long long)sample1) * (long double) (time1 - time0);
				break;
				case APAPI_OP1_AVG_SAMPLE_MUL_DIFF_TIME:
					*value1 = ((unsigned long long)sample0 + (unsigned long long)sample1) / 2.0 * (time1 - time0);
				break;
				case APAPI_OP1_DIV_DIFF_TIME:
					*value1 = (long double) ((unsigned long long)sample1) / (long double) (time1 - time0);
				break;
				default:
				break;
			}
		break;

		case PAPI_DATATYPE_INT64:
		default:
			switch(op1) {
				case APAPI_OP1_SAMPLE_DIFF:
					*value1 = sample1 - sample0;
				break;
				case APAPI_OP1_SAMPLE1_MUL_DIFF_TIME:
					*value1 = (long double) sample1 * (long double) (time1 - time0);
				break;
				case APAPI_OP1_AVG_SAMPLE_MUL_DIFF_TIME:
					*value1 = (long double) (sample0 + sample1) / 2.0 * (time1 - time0);
				break;
				case APAPI_OP1_DIV_DIFF_TIME:
					*value1 = (long double) sample1 / (long double) (time1 - time0);
				break;
				default:
				break;
			}
		break;
	}
}

