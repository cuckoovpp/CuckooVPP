#include "cuckoo.h"

#define CUCKOO_NESTS_PER_BUCKET 4

/* 
 * finds the next power of 2 for the provided value
 * @param x , value representing maximum keys to be added
 * @return , finds the next power of two, so the size of the cuckoo filter can be set
 */
static inline size_t
next_power_of_two(size_t x) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;

    if (sizeof(size_t) == 8) {
        x |= x >> 32;
    }

    return ++x;
}

/**
 * creates a new cuckoo filter
 * @param filter , pointer to intended cuckoo filter
 * @param max_key_count , maximum number of ips that will be added to the filter
 * @param max_kick_attempts , maximum kicks allowed
 * @return CUCKOO_FILTER_RETURN indicating the result
 */
CUCKOO_FILTER_RETURN
cuckoo_filter_new(cuckoo_filter_t **filter, size_t max_key_count, size_t max_kick_attempts) {
    cuckoo_filter_t *new_filter;

    size_t bucket_count =
            next_power_of_two(max_key_count / CUCKOO_NESTS_PER_BUCKET);
    if (0.96 < (double) max_key_count / bucket_count / CUCKOO_NESTS_PER_BUCKET) {
        bucket_count <<= 1;
    }

    size_t allocation_in_bytes = (sizeof(cuckoo_filter_t)
                                  + (bucket_count * CUCKOO_NESTS_PER_BUCKET * sizeof(cuckoo_nest_t)));

    if (0 != posix_memalign((void **) &new_filter, sizeof(uint64_t),
                            allocation_in_bytes)) {
        return CUCKOO_FILTER_ALLOCATION_FAILED;
    }

    memset(new_filter, 0, allocation_in_bytes);

    new_filter->last_victim = NULL;
    memset(&new_filter->victim, 0, sizeof(new_filter->victim));
    new_filter->bucket_count = bucket_count*CUCKOO_NESTS_PER_BUCKET;
    new_filter->nests_per_bucket = CUCKOO_NESTS_PER_BUCKET;
    new_filter->max_kick_attempts = max_kick_attempts;
    new_filter->seed = (size_t) time(NULL);
	new_filter->mask = 0xffff;
	*filter = new_filter;

    return CUCKOO_FILTER_OK;

}


/**
 * get a crc 32 bit hash value
 * @param data
 * @param value
 * @return
 */
static inline uint32_t
crc_u32(uint32_t data, uint32_t value) {
    __asm__ volatile ("crc32l %[data], %[value];":[value] "+r"(value):[data]
    "rm"(data));
    return value;
}
//#endif /* __defined_crc_u32__ */

/**
 * returns the hash value by combining two murmur hash in order to get better hash
 * @param key , key to be added
 * @param key_length_in_bytes , number of bytes in key
 * @param size , size is total array size used to perform a mod operation
 * @param n , n ideally different for h1 and h2
 * @param seed, seed of the cuckoo filter
 * @return hash value
 */
static inline uint32_t
hash(
        const void *key,
        uint32_t key_length_in_bytes,
        uint32_t size,
        uint32_t n,
        uint32_t seed
) {
    	uint32_t value = 0;
		
    if (n == 0) {
		
		 uint64_t *fkey64 = (uint64_t *) key;
	        uint32_t fkey32= fkey64[0] & 0x00000000ffffffff;
   		uint32_t val=fkey64[0]>>32;
                value &=val;
		value = crc_u32(fkey32, seed);
		return (value & (size-1));
    } else {
		
		uint16_t *fkey32 = (uint16_t *) key;
		uint32_t fkey=fkey32[0];
		value = crc_u32(fkey, seed);
		return (value & (size-1));
    }

}


/**
 * generates a fingerprint for given key
 * @param key , key
 * @param key_length_in_bytes, number of bytes in key
 * @param n , n ideally different for h1 and h2
 * @param seed, seed of the cuckoo filter
 * @return fingerprint value
 */
static inline uint16_t
generate_fp(const void *key, uint32_t key_length_in_bytes, uint32_t n, uint32_t seed) {
  
    uint64_t *fkey64 = (uint64_t *) key;
    uint32_t value = 0;

    uint32_t fkey32= fkey64[0] & 0x00000000ffffffff;
    uint32_t fkey3= fkey64[0]>>32;
    fkey32&=fkey3;
    value = crc_u32(fkey32, seed);
    uint16_t val = value & 0x0000ffff;
    return val;

}


/**
 * performs lookup operation in the cuckoo filter
 * @param filter , pointer to intended cuckoo filter
 * @param result , pointer where the result is to be stored
 * @param key , the target key to lookup
 * @param key_length_in_bytes , number of bytes in key
 * @return CUCKOO_FILTER_RETURN indicating the result of lookup
 */
static inline CUCKOO_FILTER_RETURN
cuckoo_filter_lookup(
        cuckoo_filter_t *filter,
        cuckoo_result_t *result,
        void *key,
        size_t key_length_in_bytes,
	int subnet_length
) {
    uint16_t fingerprint =(uint16_t) (generate_fp(key, key_length_in_bytes, 1, filter->seed));
	fingerprint=(fingerprint>128?fingerprint-subnet_length:fingerprint);
    uint32_t h1 = hash(key, key_length_in_bytes, filter->bucket_count, 0,filter->seed);
    uint32_t h2 = ((h1 ^ hash(&fingerprint, sizeof(fingerprint),
                              filter->bucket_count, 1, filter->seed)) & (filter->bucket_count-1));    
    result->was_found = false;
    result->item.fingerprint = 0;
    result->item.h1 = 0;
    result->item.h2 = 0;

    fingerprint &= filter->mask;
	size_t ii;
    for (ii = 0; ii < filter->nests_per_bucket; ++ii) {
        cuckoo_nest_t *n1 =
                &filter->bucket[h1 + ii];
        if (fingerprint == n1->fingerprint) {
            result->was_found = true;
            break;
        }

        cuckoo_nest_t *n2 =
                &filter->bucket[h2 + ii];
        if (fingerprint == n2->fingerprint) {
            result->was_found = true;
            break;
        }
    }

    result->item.fingerprint = fingerprint;
    result->item.h1 = h1;
    result->item.h2 = h2;
    return ((true == result->was_found)
            ? CUCKOO_FILTER_OK : CUCKOO_FILTER_NOT_FOUND);

}

static inline CUCKOO_FILTER_RETURN
cuckoo_filter_lookup_contains(
        cuckoo_filter_t *filter,
        cuckoo_result_t *result,
        void *key,
        size_t key_length_in_bytes,
	int subnet_length, int *cnt
) {
    uint16_t fingerprint = (uint16_t)(generate_fp(key, key_length_in_bytes, 1, filter->seed));

	fingerprint=(fingerprint>128?fingerprint-subnet_length:fingerprint);
    result->was_found = false;
    result->item.fingerprint = 0;
    result->item.h1 = 0;
    result->item.h2 = 0;

    fingerprint &= filter->mask;
	size_t ii;
	
	uint32_t h1 = hash(key, key_length_in_bytes, filter->bucket_count, 0,
                       filter->seed);
	(*cnt)++;
	uint32_t h2 = 0;
	
    for (ii = 0; ii < filter->nests_per_bucket; ++ii) {
        cuckoo_nest_t *n1 =
                &filter->bucket[h1 + ii];
        if (fingerprint == n1->fingerprint) {
            result->was_found = true;
            goto goto1;
        }
    }
	
	if (false == result->was_found) {
		h2 = ((h1 ^ hash(&fingerprint, sizeof(fingerprint),
                             filter->bucket_count, 1, filter->seed)) & (filter->bucket_count-1));
		(*cnt)++;
	for (ii = 0; ii < filter->nests_per_bucket; ++ii) {
			cuckoo_nest_t *n2 =
					&filter->bucket[h2 + ii];
			if (fingerprint == n2->fingerprint) {
				result->was_found = true;
				break;
			}
		}
    }
	goto1 :
    result->item.fingerprint = fingerprint;
    result->item.h1 = h1;
    result->item.h2 = h2;
    return ((true == result->was_found)
            ? CUCKOO_FILTER_OK : CUCKOO_FILTER_NOT_FOUND);

}


/**
 * performs add fingerprint operation in the cuckoo filter for given index h
 * @param filter , pointer to intended cuckoo filter
 * @param fp , fingerprint to be added
 * @param h , index where the fingerprint is to be added
 * @return CUCKOO_FILTER_RETURN indicating the result of add
 */
static inline CUCKOO_FILTER_RETURN
add_fingerprint_to_bucket(
        cuckoo_filter_t *filter,
        uint16_t fp,
        uint32_t h
) {
    fp &= filter->mask;
	size_t ii;
    for (ii = 0; ii < filter->nests_per_bucket; ++ii) {
        cuckoo_nest_t *nest =
                &filter->bucket[h + ii];
        if (0 == nest->fingerprint) {
            nest->fingerprint = fp;
            return CUCKOO_FILTER_OK;
        }
    }

    return CUCKOO_FILTER_FULL;
}

/**
 * performs move operation in cuckoo filter
 * @param filter ,pointer to intended cuckoo filter
 * @param fingerprint , fingerprint to be moved
 * @param h1 , index from where it is initially stored
 * @param depth , depth indicate the number of kick occurred while moving
 * @return CUCKOO_FILTER_RETURN indicating the result of move
 */
static inline CUCKOO_FILTER_RETURN 
cuckoo_filter_move(
        cuckoo_filter_t *filter,
        uint16_t fingerprint,
        uint32_t h1,
        int depth
) {
    uint32_t h2 = ((h1 ^ hash(&fingerprint, sizeof(fingerprint),
                              filter->bucket_count, 1, filter->seed)) & (filter->bucket_count-1));

    if (CUCKOO_FILTER_OK == add_fingerprint_to_bucket(filter,
                                                      fingerprint, h1)) {
        return CUCKOO_FILTER_OK;
    }

    if (CUCKOO_FILTER_OK == add_fingerprint_to_bucket(filter,
                                                      fingerprint, h2)) {
        return CUCKOO_FILTER_OK;
    }

    if (filter->max_kick_attempts == depth) {
        return CUCKOO_FILTER_FULL;
    }

    size_t row = (0 == (rand() & 1) ? h1 : h2);
    size_t col = (rand() & (filter->nests_per_bucket-1));
    size_t elem =
            filter->bucket[row + col].fingerprint;

    filter->bucket[row + col].fingerprint =
            fingerprint;

    return cuckoo_filter_move(filter, elem, row, (depth + 1));

}


// just CUCKOO 
CUCKOO_FILTER_RETURN
cuckoo_filter_add(
        cuckoo_filter_t *filter,
        void *key,
        size_t key_length_in_bytes, 
		int subnet_length
) {
    cuckoo_result_t result;
     CUCKOO_FILTER_RETURN res1 = CUCKOO_FILTER_OK;

    cuckoo_filter_lookup(filter, &result, key, key_length_in_bytes,subnet_length);
    if (true == result.was_found) {       // no need to try to insert twice
        return CUCKOO_FILTER_OK;
    }

    if (NULL != filter->last_victim) {
        return CUCKOO_FILTER_FULL;
    }
    if (!result.was_found) {                            // first insertion
        res1 = cuckoo_filter_move(filter, result.item.fingerprint, result.item.h1, 0);
    }
    return res1;    
    
}

/**
 * checks if the key is present in intended cuckoo filter
 * @param filter , pointer to intended cuckoo filter
 * @param key , key to be added
 * @param key_length_in_bytes , number of bytes in key
 * @return CUCKOO_FILTER_RETURN indicating the result of contains
 */
CUCKOO_FILTER_RETURN
cuckoo_filter_contains(
        cuckoo_filter_t *filter,
        void *key,
        size_t key_length_in_bytes,
		int subnet_length, int *cnt_hash
) {
    cuckoo_result_t result;
    CUCKOO_FILTER_RETURN res1;
	
    res1 = cuckoo_filter_lookup_contains(filter, &result, key, key_length_in_bytes,subnet_length,cnt_hash);
    return res1;
}


/**
 * must be called to free the memory allocated in the end to make sure there is no memory leak
 * @param filter , pointer of the cuckoo filter
 * @return CUCKOO_FILTER_RETURN indicating the result of free operation
 */
CUCKOO_FILTER_RETURN
cuckoo_filter_free(
        cuckoo_filter_t **filter
) {
    free(*filter);
    *filter = NULL;
    return CUCKOO_FILTER_OK;
}


