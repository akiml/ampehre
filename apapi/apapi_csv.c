int apapi_parse_op1(char* input, enum APAPI_op1 *output) {
    if (input == NULL || input[0] == 0) {
        return -1;
    }
    if (strcmp(input, "APAPI_OP1_NOP") == 0) {
        *output = APAPI_OP1_NOP;
        return PAPI_OK;
    }
    if (strcmp(input, "APAPI_OP1_SAMPLE_DIFF") == 0) {
        *output = APAPI_OP1_SAMPLE_DIFF;
        return PAPI_OK;
    }
    if (strcmp(input, "APAPI_OP1_DIV_DIFF_TIME") == 0) {
        *output = APAPI_OP1_DIV_DIFF_TIME;
        return PAPI_OK;
    }
    if (strcmp(input, "APAPI_OP1_SAMPLE1_MUL_DIFF_TIME") == 0) {
        *output = APAPI_OP1_SAMPLE1_MUL_DIFF_TIME;
        return PAPI_OK;
    }
    if (strcmp(input, "APAPI_OP1_AVG_SAMPLE_MUL_DIFF_TIME") == 0) {
        *output = APAPI_OP1_AVG_SAMPLE_MUL_DIFF_TIME;
        return PAPI_OK;
    }
    return -1;
}

int apapi_parse_stats(char* input, enum APAPI_stats *output, int only_single_attribute) {
    if (input == NULL || input[0] == 0) {
        return -1;
    }
    // single attribute
    if (strcmp(input, "APAPI_STAT_NO") == 0) {
        *output = APAPI_STAT_NO;
        return PAPI_OK;
    }
    if (strcmp(input, "APAPI_STAT_MIN") == 0) {
        *output = APAPI_STAT_MIN;
        return PAPI_OK;
    }
    if (strcmp(input, "APAPI_STAT_MAX") == 0) {
        *output = APAPI_STAT_MAX;
        return PAPI_OK;
    }
    if (strcmp(input, "APAPI_STAT_AVG") == 0) {
        *output = APAPI_STAT_AVG;
        return PAPI_OK;
    }
    if (strcmp(input, "APAPI_STAT_ACC") == 0) {
        *output = APAPI_STAT_ACC;
        return PAPI_OK;
    }
    if (strcmp(input, "APAPI_STAT_ALL") == 0) {
        *output = APAPI_STAT_ALL;
        return PAPI_OK;
    }
    if (only_single_attribute) {
        return -1;
    }
    // complex attribute
    char delimiter = '|';
    int currentPos = 0;
    int tokenStart;
    int tokenEnd;
    int charIx;
    enum APAPI_stats current = 0;
    enum APAPI_stats new = 0;
    int retv;
    int end = 0;
    while (input[currentPos] != 0) {
        // search for next token stop
        for(charIx = currentPos; input[charIx] != delimiter && input[charIx] != 0; charIx++);
        tokenStart = currentPos;
        tokenEnd = charIx;
        if (input[tokenEnd] == 0) {
            end = 1;
        }
        input[tokenEnd] = 0;
        retv = apapi_parse_stats(&(input[tokenStart]), &(new), 1);
        if (retv != PAPI_OK) {
            return -1;
        }
        current = current | new;
        if (end) {
            break;
        }
        currentPos = tokenEnd+1;
    }
    *output = current;

    return PAPI_OK;
}

/**
 * Note: The input char' buffer is modified and the char* in the returned structs reference the respective strings in the buffer.
 *       Therefore you should not free the buffer if the function returns successfully.
 *       If the function fails you are free to free the buffer.
 *       If the function fails the buffer may still have been already modified.
 */
int APAPI_read_event_ops_csv(char *input, char delimiter, struct apapi_event_ops **events_out, int *num_events_out) {

    // dependent on number of attributes in struct apapi_event_ops
    #define MAX_APAPI_EVENT_OPS_TOKENS 11

    if (input == NULL) {
        return -1;
    }

    // check number of lines
    int retv;
    int charIx;
    int lines = 0;
    for(charIx=0; ;charIx++) {
        if ((input[charIx] == 0 && charIx!=0) || input[charIx] == '\n') {
            lines++;
        }
        if (input[charIx] == 0) {
            break;
        }
    }
    #ifdef DEBUG
        printf("Found %d lines\n", lines);
    #endif
    if (lines == 0) {
        // empty file
        return -1;
    }

    // allocate memory for #lines structs
    *events_out = calloc(lines+1, sizeof(struct apapi_event_ops));

    // go through all lines
    int lineIx;
    int currentPos = 0;
    int lineBreak = 0;
    int tokenIx = 0;
    int tokenStart = 0;
    int tokenBreak = 0;
    int lineStart = 0;
    int error = 0;
    int eventIx = 0;
    int read_events = 0;
    char *strto_endptr = 0;
    for(lineIx=0; lineIx < lines; lineIx++) {
        lineStart = currentPos;
        // search next line break
        for(charIx = currentPos; input[charIx]!=0 && input[charIx]!='\n'; charIx++);
        lineBreak = charIx;

        // if last line has zero length:
        if (lineStart == lineBreak) {
            currentPos = lineBreak+1;
            continue;
        }

        // line between currentPos and nextBreak
        // go through tokens using delimiter
        for(tokenIx=0; ; tokenIx++) {
            // get next delimiter position - end of token
            for(charIx = currentPos; input[charIx] != delimiter && charIx<lineBreak; charIx++);
            if (charIx == lineBreak && tokenIx < MAX_APAPI_EVENT_OPS_TOKENS-1) {
                error = 1;
                printf("Low number of tokens in line %d\n", lineIx+1);
                break;
            }
            if (tokenIx > MAX_APAPI_EVENT_OPS_TOKENS-1) {
                error = 1;
                printf("High number of tokens in line %d\n", lineIx+1);
                break;
            }
            tokenStart = currentPos;
            tokenBreak = charIx;

            if (tokenStart == tokenBreak) {
                error = 1;
                printf("Attribute %d has zero characters at line %d character %d\n", tokenIx+1, lineIx+1, tokenStart-lineStart+1);
                break;
            }
            // according to tokenIx try to fill struct apapi_event_ops
            switch(tokenIx) {
                case 0:
                    // char *event_name
                    // set delimiter to 0 and reuse string
                    input[tokenBreak] = 0;
                    (*events_out)[eventIx].event_name = &(input[tokenStart]);
                break;
                case 1:
                    // enum APAPI_op1 op1
                    input[tokenBreak] = 0;
                    retv = apapi_parse_op1(&(input[tokenStart]), &((*events_out)[eventIx].op1));
                    if (retv != PAPI_OK) {
                        error = 1;
                        printf("Unknown attribute %d op1 at line %d character %d\n", tokenIx+1, lineIx+1, tokenStart-lineStart+1);
                        break;
                    }
                break;
                case 2:
                    // APAPI_stats value0
                    input[tokenBreak] = 0;
                    retv = apapi_parse_stats(&(input[tokenStart]), &((*events_out)[eventIx].value0), 0);
                    if (retv != PAPI_OK) {
                        error = 1;
                        printf("Unknown attribute %d stats value0 at line %d character %d\n", tokenIx+1, lineIx+1, tokenStart-lineStart+1);
                        break;
                    }
                break;
                case 3:
                    // APAPI_stats value1
                    input[tokenBreak] = 0;
                    retv = apapi_parse_stats(&(input[tokenStart]), &((*events_out)[eventIx].value1), 0);
                    if (retv != PAPI_OK) {
                        error = 1;
                        printf("Unknown attribute %d stats value1 at line %d character %d\n", tokenIx+1, lineIx+1, tokenStart-lineStart+1);
                        break;
                    }
                break;
                case 4:
                    // long long max_sample
                    input[tokenBreak] = 0;
                    (*events_out)[eventIx].max_sample = strtoll(&(input[tokenStart]), &strto_endptr, 0);
                    if (errno != 0 || strto_endptr == &(input[tokenStart])) {
                        error = 1;
                        printf("Invalid attribute %d max_sample at at line %d character %d\n", tokenIx+1, lineIx+1, tokenStart-lineStart+1);
                        break;
                    }
                break;
                case 5:
                    // char *value0_type
                    input[tokenBreak] = 0;
                    (*events_out)[eventIx].value0_type = &(input[tokenStart]);
                break;
                case 6:
                    // char *value0_unit
                    input[tokenBreak] = 0;
                    (*events_out)[eventIx].value0_unit = &(input[tokenStart]);
                break;
                case 7:
                    // double value0_prefix
                    input[tokenBreak] = 0;
                    (*events_out)[eventIx].value0_prefix = strtod(&(input[tokenStart]), &strto_endptr);
                    if (errno != 0 || strto_endptr == &(input[tokenStart])) {
                        error = 1;
                        printf("Invalid attribute %d double value0_prefix at at line %d character %d\n", tokenIx+1, lineIx+1, tokenStart-lineStart+1);
                        break;
                    }
                break;
                case 8:
                    // char *value1_type
                    input[tokenBreak] = 0;
                    (*events_out)[eventIx].value1_type = &(input[tokenStart]);
                break;
                case 9:
                    // char *value1_unit
                    input[tokenBreak] = 0;
                    (*events_out)[eventIx].value1_unit = &(input[tokenStart]);
                break;
                case 10:
                    // double value0_prefix
                    input[tokenBreak] = 0;
                    (*events_out)[eventIx].value1_prefix = strtod(&(input[tokenStart]), &strto_endptr);
                    if (errno != 0 || strto_endptr == &(input[tokenStart])) {
                        error = 1;
                        printf("Invalid attribute %d double value1_prefix at at line %d character %d\n", tokenIx+1, lineIx+1, tokenStart-lineStart+1);
                        break;
                    }
                    read_events++;
                    eventIx++;
                break;
            }
            if (error) {
                break;
            }
            currentPos = tokenBreak+1;
            if (tokenBreak == lineBreak)
                break;
        }
        if (error) {
            break;
        }
        currentPos = lineBreak+1;
    }
    if (error) {
        // free memory
        printf("An error occured while reading events\n");
        free(*events_out);
        return -1;
    }

    *num_events_out = read_events;
    printf("events done\n");
    return PAPI_OK;
}
