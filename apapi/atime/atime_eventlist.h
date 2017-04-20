#include <string.h>
#include <stdint.h>

int32_t count_cmp_events(char **cmp_list, char **events, int32_t *event_cmp_map, uint32_t *event_counts, uint32_t *unknown) {

    // set counts to 0
    int32_t current_cmp = 0;
    int32_t cmp_count = 0;
    for(current_cmp = 0; cmp_list[current_cmp] != NULL; current_cmp++) {
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
    for(current_event = 0; events[current_event] != NULL; current_event++) {
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

/** Converts an ASCII file to a two-dimensional array of char*
 *  Manipulates input array.
 *  Resulting pointers point into input.
 *  Output:
 *      event_sets - array of event sets
 *                   an event set is a char* array with
 *                   first pointer pointing to name of event set
 *                   next pointers point to event names
 *                   last pointer is NULL
 */
int read_event_list(char *input, char ***events, uint32_t *event_count) {
    if (input == NULL) {
        return 1;
    }

    // scan for maximal number of events
    int max_events = 0;
    int current = 0;
    int inputsize = 0;
    for(current = 0; input[current] != 0; current++) {
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
    uint32_t eventIx = 0;
    (*events)[eventIx] = input;
    eventIx++;
    // create event array
    for(current = 0; input[current] != 0; current++) {
        // current character is newline
        if (input[current] == '\n') {
            input[current] = 0;
            // next character is start of next line (therefor not newline or 0)
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

int32_t sort_events(char **cmp_list, char **events, char ***used_cmp, char ****sorted_events) {

    if (cmp_list == NULL || events == NULL) {
        return -1;
    }

    uint32_t cmp_count = 0;
    uint32_t event_count = 0;
    uint32_t cmpIx = 0;
    uint32_t eventIx = 0;
    for(cmpIx = 0; cmp_list[cmpIx] != NULL; cmpIx++);
    cmp_count = cmpIx;
    for(eventIx = 0; events[eventIx] != NULL; eventIx++);
    event_count = eventIx;

    uint32_t *event_counts = calloc(sizeof(uint32_t), cmp_count);
    if (event_counts == NULL) {
        return -1;
    }
    int32_t *event_cmp_map = calloc(sizeof(int32_t), event_count);
    if (event_cmp_map == NULL) {
        free(event_counts);
        return -1;
    }
    uint32_t unknown = 0;

    int32_t retv = 0;

    retv = count_cmp_events(cmp_list, events, event_cmp_map, event_counts, &unknown);
    if (retv != 0) {
        free(event_cmp_map);
        free(event_counts);
        return -1;
    }

    uint32_t selected_cmp_count = 0;
    for(cmpIx = 0; cmpIx < cmp_count; cmpIx++) {
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
    uint32_t *used_cmp_map = calloc(sizeof(uint32_t), cmp_count);
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
    uint32_t selectedCmpIx = 0;
    for(cmpIx = 0; cmpIx < cmp_count; cmpIx++) {
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
    for(eventIx = 0; eventIx < event_count; eventIx++) {
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

void free_sorted_events(char ***used_cmp, char ****sorted_events){
    if (used_cmp != NULL) {
        free(*used_cmp);
    }

    if (sorted_events != NULL) {
        int ix = 0;
        for(ix = 0; (*sorted_events)[ix] != NULL; ix++) {
            free((*sorted_events)[ix]);
        }
        free(*sorted_events);
    }

}
