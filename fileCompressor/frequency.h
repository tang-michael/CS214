/*
 * frequency.h
 *
 * Counter structure for counting token frequencies.
 */

#ifndef FREQUENCY_H
#define FREQUENCY_H

// Creates name for frequency_map
typedef struct frequency_map FrequencyMap;


/*
 * Create a new FrequencyMap instance.
 * Capacity determines the number of distinct tokens the instance can track.
 */
FrequencyMap *FM_create(size_t capacity);


/*
 * Deallocate the instance.
 * This does not affect any of the tokens referenced by the map.
 */
void FM_destroy(FrequencyMap *fm);


/*
 * Increase the frequency of a token by 1 (inserts the token if it is not
 * already present in the map).
 */
void FM_add(FrequencyMap *fm, char *token);


/*
 * Get the frequency of a token. Returns 0 if the token is not in the map.
 */
int FM_frequency(FrequencyMap *fm, char *token);


#endif
