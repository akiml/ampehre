void exec_op1(enum APAPI_op1 op1, long long sample0, long long sample1, long long time0, long long time1, double *value1){

    switch(op1) {
        case OP1_SAMPLE_DIFF:
            *value1 = sample1 - sample0;
        break;
        case OP1_SAMPLE1_MUL_DIFF_TIME:
            *value1 = sample1 * (time1 - time0);
        break;
        case OP1_AVG_SAMPLE_MUL_DIFF_TIME:
            *value1 = (sample0 + sample1) / 2.0 * (time1 - time0);
        break;
        case OP1_DIV_DIFF_TIME:
            *value1 = sample1 / (time1 - time0);
        break;
        default:
        break;
    }
}

