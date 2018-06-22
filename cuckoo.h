#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>



/**
 * structure for a cuckoo item
 */
typedef struct {
    uint16_t fingerprint;
    uint32_t h1;
    uint32_t h2;
    uint32_t padding;
} __attribute__((packed)) cuckoo_item_t;

/**
 * structure for result of contains
 */
typedef struct {
    bool was_found;
    cuckoo_item_t item;
} cuckoo_result_t;

/**
 * structure for a cuckoo nest which stores a key
 */
typedef struct {
    uint16_t fingerprint;
} __attribute__((packed)) cuckoo_nest_t;


/**
 * structure of a cuckoo filter
 */
typedef struct cuckoo_filter_t {
    uint32_t bucket_count;
    uint32_t nests_per_bucket;
    uint16_t mask;
    uint32_t max_kick_attempts;
    uint32_t seed;
    cuckoo_item_t victim;
    cuckoo_item_t *last_victim;
    cuckoo_nest_t bucket[1];
} __attribute__((packed)) cuckoo_filter_t;


/**
 * enum for different type of return values indicating whether the operation was success or not
 */
typedef enum {
    CUCKOO_FILTER_OK = 0,
    CUCKOO_FILTER_NOT_FOUND = 1,
    CUCKOO_FILTER_FULL = 2,
    CUCKOO_FILTER_ALLOCATION_FAILED = 3,
} CUCKOO_FILTER_RETURN;


/**
 * creates a new cuckoo filter
 * @param filter , pointer to intended cuckoo filter
 * @param max_key_count , maximum number of ips that will be added to the filter
 * @param max_kick_attempts , maximum kicks allowed
 * @return CUCKOO_FILTER_RETURN indicating the result of adding
 */
CUCKOO_FILTER_RETURN
cuckoo_filter_new(cuckoo_filter_t **filter, size_t max_key_count, size_t max_kick_attempts);


/**
 * attempts to add the key to intended cuckoo filter
 * @param filter , pointer to intended cuckoo filter
 * @param key , key to be added
 * @param key_length_in_bytes , number of bytes in key
 * @param subnet_length, length of the subnet
 * @return CUCKOO_FILTER_RETURN indicating the result of adding
 */
CUCKOO_FILTER_RETURN
cuckoo_filter_add(
        cuckoo_filter_t *filter,
        void *key,
        size_t key_length_in_bytes, int subnet_length
);

/**
 * checks if the key is present in intended cuckoo filter
 * @param filter , pointer to intended cuckoo filter
 * @param key , key to be added
 * @param key_length_in_bytes , number of bytes in key
 * @return CUCKOO_FILTER_RETURN indicating the result of adding
 */
CUCKOO_FILTER_RETURN
cuckoo_filter_contains(
        cuckoo_filter_t *filter,
        void *key,
        size_t key_length_in_bytes,
		int subnet_length, int *cnt_hash
);


/**
 * must be called to free the memory allocated in the end to make sure there is no memory leak
 * @param filter , pointer of the cuckoo filter
 * @return CUCKOO_FILTER_RETURN indicating the result of free operation
 */
CUCKOO_FILTER_RETURN
cuckoo_filter_free(
        cuckoo_filter_t **filter
);

