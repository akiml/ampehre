/*                                                                                                                                              
 * apapi_defaults.c
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
 * version: 0.8.0 - initial implementation
 */

// parse csv

/** Internal
 *	@class _apapi_parse_op1
 *	@brief checks input string and returns matching enum APAPI_op1 value
 *
 *	@param char* input
 *		string to match
 *	@param enum APAPI_op1 *output
 *		Output parameter for matching enum value
 */
int _apapi_parse_op1(char* input, enum APAPI_op1 *output) {
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

/** Internal
 *	@class _apapi_parse_stats
 *	@brief matches string to enum APAPI_stats value or a combination
 *		examples:
 *			APAPI_STAT_NO
 *			APAPI_STAT_MIN|APAPI_STAT_MAX
 *			APAPI_STAT_AVG|APAPI_STAT_ACC|APAPI_STAT_MIN
 *
 *		this method uses one recursive call
 *
 *
 *	@param char* input
 *		input string to match
 *	@param enum APAPI_stats *output
 *		Output parameter for matched value
 *	@param int only_single_attribute
 *		match only single identifiers no combinations
 */
int _apapi_parse_stats(char* input, enum APAPI_stats *output, int only_single_attribute) {
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
        retv = _apapi_parse_stats(&(input[tokenStart]), &(new), 1);
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
		
		// if line starts with '#'
		if (input[lineStart] == '#') {
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
                APAPI_PRINTERR("Low number of tokens in line %d\n", lineIx+1)
                break;
            }
            if (tokenIx > MAX_APAPI_EVENT_OPS_TOKENS-1) {
                error = 1;
                APAPI_PRINTERR("High number of tokens in line %d\n", lineIx+1)
                break;
            }
            tokenStart = currentPos;
            tokenBreak = charIx;

            if (tokenStart == tokenBreak) {
                error = 1;
                APAPI_PRINTERR("Attribute %d has zero characters at line %d character %d\n", tokenIx+1, lineIx+1, tokenStart-lineStart+1)
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
                    retv = _apapi_parse_op1(&(input[tokenStart]), &((*events_out)[eventIx].op1));
                    if (retv != PAPI_OK) {
                        error = 1;
                        APAPI_PRINTERR("Unknown attribute %d op1 at line %d character %d\n", tokenIx+1, lineIx+1, tokenStart-lineStart+1)
                        break;
                    }
                break;
                case 2:
                    // APAPI_stats value0
                    input[tokenBreak] = 0;
                    retv = _apapi_parse_stats(&(input[tokenStart]), &((*events_out)[eventIx].value0), 0);
                    if (retv != PAPI_OK) {
                        error = 1;
                        APAPI_PRINTERR("Unknown attribute %d stats value0 at line %d character %d\n", tokenIx+1, lineIx+1, tokenStart-lineStart+1)
                        break;
                    }
                break;
                case 3:
                    // APAPI_stats value1
                    input[tokenBreak] = 0;
                    retv = _apapi_parse_stats(&(input[tokenStart]), &((*events_out)[eventIx].value1), 0);
                    if (retv != PAPI_OK) {
                        error = 1;
                        APAPI_PRINTERR("Unknown attribute %d stats value1 at line %d character %d\n", tokenIx+1, lineIx+1, tokenStart-lineStart+1)
                        break;
                    }
                break;
                case 4:
                    // long long max_sample
                    input[tokenBreak] = 0;
                    (*events_out)[eventIx].max_sample = strtoll(&(input[tokenStart]), &strto_endptr, 0);
                    if (errno != 0 || strto_endptr == &(input[tokenStart])) {
                        error = 1;
                        APAPI_PRINTERR("Invalid attribute %d max_sample at at line %d character %d\n", tokenIx+1, lineIx+1, tokenStart-lineStart+1)
						errno = 0;
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
                        APAPI_PRINTERR("Invalid attribute %d double value0_prefix at at line %d character %d\n", tokenIx+1, lineIx+1, tokenStart-lineStart+1)
						errno = 0;
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
                        APAPI_PRINTERR("Invalid attribute %d double value1_prefix at at line %d character %d\n", tokenIx+1, lineIx+1, tokenStart-lineStart+1)
						errno = 0;
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
        APAPI_PRINTERR("An error occured while reading events\n")
        free(*events_out);
        return -1;
    }

    *num_events_out = read_events;
    return PAPI_OK;
}

/** Internal
 *	@class _apapi_read_file
 *	@brief Reads content of file to memory
 *
 *	@param char* filename
 *		name of file to read
 *	@param char **file_buffer
 *		Output parameter, sets *file_buffer to read file in memory
 *		The buffer has a length of filesize+1
 *		Last char is 0
 *	@param long *filesize
 *		Output parameter, sets *filesize to size of file
 */
int _apapi_read_file(char* filename, char **file_buffer, long *filesize) {
    if (NULL == filename) {
        return 1;
    }
    int retv;
    FILE *file;
    // try to open file
    file = fopen(filename, "r");
    if (NULL == file && errno != 0) {
        APAPI_PRINTERR("Unable to open file %s.\n", filename)
		errno = 0;
        return 1;
    }

    // check file size
    *filesize = 0;
    retv = fseek(file, 0L, SEEK_END);
    if (retv == -1 && errno != 0) {
        APAPI_PRINTERR("Unable to seek file %s.\n",filename)
        fclose(file);
		errno = 0;
        return 1;
    }
    *filesize = ftell(file);
    if (*filesize == -1 && errno != 0) {
        APAPI_PRINTERR("Unable to tell file %s.\n",filename)
        fclose(file);
		errno = 0;
        return 1;
    }
    rewind(file);
    if (*filesize > 10000000L) {
        APAPI_PRINTERR("File %s is too big.\n",filename)
        fclose(file);
        return 1;
    }
    *file_buffer = calloc(1, *filesize+1);
    if (*file_buffer == NULL) {
        APAPI_PRINTERR("Not enough memory for file %s.\n",filename)
        fclose(file);
        return 1;
    }
    size_t read;
    read = fread(*file_buffer, *filesize, 1, file);
    if (read != 1) {
        APAPI_PRINTERR("Reading file %s failed.\n.",filename)
        free(*file_buffer);
        fclose(file);
        return 1;
    }

    return 0;
}


/** Internal
 *	@class _apapi_read_eventops_file
 *	@brief Reads file with event default operations
 *
 *	@param char *filename
 *		name of file to read
 *	@param struct apapi_event_ops **events_output
 *		Output parameter, points to array with event operation definitions
 *	@param char **defaults_file_buffer
 *		Output parameter, points to allocated memory for file content and event operation strings
 *	@param int *num_events
 *		Output parameter, number of read event operation definitions
 */
int _apapi_read_eventops_file(char *filename, struct apapi_event_ops **events_output, char **defaults_file_buffer, int *num_events) {

    int retv;
    long filesize;

    retv = _apapi_read_file(filename, defaults_file_buffer, &filesize);

    if (retv != 0) {
        return retv;
    }

    retv = APAPI_read_event_ops_csv(*defaults_file_buffer, ',', events_output, num_events);
    if (PAPI_OK == retv) {
		if (_apapi_verbose) {
	        APAPI_PRINT("Read %d event operations from %s.\n", *num_events, filename)
		}
		return PAPI_OK;
    } else {
        APAPI_PRINTERR("Reading event operations failed.\n")
		if (*defaults_file_buffer != NULL) {
			free(*defaults_file_buffer);
			*defaults_file_buffer = NULL;
		}
		if (NULL != *events_output) {
			free(*events_output);
			*events_output = NULL;
		}
		return -1;
    }
}

int APAPI_read_env_eventops(char **buffer, struct apapi_event_ops **events_out, int *num_events_out){

	// search for APAPI_EVENTOPS variable	
	int envIx;
	char *eventops_env = NULL;
	for (envIx = 0; environ[envIx] != NULL; ++envIx) {
		if (strncmp(environ[envIx], "APAPI_EVENTOPS=", 15) == 0) {
			eventops_env = environ[envIx];
			break;
		}
	}

	// variable not found
	if (NULL == eventops_env) {
		return -1;
	}

	// get filename
	char *eventops_filename = &(eventops_env[15]);

	if (_apapi_verbose == 1) {
		APAPI_PRINT("Env event operations file: %s\n", eventops_filename)
	}

	return _apapi_read_eventops_file(eventops_filename, events_out, buffer, num_events_out);
}

// parse event list

/** Internal
 *	@class _apapi_count_cmp_events
 *	@brief Count number of events per component in event list
 *
 *	@param char **cmp_list
 *		list of component names
 *	@param char **events
 *		list of event names
 *	@param int32_t *event_cmp_map
 *		Output parameter, list of component index for each event from event list
 *	@param int *event_counts
 *		Output parameter, list of event numbers for each component from component list
 *	@param int *unknown
 *		Output parameter, number of events that belong to unknown components
 */
int32_t _apapi_count_cmp_events(char **cmp_list, char **events, int32_t *event_cmp_map, int *event_counts, int *unknown) {

	// set counts to 0
	int32_t current_cmp = 0;
	int32_t cmp_count = 0;
	for (current_cmp = 0; cmp_list[current_cmp] != NULL; current_cmp++) {
		event_counts[current_cmp] = 0;
	}
	cmp_count = current_cmp;
	// get cmp name lengths
	int8_t cmp_len[cmp_count];
	memset(cmp_len, 0, sizeof(int8_t)*cmp_count);
	for(current_cmp = 0; current_cmp < cmp_count; current_cmp++) {
		cmp_len[current_cmp] = strlen(cmp_list[current_cmp]);
	}

	int32_t current_event = 0;
	*unknown = 0;
	// for all events
	for (current_event = 0; events[current_event] != NULL; current_event++) {
		// for all components
		for(current_cmp = 0; current_cmp < cmp_count ; current_cmp++) {
			if (strncmp(cmp_list[current_cmp], events[current_event], cmp_len[current_cmp]) == 0) {
				event_counts[current_cmp]++;
				break;
			}
		}
		if (current_cmp == cmp_count) {
			(*unknown)++;
			event_cmp_map[current_event] = -1;
		} else {
			event_cmp_map[current_event] = current_cmp;
		}
	}
	return 0;
}

/** Internal
 *	@class _apapi_read_eventlist
 *	@brief Converts an ASCII file to a two-dimensional array of char*
 *  Manipulates input array.
 *  Resulting pointers point into input.
 *
 *	@param char *input
 *		Input data
 *	@param char ***events
 *		Output parameter,
 *	  event_sets - array of event sets
 *				   an event set is a char* array with
 *				   first pointer pointing to name of event set
 *				   next pointers point to event names
 *				   last pointer is NULL
 *	@param int *event_count
 *		number of events
 */
int _apapi_read_eventlist(char *input, char ***events, int *event_count) {
	if (input == NULL) {
		return 1;
	}

	// scan for maximal number of events
	int max_events = 0;
	int current = 0;
	int inputsize = 0;
	for (current = 0; input[current] != 0; current++) {
		if (input[current] == '\n') {
			max_events++;
		}
	}
	if (current > 1 && input[current-1] != '\n') {
		max_events++;
	}
	inputsize = current;
	if (inputsize <5) {
		return 1;
	}
	*events = calloc(sizeof(char*), max_events+1);
	if (*events == NULL) {
		return 1;
	}
	int eventIx = 0;
	(*events)[eventIx] = input;
	eventIx++;
	// create event array
	for (current = 0; input[current] != 0; current++) {
		// current character is newline
		if (input[current] == '\n') {
			input[current] = 0;
			// next character is start of next line (therefor not newline or 0 or #)
			if (input[current+1] != '\n' && input[current+1] != 0 && input[current+1] != '#') {
				(*events)[eventIx] = &(input[current+1]);
				eventIx++;
				continue;
			}
		}
	}
	*event_count = eventIx;
	return 0;
}

/** Internal
 *	@class _apapi_sort_events
 *	@brief sort events by component
 *
 *	@param char **cmp_list
 *		list of component names
 *	@param char **events
 *		list of event names
 *	@param char ***used_cmp
 *		Output parameter, list of used components
 *	@param char ****sorted_events
 *		Output parameter, list of list of event names sorted by components
 *						component x event
 */
int32_t _apapi_sort_events(char **cmp_list, char **events, char ***used_cmp, char ****sorted_events) {

	if (cmp_list == NULL || events == NULL) {
		return -1;
	}

	int cmp_count = 0;
	int event_count = 0;
	int cmpIx = 0;
	int eventIx = 0;
	for (cmpIx = 0; cmp_list[cmpIx] != NULL; cmpIx++);
	cmp_count = cmpIx;
	for (eventIx = 0; events[eventIx] != NULL; eventIx++);
	event_count = eventIx;

	int *event_counts = calloc(sizeof(int), cmp_count);
	if (event_counts == NULL) {
		return -1;
	}
	int32_t *event_cmp_map = calloc(sizeof(int32_t), event_count);
	if (event_cmp_map == NULL) {
		free(event_counts);
		return -1;
	}
	int unknown = 0;

	int32_t retv = 0;

	retv = _apapi_count_cmp_events(cmp_list, events, event_cmp_map, event_counts, &unknown);
	if (retv != 0) {
		free(event_cmp_map);
		free(event_counts);
		return -1;
	}

	int selected_cmp_count = 0;
	for (cmpIx = 0; cmpIx < cmp_count; cmpIx++) {
		if (event_counts[cmpIx] > 0) {
			selected_cmp_count++;
		}
	}

	*sorted_events = calloc(sizeof(char**), selected_cmp_count+1);
	if (*sorted_events == NULL) {
		free(event_cmp_map);
		free(event_counts);
		return -1;
	}
	int *used_cmp_map = calloc(sizeof(int), cmp_count);
	if (used_cmp_map == NULL) {
		free(*sorted_events);
		free(event_cmp_map);
		free(event_counts);
		return -1;
	}
	*used_cmp = calloc(sizeof(char*), selected_cmp_count+1);
	if (*used_cmp == NULL) {
		free(used_cmp_map);
		free(*sorted_events);
		free(event_cmp_map);
		free(event_counts);
		return -1;
	}
   
	// allocated memory for event lists (per component)
	int selectedCmpIx = 0;
	for (cmpIx = 0; cmpIx < cmp_count; cmpIx++) {
		if (event_counts[cmpIx] > 0) {
			(*sorted_events)[selectedCmpIx] = calloc(sizeof(char*), event_counts[cmpIx]+1);
			used_cmp_map[cmpIx] = selectedCmpIx;
			(*used_cmp)[selectedCmpIx] = cmp_list[cmpIx];
			selectedCmpIx++;
		} else {
			used_cmp_map[cmpIx] = -1;
		}
		event_counts[cmpIx] = 0;
	}
	// fill per component event lists with event names from original event list
	for (eventIx = 0; eventIx < event_count; eventIx++) {
		cmpIx = event_cmp_map[eventIx];
		if(cmpIx != -1) {
			selectedCmpIx = used_cmp_map[cmpIx];
			(*sorted_events)[selectedCmpIx][event_counts[cmpIx]] = events[eventIx];
			event_counts[cmpIx]++;
		}
	}

	free(event_cmp_map);
	free(event_counts);
	free(used_cmp_map);

	return 0;
}

/** Internal 
 *	@class _apapi_free_sorted_eventlist
 *	@brief free sorted event list and list of used components
 *
 *	@param char ***used_cmp
 *		pointer to list of used components
 *	@param char ****sorted_events
 *		pointer to list of event names sorted by components
 */
void _apapi_free_sorted_eventlist(char ***used_cmp, char ****sorted_events){
	if (used_cmp != NULL) {
		free(*used_cmp);
		*used_cmp = NULL;
	}

	if (sorted_events != NULL) {
		int ix = 0;
		for(ix = 0; (*sorted_events)[ix] != NULL; ix++) {
			free((*sorted_events)[ix]);
		}
		free(*sorted_events);
		*sorted_events = NULL;
	}
}

/** Internal
 *	@class _apapi_read_eventlist_file
 *	@brief Reads file with event default operations
 *
 *	@param char *filename
 *		name of file to read
 *	@param char **cmp_list
 *		list of components to use
 *	@param char **buffer
 *		Output parameter, buffer with file contents
 *	@param char ****sorted_list
 *		Output parameter, list of event names sorted by components
 *	@param char ***used_cmp
 *		Output parameter, list of used components
 */
int _apapi_read_eventlist_file(char *filename, char **cmp_list, char **buffer, char ****sorted_list, char ***used_cmp) {

    int retv;
    long filesize;

    retv = _apapi_read_file(filename, buffer, &filesize);

    if (retv != 0) {
        return retv;
    }

	int event_count = 0;
	char **all_events;

	retv = _apapi_read_eventlist(*buffer, &all_events, &event_count);
	if (PAPI_OK != retv) {
        APAPI_PRINTERR("Reading eventlist file failed.\n")
		return -1;
	}

	retv = _apapi_sort_events(cmp_list, all_events, used_cmp, sorted_list);
	if (PAPI_OK != retv) {
        APAPI_PRINTERR("Sorting eventlist file failed.\n")
		free(*buffer);
		*buffer = NULL;
		free(all_events);
		free(all_events);
		return -1;
	}

	free(all_events);
	return PAPI_OK;
}


int APAPI_read_env_eventlist(char **cmp_list, char **buffer, char ****sorted_list, char ***used_cmp) {

	// search for APAPI_EVENTLIST variable
	int envIx;
	char *eventlist_env = NULL;
	for (envIx = 0; environ[envIx] != NULL; ++envIx) {
		if (strncmp(environ[envIx], "APAPI_EVENTLIST=", 16) == 0) {
			eventlist_env = environ[envIx];
			break;
		}
	}

	// variable not found
	if (eventlist_env == NULL) {
		return -1;
	}

	// get filename
	char *eventlist_filename = &(eventlist_env[16]);

	if (_apapi_verbose == 1) {
		APAPI_PRINT("Env eventlist file: %s\n", eventlist_filename)
	}

	return _apapi_read_eventlist_file(eventlist_filename, cmp_list, buffer, sorted_list, used_cmp);
}

int _apapi_read_cmplist(char *cmplist_str, char **buffer, char ***cmplist) {

	int cmp_count_max = 1; // n+1 possible entries delimited by n delimitors
	char *strpos = cmplist_str;
	for ( ; *strpos != 0; ++strpos) {
		if (*strpos == ':') {
			cmp_count_max += 1;
		}
	}


	*buffer = strdup(cmplist_str);
	if (*buffer == NULL) {
		return -1;
	}
	*cmplist = calloc(cmp_count_max+1, sizeof(char*)); // +1 for NULL pointer at the end
	if (*cmplist == NULL) {
		free(*buffer);
		*buffer = NULL;
		return -1;
	}

	char **cmplist_arr = *cmplist;
	strpos = *buffer;
	int cmplist_pos = 0;
	// search for next cmp name
	while ( strpos != NULL ) {
		while (*strpos == ':') {
			*strpos = 0;
			strpos++;
		}
		if (*strpos == 0) {
			break;
		}
		// found next cmp name
		cmplist_arr[cmplist_pos] = strpos;
		cmplist_pos++;

		strpos = strstr(strpos, ":");
	}

	return PAPI_OK;
}

int APAPI_read_env_cmplist(char **buffer, char ***cmplist) {

	// search for APAPI_CMPLIST variable
	int envIx;
	char *cmplist_env = NULL;
	for (envIx = 0; environ[envIx] != NULL; ++envIx) {
		if (strncmp(environ[envIx], "APAPI_CMPLIST=", 14) == 0) {
			cmplist_env = environ[envIx];
			break;
		}
	}

	// variable not found
	if (cmplist_env == NULL) {
		return -1;
	}

	// get cmplist string
	char *cmplist_str = &(cmplist_env[14]);

	if (_apapi_verbose == 1) {
		APAPI_PRINT("Env cmplist file: %s\n", cmplist_str)
	}

	return _apapi_read_cmplist(cmplist_str, buffer, cmplist);
}

int APAPI_cmp_cmplist_index(const char *cmp, char **cmplist) {

	if (NULL == cmp || NULL == cmplist) {
		return -1;
	}

	int cmpIx = 0;
	for(cmpIx = 0; cmplist[cmpIx] != NULL; ++cmpIx) {
		if (strcmp(cmp, cmplist[cmpIx]) == 0) {
			return cmpIx;
		}
	}

	return -1;
}
