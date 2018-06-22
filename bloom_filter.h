#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

typedef enum {
	BLOOM_FILTER_OK = 1,
	BLOOM_FILTER_NOT_OK = 0,
} BLOOM_FILTER_RETURN;

typedef struct bloom_array {
	bool* bit_array;
} bloom_array_t;

typedef struct bloom_filter_t {
	double probability;
	uint32_t no_hash_functions;
	uint32_t size;
	bloom_array_t bloom_array;
} __attribute__((packed)) bloom_filter_t;

BLOOM_FILTER_RETURN bloom_filter_new (bloom_filter_t **filter,
										uint32_t n,
										double probability,
										uint32_t k,
										uint32_t m);


BLOOM_FILTER_RETURN bloom_filter_add (bloom_filter_t *filter,
									void *key);
									
BLOOM_FILTER_RETURN bloom_filter_contains (bloom_filter_t *filter,
									void *key, int *cnt);

void print_hashes(bloom_filter_t *filter, void *key);

void create_threads();
void count_bloom(bloom_filter_t *filter,int *b_count);
