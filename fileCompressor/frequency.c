#define _GNU_SOURCE

#include <search.h>
#include <stdlib.h>
#include <string.h>

#include "frequency.h"

struct frequency_map {
    // Hashmap called table
    struct hsearch_data table;
};

// Creates frequency map
FrequencyMap *frequencyMap_create(size_t capacity) {
    // Allocates size
    FrequencyMap *fm = malloc(sizeof(FrequencyMap));
    // Error checking
    if (fm == NULL) {
        return NULL;
    }
    //
    memset(&fm->table, 0, sizeof(fm->table));
    hcreate_r(capacity, &fm->table);
    return fm;
}


void frequencyMap_destroy(FrequencyMap *fm) {
    hdestroy_r(&fm->table);
    free(fm);
}


void frequencyMap_insert(FrequencyMap *fm, char *token) {
    // Creates entry
    ENTRY entry, *entry_p;
    // Adds token to entry
    entry.key = token;
    // Set frequency to 0
    entry.data = 0;
    hsearch_r(entry, ENTER, &entry_p, &fm->table);
    if (entry_p != NULL) {
        entry_p->data = (void *)(((size_t)entry_p->data) + 1);
    }
}


int frequencyMap_frequency(FrequencyMap *fm, char *token) {
    ENTRY entry, *entry_p;
    entry.key = token;
    hsearch_r(entry, FIND, &entry_p, &fm->table);
    return entry_p ? (size_t)(entry_p->data) : 0;
}
