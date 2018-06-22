#include "bloom_filter.h"
#define PRIME64_1 11400714785074694791ULL
#define PRIME64_2 14029467366897019727ULL
#define PRIME64_3  1609587929392839161ULL
#define PRIME64_4  9650029242287828579ULL
#define PRIME64_5  2870177450012600261ULL
#define PRIME64_6  2870177450012600222ULL

static inline size_t
next_power_of_two(size_t x) 
{
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


//count for load factor
void count_bloom(bloom_filter_t *filter,int *b_count){
	int i;
	for(i=0;i<filter->size;i++){
		if(filter->bloom_array.bit_array[i]==1){
			(*b_count)++;

			}
	}
}


BLOOM_FILTER_RETURN bloom_filter_new (bloom_filter_t **filter,
										uint32_t n,
										double probability,
										uint32_t k,
										uint32_t m)
{
		bloom_filter_t *new_filter;
		size_t no_of_bytes_required = next_power_of_two ( (size_t) m );
		
		new_filter = malloc(sizeof(bloom_filter_t));
		
		new_filter->probability = probability;
		new_filter->no_hash_functions = k;
		new_filter->size = no_of_bytes_required;
		
		 if (0 != posix_memalign((void **) &new_filter->bloom_array.bit_array, sizeof(uint64_t),
                            no_of_bytes_required)) {
        		return BLOOM_FILTER_NOT_OK;
    		}

		memset(new_filter->bloom_array.bit_array, 0 , no_of_bytes_required);
		
		*filter = new_filter;
		
		return BLOOM_FILTER_OK;
}

static inline uint32_t
crc_u32(uint32_t data, uint32_t value) {
    __asm__ volatile ("crc32l %[data], %[value];":[value] "+r"(value):[data]
    "rm"(data));
    return value;
}

static inline uint32_t
hash1(
        const void *key,
        uint32_t size,
        unsigned long long seed
) {
	uint64_t *fkey64 = (uint64_t *) key;
        uint32_t fkey32= fkey64[0] & 0x00000000ffffffff;
	uint32_t value = 0;
	uint32_t sd=(uint32_t)seed;
	value = crc_u32(fkey32, sd);
	return (value & (size-1));
}

static inline uint32_t
hash2(
        const void *key,
        uint32_t size,
        unsigned long long seed
) {
	uint32_t value = 0;
	uint32_t sd=(uint32_t)seed;
	 uint64_t *fkey64 = (uint64_t *) key;
        uint32_t fkey32= fkey64[0]>>32; 
	value = crc_u32(fkey32, sd);
	return (value & (size-1));
}

static inline uint32_t
hash3(
        const void *key,
        uint32_t size,
        unsigned long long seed
) {
	uint32_t value = 0;
	uint32_t sd=(uint32_t)seed;
	uint64_t *fkey64 = (uint64_t *) key;
        uint32_t fkey32= fkey64[1] & 0x00000000ffffffff;
	value = crc_u32(fkey32, sd);
	return (value & (size-1));
}

static inline uint32_t
hash4(
        const void *key,
        uint32_t size,
        unsigned long long seed
) {
	uint32_t value = 0;
	uint32_t sd=(uint32_t)seed;
	uint64_t *fkey64 = (uint64_t *) key;
        uint32_t fkey32= fkey64[1]>>32; 
        value = crc_u32(fkey32, sd);
	return (value & (size-1));
}

static inline uint32_t
hash5(
        const void *key,
        uint32_t size,
        unsigned long long seed
) {
        uint32_t value = 0;
	uint64_t *fkey64 = (uint64_t *) key;
        uint32_t fkey32= fkey64[0]>>32;
	uint32_t fkey321= fkey64[1]>>32;  
        value = crc_u32(fkey32, fkey321);
        return (value & (size-1));
}

static inline uint32_t
hash6(
        const void *key,
        uint32_t size,
        unsigned long long seed
) {
        uint32_t value = 0;
	 uint64_t *fkey64 = (uint64_t *) key;
        uint32_t fkey32= fkey64[0]>>32;
        uint32_t fkey321= fkey64[0]& 0x00000000ffffffff;  
        value = crc_u32(fkey32, fkey321);
        return (value & (size-1));
}

static inline uint32_t
hash7(
        const void *key,
        uint32_t size,
        unsigned long long seed
) {
        uint32_t value = 0;
	uint64_t *fkey64 = (uint64_t *) key;
        uint32_t fkey32= fkey64[1]>>32;
        uint32_t fkey321= fkey64[1]& 0x00000000ffffffff;  
        value = crc_u32(fkey32, fkey321);
        return (value & (size-1));
}

BLOOM_FILTER_RETURN bloom_filter_add (bloom_filter_t *filter,
									void *key)
{
	uint32_t h1 = hash1(key,filter->size,PRIME64_1);
	uint32_t h2 = hash2(key,filter->size,PRIME64_2);
	uint32_t h3 = hash3(key,filter->size,PRIME64_3);
	uint32_t h4 = hash4(key,filter->size,PRIME64_4);
	uint32_t h5 = hash5(key,filter->size,PRIME64_5);
 	uint32_t h6 = hash6(key,filter->size,PRIME64_6);
	uint32_t h7 = hash7(key,filter->size,PRIME64_6);
	filter->bloom_array.bit_array[h1] = 1;
	filter->bloom_array.bit_array[h2] = 1;
	filter->bloom_array.bit_array[h3] = 1;
	filter->bloom_array.bit_array[h4] = 1;
	filter->bloom_array.bit_array[h5] = 1;
	filter->bloom_array.bit_array[h6] = 1;
	filter->bloom_array.bit_array[h7] = 1;
	return BLOOM_FILTER_OK;
}


BLOOM_FILTER_RETURN bloom_filter_contains (bloom_filter_t *filter,void *key,int *cnt)
{ 

	uint32_t h1 = hash1(key,filter->size,PRIME64_1);
	   (*cnt)++;
	if(filter->bloom_array.bit_array[h1] != 1) return BLOOM_FILTER_NOT_OK;
	uint32_t h2 = hash2(key,filter->size,PRIME64_2);
	   (*cnt)++;
	if(filter->bloom_array.bit_array[h2] != 1) return BLOOM_FILTER_NOT_OK;
	uint32_t h3 = hash3(key,filter->size,PRIME64_3);
	   (*cnt)++;
	 if(filter->bloom_array.bit_array[h3] != 1) return BLOOM_FILTER_NOT_OK;
	uint32_t h4 = hash4(key,filter->size,PRIME64_4);
	   (*cnt)++;
	 if(filter->bloom_array.bit_array[h4] != 1) return BLOOM_FILTER_NOT_OK;
	 uint32_t h5 = hash5(key,filter->size,PRIME64_5);
	   (*cnt)++;
         if(filter->bloom_array.bit_array[h5] != 1) return BLOOM_FILTER_NOT_OK;
	uint32_t h6 = hash6(key,filter->size,PRIME64_6);
	   (*cnt)++;
         if(filter->bloom_array.bit_array[h6] != 1) return BLOOM_FILTER_NOT_OK;
	uint32_t h7 = hash7(key,filter->size,PRIME64_6);
	   (*cnt)++;
         if(filter->bloom_array.bit_array[h7] != 1) return BLOOM_FILTER_NOT_OK;
	 return BLOOM_FILTER_OK;	
}
