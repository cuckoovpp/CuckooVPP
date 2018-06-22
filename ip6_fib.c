/*
 * Copyright (c) 2016 Cisco and/or its affiliates.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <vnet/fib/ip6_fib.h>
#include <vnet/fib/fib_table.h>
#include <time.h>
#include <sys/time.h>
struct timeval t0,t1;
static int cuckoo_lookups = 0;
static int bloom_lookups = 0;
static int fib_lookups = 0;
static int cuckoo_lookups1 = 0;
static int bloom_lookups1 = 0;
static int fib_lookups1 = 0;
static int fib_lookups12 = 0;
static int fib_lookups11 = 0;
static f64 time_taken = 0;
static f64 time_taken1 = 0;
static f64 time_taken2 = 0;
static f64 time_taken21 = 0;
static f64 time_taken23 = 0;
static f64 time_taken3 = 0;
static f64 time_taken11 = 0;
static f64 time_taken31 = 0;
static int c_routes = 3000000;
static int b_routes = 5000000;
static double bloom_prob = 0.1;
static int bloom_k = 2;
static bool use_cuckoo = false;
static bool use_bloom = false; 
static bool use_m =false;
static int counter=0;
static int blm_cnt=0;
static int cuc_cnt=0;
static int threshold=500000;
static int cont=0;
static int kk=0;
static int aa[500000]={0};
static int bb[500000]={0};
static f64 tt[500000]={0};
static int last_adr1,last_adr2;
typedef enum { NO_FILTER = 0, BLOOM_FILTER = 1, CUCKOO_FILTER = 2, } FILTER_TYPE;

static clib_error_t *
use_bloom_filter (vlib_main_t * vm,
                 unformat_input_t * input,
                 vlib_cli_command_t * cmd)
{
	if(!use_bloom) {
		
		use_bloom = true;
	}
	else use_bloom = false;
	return 0;
}

static clib_error_t *
use_cuckoo_filter (vlib_main_t * vm,
                 unformat_input_t * input,
                 vlib_cli_command_t * cmd)
{
	if(!use_cuckoo) {
		use_cuckoo = true;
		
	}
	else use_cuckoo = false;
	return 0;
}

static clib_error_t *
use_mtrie (vlib_main_t * vm,
                 unformat_input_t * input,
                 vlib_cli_command_t * cmd)
{
        if(use_m) {
                use_m = false;

        }
        else {
        use_m = true;
        }

return 0;
}

static clib_error_t *
show_number_of_lookups (vlib_main_t * vm,
                 unformat_input_t * input,
                 vlib_cli_command_t * cmd)
{
	vlib_cli_output (vm, "\n==========FWD Output===========\nBloom lookups : %d \n Cuckoo lookups : %d \n Fib lookups : %d \ntime taken by filter : %.17g\n Time taken by FIB=%.17g\n Total Time=%.17g \n Counter: %d \n cont: %d", 
			 bloom_lookups, cuckoo_lookups, fib_lookups,time_taken,time_taken2,(time_taken+time_taken2),counter,cont);
	
	vlib_cli_output (vm, "\n------------\nTrie lookups = %d\n Time to trie lookup= %.17g\nTime taken by FIB= %.17g \nTotal Time= %.17g\n",fib_lookups1,time_taken1,time_taken3,(time_taken1+time_taken3));
  vlib_cli_output (vm, "\n==========NFWD Output===========\nBloom lookups : %d \n Cuckoo lookups : %d \n Fib lookups : %d \ntime taken by filter : %.17g\n Time taken by FIB=%.17g\n Total Time=%.17g\n",  bloom_lookups1, cuckoo_lookups1, fib_lookups12,time_taken11,time_taken31,(time_taken11+time_taken31));
        
        vlib_cli_output (vm, "\n------------\nTrie lookups = %d\n Time to trie lookup= %.17g\nTime taken by FIB= %.17g \nTotal Time= %.17g\n",fib_lookups11,time_taken21,time_taken23,(time_taken21+time_taken23));
int i;

vlib_cli_output (vm, "\n Last address at NFWD= %x",last_adr1);
vlib_cli_output (vm, "\n Last address at FWD= %x",last_adr2);
vlib_cli_output (vm, "\n Number of Bloom hash lookup= %d",blm_cnt);
vlib_cli_output (vm, "\n Number of Cuckoo hash lookup= %d",cuc_cnt);
FILE *fp=fopen("/home/krishna/prefix_1.txt","w+");
for(i=0;i<500000;i++)
fprintf(fp,"%d\n",aa[i]);

fclose(fp);


	return 0;
}

static clib_error_t *
clear_lookups (vlib_main_t * vm,
                 unformat_input_t * input,
                 vlib_cli_command_t * cmd)
{
	cuckoo_lookups = 0;
	bloom_lookups = 0;
	fib_lookups = 0;
	time_taken = 0;
	 time_taken1 = 0;
 	time_taken2 = 0;
	time_taken3 = 0;
	time_taken21 = 0;
	time_taken23 = 0;
	time_taken11 = 0;
	time_taken31 = 0;
	fib_lookups1 = 0;
	fib_lookups11=0;
	cuckoo_lookups1 = 0;
        bloom_lookups1 = 0;
        fib_lookups12 = 0;
	counter=0;
	cont=0;
	kk=0;
	blm_cnt=0;
	cuc_cnt=0;
 

	return 0;
}


static void
vnet_ip6_fib_init (u32 fib_index)
{
    fib_prefix_t pfx = {
	.fp_proto = FIB_PROTOCOL_IP6,
	.fp_len = 0,
	.fp_addr = {
	    .ip6 = {
		{ 0, 0, },
	    },
	}
    };

    /*
     * Add the default route.
     */
    fib_table_entry_special_add(fib_index,
				&pfx,
				FIB_SOURCE_DEFAULT_ROUTE,
				FIB_ENTRY_FLAG_DROP,
				ADJ_INDEX_INVALID);

    /*
     * all link local for us
     */
    pfx.fp_addr.ip6.as_u64[0] = clib_host_to_net_u64 (0xFE80000000000000ULL);
    pfx.fp_addr.ip6.as_u64[1] = 0;
    pfx.fp_len = 10;
    fib_table_entry_special_add(fib_index,
				&pfx,
				FIB_SOURCE_SPECIAL,
				FIB_ENTRY_FLAG_LOCAL,
				ADJ_INDEX_INVALID);
}

static u32
create_fib_with_table_id (u32 table_id)
{
    fib_table_t *fib_table;
    ip6_fib_t *v6_fib;

    pool_get_aligned(ip6_main.fibs, fib_table, CLIB_CACHE_LINE_BYTES);
    pool_get_aligned(ip6_main.v6_fibs, v6_fib, CLIB_CACHE_LINE_BYTES);

    memset(fib_table, 0, sizeof(*fib_table));
    memset(v6_fib, 0, sizeof(*v6_fib));

    ASSERT((fib_table - ip6_main.fibs) ==
           (v6_fib - ip6_main.v6_fibs));
    
    fib_table->ft_proto = FIB_PROTOCOL_IP6;
    fib_table->ft_index =
	    v6_fib->index =
                (fib_table - ip6_main.fibs);

    hash_set(ip6_main.fib_index_by_table_id, table_id, fib_table->ft_index);
	
	// Cuckoo code starts
	cuckoo_filter_new(&v6_fib->cuckoo_filter,c_routes,500);
	// Cuckoo code ends
	// Bloom code starts 
	bloom_filter_new(&v6_fib->bloom_filter, b_routes, bloom_prob, bloom_k, b_routes);
    // Bloom code ends
	
    fib_table->ft_table_id =
	v6_fib->table_id =
	    table_id;
    fib_table->ft_flow_hash_config = IP_FLOW_HASH_DEFAULT;
	
	vnet_ip6_fib_init(fib_table->ft_index);
    fib_table_lock(fib_table->ft_index, FIB_PROTOCOL_IP6);
	
    return (fib_table->ft_index);
}

u32
ip6_fib_table_find_or_create_and_lock (u32 table_id)
{
    uword * p;

    p = hash_get (ip6_main.fib_index_by_table_id, table_id);
    if (NULL == p)
	return create_fib_with_table_id(table_id);
    
    fib_table_lock(p[0], FIB_PROTOCOL_IP6);

    return (p[0]);
}

u32
ip6_fib_table_create_and_lock (void)
{
    return (create_fib_with_table_id(~0));
}

void
ip6_fib_table_destroy (u32 fib_index)
{
    fib_prefix_t pfx = {
	.fp_proto = FIB_PROTOCOL_IP6,
	.fp_len = 0,
	.fp_addr = {
	    .ip6 = {
		{ 0, 0, },
	    },
	}
    };

    /*
     * the default route.
     */
    fib_table_entry_special_remove(fib_index,
				   &pfx,
				   FIB_SOURCE_DEFAULT_ROUTE);


    /*
     * ff02::1:ff00:0/104
     */
    ip6_set_solicited_node_multicast_address(&pfx.fp_addr.ip6, 0);
    pfx.fp_len = 104;
    fib_table_entry_special_remove(fib_index,
				   &pfx,
				   FIB_SOURCE_SPECIAL);

    /*
     * all-routers multicast address
     */
    ip6_set_reserved_multicast_address (&pfx.fp_addr.ip6,
					IP6_MULTICAST_SCOPE_link_local,
					IP6_MULTICAST_GROUP_ID_all_routers);
    pfx.fp_len = 128;
    fib_table_entry_special_remove(fib_index,
				   &pfx,
				   FIB_SOURCE_SPECIAL);

    /*
     * all-nodes multicast address
     */
    ip6_set_reserved_multicast_address (&pfx.fp_addr.ip6,
					IP6_MULTICAST_SCOPE_link_local,
					IP6_MULTICAST_GROUP_ID_all_hosts);
    pfx.fp_len = 128;
    fib_table_entry_special_remove(fib_index,
				   &pfx,
				   FIB_SOURCE_SPECIAL);

    /*
     * all-mldv2 multicast address
     */
    ip6_set_reserved_multicast_address (&pfx.fp_addr.ip6,
					IP6_MULTICAST_SCOPE_link_local,
					IP6_MULTICAST_GROUP_ID_mldv2_routers);
    pfx.fp_len = 128;
    fib_table_entry_special_remove(fib_index,
				   &pfx,
				   FIB_SOURCE_SPECIAL);

    /*
     * all link local 
     */
    pfx.fp_addr.ip6.as_u64[0] = clib_host_to_net_u64 (0xFE80000000000000ULL);
    pfx.fp_addr.ip6.as_u64[1] = 0;
    pfx.fp_len = 10;
    fib_table_entry_special_remove(fib_index,
				   &pfx,
				   FIB_SOURCE_SPECIAL);

	/*
	 * Remove cuckoo memory
	 */
	ip6_fib_t *v6_fib;
   	v6_fib = &ip6_main.v6_fibs[fib_index];
   	cuckoo_filter_free(&v6_fib->cuckoo_filter);
	
	/*
	 * validate no more routes.
	 */
	fib_table_t *fib_table = fib_table_get(fib_index, FIB_PROTOCOL_IP6);
    fib_source_t source;
	ASSERT(0 == fib_table->ft_total_route_counts);
    FOR_EACH_FIB_SOURCE(source)
    {
	ASSERT(0 == fib_table->ft_src_route_counts[source]);
    }

    if (~0 != fib_table->ft_table_id)
    {
	hash_unset (ip6_main.fib_index_by_table_id, fib_table->ft_table_id);
    }
    pool_put_index(ip6_main.v6_fibs, fib_table->ft_index);
    pool_put(ip6_main.fibs, fib_table);
}

fib_node_index_t
ip6_fib_table_lookup (u32 fib_index,
		      const ip6_address_t *addr,
		      u32 len)
{
    const ip6_fib_table_instance_t *table;
    BVT(clib_bihash_kv) kv, value;
    int i, n_p, rv;
    u64 fib;
	u64 adr[2];
	ip6_fib_t *v6_fib;
	v6_fib = &ip6_main.v6_fibs[fib_index];
	vlib_main_t *vm = &vlib_global_main;

    table = &ip6_main.ip6_table[IP6_FIB_TABLE_NON_FWDING];
    n_p = vec_len (table->prefix_lengths_in_search_order);

    kv.key[0] = addr->as_u64[0];
    kv.key[1] = addr->as_u64[1];
    fib = ((u64)((fib_index))<<32);
   adr[0]=addr->as_u64[0];
   adr[1]=addr->as_u64[1];

	f64 befor,befor1;

	f64 delt1,delt2;
    /*
     * start search from a mask length same length or shorter.
     * we don't want matches longer than the mask passed
     */

if(use_m) 
{
        //trie lookup
 	befor = vlib_time_now(vm);
      	// gettimeofday(&t0, 0);
	 int dt_address_length  = lookup_in_trie(adr);
	delt1 = vlib_time_now(vm) - befor;
       //gettimeofday(&t1, 0);
	if(kk<500000){
	aa[kk]=dt_address_length;
	bb[kk]=adr[0];
	tt[kk]=delt1;
	}
	
	kk++;
       time_taken21 +=delt1; 
	 fib_lookups11++;
	ip6_address_t * mask = &ip6_main.fib_masks[dt_address_length];
        ASSERT(dt_address_length >= 0 && dt_address_length <= 128);
        //As lengths are decreasing, masks are increasingly specific.
        kv.key[0] &= mask->as_u64[0];
        kv.key[1] &= mask->as_u64[1];
        kv.key[2] = fib | dt_address_length;

	gettimeofday(&t0, 0);

         rv = BV(clib_bihash_search_inline_2)(&table->ip6_hash, &kv, &value);

     if (rv == 0){
		   
			gettimeofday(&t1, 0);
                    time_taken23 += (t1.tv_sec* 1e6 + t1.tv_usec- t0.tv_sec*1e6 - t0.tv_usec)/1e6;
		return value.value;
            }
 		  
		    gettimeofday(&t1, 0);
                    time_taken23 += (t1.tv_sec* 1e6 + t1.tv_usec- t0.tv_sec*1e6 - t0.tv_usec)/1e6;
}

else{

	i = 0;	
	for (; i < n_p; i++)
        {
        int dst_address_length = table->prefix_lengths_in_search_order[i];
        ip6_address_t * mask = &ip6_main.fib_masks[dst_address_length];
        ASSERT(dst_address_length >= 0 && dst_address_length <= 128);
        //As lengths are decreasing, masks are increasingly specific.
        kv.key[0] &= mask->as_u64[0];
        kv.key[1] &= mask->as_u64[1];
        kv.key[2] = fib | dst_address_length;

if(use_cuckoo) {

	 cuckoo_lookups1++;
	
	befor = vlib_time_now(vm);

    	if (cuckoo_filter_contains(v6_fib->cuckoo_filter, kv.key, 16, dst_address_length,&cuc_cnt) == CUCKOO_FILTER_OK) {
	
	delt1 = vlib_time_now(vm) - befor;
                time_taken11 += delt1;
	fib_lookups12++;
	 befor1 = vlib_time_now(vm) ;
        rv = BV(clib_bihash_search_inline_2)(&table->ip6_hash, &kv, &value);
        if (rv == 0)
        {
		delt2 = vlib_time_now(vm) - befor1;
                time_taken31 += delt2;
                return value.value;
        }
		delt2 = vlib_time_now(vm) - befor1;
                time_taken31 += delt2;

	}

	else{
 		delt1 = vlib_time_now(vm) - befor;
                time_taken11 += delt1;
	   }
}

else if(use_bloom) {

	bloom_lookups1++;

	befor = vlib_time_now(vm);


	if(bloom_filter_contains(v6_fib->bloom_filter, kv.key,&blm_cnt) == BLOOM_FILTER_OK) {
		 delt1 = vlib_time_now(vm) - befor;
                time_taken11 += delt1;
		fib_lookups12++;
         befor1 = vlib_time_now(vm) ;
        rv = BV(clib_bihash_search_inline_2)(&table->ip6_hash, &kv, &value);
        if (rv == 0)
        {
                delt2 = vlib_time_now(vm) - befor1;
                time_taken31 += delt2;
                return value.value;
        }
                delt2 = vlib_time_now(vm) - befor1;
                time_taken31 += delt2;

        }

        else{
                delt1 = vlib_time_now(vm) - befor;
                time_taken11 += delt1;
           }
}


else {

	fib_lookups12++;
	
	befor = vlib_time_now(vm);

	rv = BV(clib_bihash_search_inline_2)(&table->ip6_hash, &kv, &value);

	if (rv == 0)
	{
		delt2 = vlib_time_now(vm) - befor;
                time_taken31 += delt2;  
		return value.value;
    	}
 		delt2 = vlib_time_now(vm) - befor;
                time_taken31 += delt2;

	}

   }

 
}

  return (FIB_NODE_INDEX_INVALID);
}

fib_node_index_t
ip6_fib_table_lookup_exact_match (u32 fib_index,
				  const ip6_address_t *addr,
				  u32 len)
{
    const ip6_fib_table_instance_t *table;
    BVT(clib_bihash_kv) kv, value;
    ip6_address_t *mask;
    u64 fib;
    int rv;

    table = &ip6_main.ip6_table[IP6_FIB_TABLE_NON_FWDING];
    mask = &ip6_main.fib_masks[len];
    fib = ((u64)((fib_index))<<32);

    kv.key[0] = addr->as_u64[0] & mask->as_u64[0];
    kv.key[1] = addr->as_u64[1] & mask->as_u64[1];
    kv.key[2] = fib | len;
      
    rv = BV(clib_bihash_search_inline_2)(&table->ip6_hash, &kv, &value);
    if (rv == 0)
	return value.value;

    return (FIB_NODE_INDEX_INVALID);
}

static void
compute_prefix_lengths_in_search_order (ip6_fib_table_instance_t *table)
{
    int i;
    vec_reset_length (table->prefix_lengths_in_search_order);
    /* Note: bitmap reversed so this is in fact a longest prefix match */
    clib_bitmap_foreach (i, table->non_empty_dst_address_length_bitmap,
    ({
	int dst_address_length = 128 - i;
	vec_add1(table->prefix_lengths_in_search_order, dst_address_length);
    }));
}

void
ip6_fib_table_entry_remove (u32 fib_index,
			    const ip6_address_t *addr,
			    u32 len)
{
    ip6_fib_table_instance_t *table;
    BVT(clib_bihash_kv) kv;
    ip6_address_t *mask;
    u64 fib;

    table = &ip6_main.ip6_table[IP6_FIB_TABLE_NON_FWDING];
    mask = &ip6_main.fib_masks[len];
    fib = ((u64)((fib_index))<<32);

    kv.key[0] = addr->as_u64[0] & mask->as_u64[0];
    kv.key[1] = addr->as_u64[1] & mask->as_u64[1];
    kv.key[2] = fib | len;

    BV(clib_bihash_add_del)(&table->ip6_hash, &kv, 0);

    /* refcount accounting */
    ASSERT (table->dst_address_length_refcounts[len] > 0);
    if (--table->dst_address_length_refcounts[len] == 0)
    {
	table->non_empty_dst_address_length_bitmap =
            clib_bitmap_set (table->non_empty_dst_address_length_bitmap, 
                             128 - len, 0);
	compute_prefix_lengths_in_search_order (table);
    }
}

void
ip6_fib_table_entry_insert (u32 fib_index,
			    const ip6_address_t *addr,
			    u32 len,
			    fib_node_index_t fib_entry_index)
{
    ip6_fib_table_instance_t *table;
    BVT(clib_bihash_kv) kv;
    ip6_address_t *mask;
    u64 fib;
	ip6_fib_t *v6_fib;

    table = &ip6_main.ip6_table[IP6_FIB_TABLE_NON_FWDING];
    mask = &ip6_main.fib_masks[len];
    fib = ((u64)((fib_index))<<32);
	v6_fib = &ip6_main.v6_fibs[fib_index];


    kv.key[0] = addr->as_u64[0] & mask->as_u64[0];
    kv.key[1] = addr->as_u64[1] & mask->as_u64[1];
    kv.key[2] = fib | len;
    kv.value = fib_entry_index;
	last_adr1=kv.key[0];
    BV(clib_bihash_add_del)(&table->ip6_hash, &kv, 1);

    table->dst_address_length_refcounts[len]++;

    table->non_empty_dst_address_length_bitmap =
        clib_bitmap_set (table->non_empty_dst_address_length_bitmap, 
			 128 - len, 1);
    compute_prefix_lengths_in_search_order (table);
	
	//filter insertion 	
	cuckoo_filter_add(v6_fib->cuckoo_filter, kv.key,16,len);
	bloom_filter_add(v6_fib->bloom_filter, kv.key);

	// trie insertion
	
   	insert_route_in_trie(kv.key,len);
 
}

u32 
ip6_fib_table_fwding_lookup (ip6_main_t * im,
                             u32 fib_index,
                             const ip6_address_t * dst)
{
   const ip6_fib_table_instance_t *table;
   ip6_fib_t *v6_fib;
    int i, len;
     int rv;
   f64 before,before1;

   f64 delta,delta1;
   
   vlib_main_t *vm = &vlib_global_main;

    BVT(clib_bihash_kv) kv, value;
    u64 fib;
    u64 adr[2];
	
    table = &ip6_main.ip6_table[IP6_FIB_TABLE_FWDING];
    len = vec_len (table->prefix_lengths_in_search_order);
    v6_fib = &ip6_main.v6_fibs[fib_index];
	
    kv.key[0] = dst->as_u64[0];
    kv.key[1] = dst->as_u64[1];
   
    adr[0]=dst->as_u64[0];
    adr[1]=dst->as_u64[1];

    fib = ((u64)((fib_index))<<32);

	
if(use_m) 
{

	counter++;
	if(counter==threshold)
	cont=1;

                if(cont==0){

		before= vlib_time_now(vm);
		

        //trie lookup
       int dt_address_length  = lookup_in_trie(adr);
 	delta = vlib_time_now(vm) - before;
                time_taken1 += delta;
	if(kk<500000){
	aa[kk]=dt_address_length;
	bb[kk]=adr[0];
	tt[kk]=delta;
	}
	kk++;	
       fib_lookups1++;
        ip6_address_t * mask = &ip6_main.fib_masks[dt_address_length];
        ASSERT(dt_address_length >= 0 && dt_address_length <= 128);
        //As lengths are decreasing, masks are increasingly specific.
        kv.key[0] &= mask->as_u64[0];
        kv.key[1] &= mask->as_u64[1];
        kv.key[2] = fib | dt_address_length;
	

			before1= vlib_time_now(vm);
                        rv = BV(clib_bihash_search_inline_2)(&table->ip6_hash, &kv, &value);

                        if (rv == 0){
                                delta1 = vlib_time_now(vm) - before1;
                                time_taken3 += delta1;
                                return value.value;
                        }
                        
                        delta1 = vlib_time_now(vm) - before1;
                                time_taken3 += delta1;
	}
}


else {

if(use_cuckoo) {
    	
		 
	counter++;
	if(counter==threshold)
        	cont=1;


   for (i=0; i < len; i++)
    {
        int dst_address_length = table->prefix_lengths_in_search_order[i];
        ip6_address_t * mask = &ip6_main.fib_masks[dst_address_length];
      
        ASSERT(dst_address_length >= 0 && dst_address_length <= 128);
        //As lengths are decreasing, masks are increasingly specific.
        kv.key[0] &= mask->as_u64[0];
        kv.key[1] &= mask->as_u64[1];
        kv.key[2] = fib | dst_address_length;


               if(cont==0){


	    	  cuckoo_lookups++;

		  before= vlib_time_now(vm);
 
		if (cuckoo_filter_contains(v6_fib->cuckoo_filter, kv.key, 16, table->prefix_lengths_in_search_order[i],&cuc_cnt) ==
			CUCKOO_FILTER_OK) {
			fib_lookups++;
		 delta = vlib_time_now(vm) - before;
                time_taken += delta;

			 before1= vlib_time_now(vm);
			rv = BV(clib_bihash_search_inline_2)(&table->ip6_hash, &kv, &value);

			if (rv == 0)
			{
				delta1 = vlib_time_now(vm) - before1;
                                time_taken2 += delta1;

				return value.value;
    			}
			
			delta1 = vlib_time_now(vm) - before1;
                                time_taken2 += delta1;
		}

		else{
		delta = vlib_time_now(vm) - before;
                time_taken += delta;
		}


	   }
	}
}


	else if(use_bloom) {
	
	         counter++;
		 if(counter==threshold)      
		  cont=1;



   for (i=0; i < len; i++)
    {
        int dst_address_length = table->prefix_lengths_in_search_order[i];
        ip6_address_t * mask = &ip6_main.fib_masks[dst_address_length];

        ASSERT(dst_address_length >= 0 && dst_address_length <= 128);
        //As lengths are decreasing, masks are increasingly specific.
        kv.key[0] &= mask->as_u64[0];
        kv.key[1] &= mask->as_u64[1];
        kv.key[2] = fib | dst_address_length;



                if(cont==0){

		 bloom_lookups++;
                before= vlib_time_now(vm);

		if (bloom_filter_contains(v6_fib->bloom_filter, kv.key,&blm_cnt) ==
			BLOOM_FILTER_OK) {
			fib_lookups++;
			delta = vlib_time_now(vm) - before;
                	time_taken += delta;


			before1= vlib_time_now(vm);
                        rv = BV(clib_bihash_search_inline_2)(&table->ip6_hash, &kv, &value);

                        if (rv == 0)
                        {
                                delta1 = vlib_time_now(vm) - before1;
                                time_taken2 += delta1;

                                return value.value;
                        }
                        
                        delta1 = vlib_time_now(vm) - before1;
                                time_taken2 += delta1;
		}

		else{
		delta = vlib_time_now(vm) - before;
                time_taken += delta;
		}
		

	     }
       }

    }

    else {
		       counter++;

		if(counter==threshold)
        		cont=1;

   for (i=0; i < len; i++)
    {
        int dst_address_length = table->prefix_lengths_in_search_order[i];
        ip6_address_t * mask = &ip6_main.fib_masks[dst_address_length];

        ASSERT(dst_address_length >= 0 && dst_address_length <= 128);
        //As lengths are decreasing, masks are increasingly specific.
        kv.key[0] &= mask->as_u64[0];
        kv.key[1] &= mask->as_u64[1];
        kv.key[2] = fib | dst_address_length;



                if(cont==0){


		fib_lookups++;
		before = vlib_time_now(vm);
		rv = BV(clib_bihash_search_inline_2)(&table->ip6_hash, &kv, &value);
		

		if (rv == 0)
		{	 delta = vlib_time_now(vm) - before;
                         time_taken2 += delta;

			return value.value;
        	}

		 delta = vlib_time_now(vm) - before;
                         time_taken2 += delta;


   	   }

    	}
  }

}

  ASSERT(0);

    return 0;
}



u32 ip6_fib_table_fwding_lookup_with_if_index (ip6_main_t * im,
					       u32 sw_if_index,
					       const ip6_address_t * dst)
{
    u32 fib_index = vec_elt (im->fib_index_by_sw_if_index, sw_if_index);
    return ip6_fib_table_fwding_lookup(im, fib_index, dst);
}

flow_hash_config_t
ip6_fib_table_get_flow_hash_config (u32 fib_index)
{
    return (ip6_fib_get(fib_index)->flow_hash_config);
}

u32
ip6_fib_table_get_index_for_sw_if_index (u32 sw_if_index)
{
    if (sw_if_index >= vec_len(ip6_main.fib_index_by_sw_if_index))
    {
	/*
	 * This is the case for interfaces that are not yet mapped to
	 * a IP table
	 */
	return (~0);
    }
    return (ip6_main.fib_index_by_sw_if_index[sw_if_index]);
}

void
ip6_fib_table_fwding_dpo_update (u32 fib_index,
				 const ip6_address_t *addr,
				 u32 len,
				 const dpo_id_t *dpo)
{
    ip6_fib_table_instance_t *table;
    BVT(clib_bihash_kv) kv;
    ip6_address_t *mask;
    u64 fib;

    table = &ip6_main.ip6_table[IP6_FIB_TABLE_FWDING];
    mask = &ip6_main.fib_masks[len];
    fib = ((u64)((fib_index))<<32);

    kv.key[0] = addr->as_u64[0] & mask->as_u64[0];
    kv.key[1] = addr->as_u64[1] & mask->as_u64[1];
    kv.key[2] = fib | len;
    kv.value = dpo->dpoi_index;
	last_adr2=kv.key[0];	
    BV(clib_bihash_add_del)(&table->ip6_hash, &kv, 1);

    table->dst_address_length_refcounts[len]++;

    table->non_empty_dst_address_length_bitmap =
        clib_bitmap_set (table->non_empty_dst_address_length_bitmap, 
			 128 - len, 1);
    compute_prefix_lengths_in_search_order (table);

}

void
ip6_fib_table_fwding_dpo_remove (u32 fib_index,
				 const ip6_address_t *addr,
				 u32 len,
				 const dpo_id_t *dpo)
{
    ip6_fib_table_instance_t *table;
    BVT(clib_bihash_kv) kv;
    ip6_address_t *mask;
    u64 fib;

    table = &ip6_main.ip6_table[IP6_FIB_TABLE_FWDING];
    mask = &ip6_main.fib_masks[len];
    fib = ((u64)((fib_index))<<32);

    kv.key[0] = addr->as_u64[0] & mask->as_u64[0];
    kv.key[1] = addr->as_u64[1] & mask->as_u64[1];
    kv.key[2] = fib | len;
    kv.value = dpo->dpoi_index;

    BV(clib_bihash_add_del)(&table->ip6_hash, &kv, 0);

    /* refcount accounting */
    ASSERT (table->dst_address_length_refcounts[len] > 0);
    if (--table->dst_address_length_refcounts[len] == 0)
    {
	table->non_empty_dst_address_length_bitmap =
            clib_bitmap_set (table->non_empty_dst_address_length_bitmap,
                             128 - len, 0);
	compute_prefix_lengths_in_search_order (table);
    }
}

/**
 * @brief Context when walking the IPv6 table. Since all VRFs are in the
 * same hash table, we need to filter only those we need as we walk
 */
typedef struct ip6_fib_walk_ctx_t_
{
    u32 i6w_fib_index;
    fib_table_walk_fn_t i6w_fn;
    void *i6w_ctx;
} ip6_fib_walk_ctx_t;

static int
ip6_fib_walk_cb (clib_bihash_kv_24_8_t * kvp,
                 void *arg)
{
    ip6_fib_walk_ctx_t *ctx = arg;

    if ((kvp->key[2] >> 32) == ctx->i6w_fib_index)
    {
        ctx->i6w_fn(kvp->value, ctx->i6w_ctx);
    }

    return (1);
}

void
ip6_fib_table_walk (u32 fib_index,
                    fib_table_walk_fn_t fn,
                    void *arg)
{
    ip6_fib_walk_ctx_t ctx = {
        .i6w_fib_index = fib_index,
        .i6w_fn = fn,
        .i6w_ctx = arg,
    };
    ip6_main_t *im = &ip6_main;

    BV(clib_bihash_foreach_key_value_pair)(&im->ip6_table[IP6_FIB_TABLE_NON_FWDING].ip6_hash,
					   ip6_fib_walk_cb,
					   &ctx);

}

typedef struct ip6_fib_show_ctx_t_ {
    fib_node_index_t *entries;
} ip6_fib_show_ctx_t;

static int
ip6_fib_table_show_walk (fib_node_index_t fib_entry_index,
                         void *arg)
{
    ip6_fib_show_ctx_t *ctx = arg;

    vec_add1(ctx->entries, fib_entry_index);

    return (1);
}

static void
ip6_fib_table_show_all (ip6_fib_t *fib,
			vlib_main_t * vm)
{
    fib_node_index_t *fib_entry_index;
    ip6_fib_show_ctx_t ctx = {
	.entries = NULL,
    };

    ip6_fib_table_walk(fib->index, ip6_fib_table_show_walk, &ctx);
    vec_sort_with_function(ctx.entries, fib_entry_cmp_for_sort);

    vec_foreach(fib_entry_index, ctx.entries)
    {
	vlib_cli_output(vm, "%U",
                        format_fib_entry,
                        *fib_entry_index,
                        FIB_ENTRY_FORMAT_BRIEF);
    }

    vec_free(ctx.entries);
}

static void
ip6_fib_table_show_one (ip6_fib_t *fib,
			vlib_main_t * vm,
			ip6_address_t *address,
			u32 mask_len
            )
{
    vlib_cli_output(vm, "%U",
                    format_fib_entry,
                    ip6_fib_table_lookup(fib->index, address, mask_len),
                    FIB_ENTRY_FORMAT_DETAIL);
}

typedef struct {
  u32 fib_index;
  u64 count_by_prefix_length[129];
} count_routes_in_fib_at_prefix_length_arg_t;

static void
count_routes_in_fib_at_prefix_length (BVT(clib_bihash_kv) * kvp,
                                      void *arg)
{
  count_routes_in_fib_at_prefix_length_arg_t * ap = arg;
  int mask_width;

  if ((kvp->key[2]>>32) != ap->fib_index)
    return;

  mask_width = kvp->key[2] & 0xFF;

  ap->count_by_prefix_length[mask_width]++;
}

static clib_error_t *
ip6_show_fib (vlib_main_t * vm,
	      unformat_input_t * input,
	      vlib_cli_command_t * cmd)
{
    count_routes_in_fib_at_prefix_length_arg_t _ca, *ca = &_ca;
    ip6_main_t * im6 = &ip6_main;
    fib_table_t *fib_table;
    ip6_fib_t * fib;
    int verbose, matching;
    ip6_address_t matching_address;
    u32 mask_len  = 128;
    int table_id = -1, fib_index = ~0;
    
    verbose = 1;
    matching = 0;

    while (unformat_check_input (input) != UNFORMAT_END_OF_INPUT)
    {
	if (unformat (input, "brief")   ||
	    unformat (input, "summary") ||
	    unformat (input, "sum"))
	    verbose = 0;
 
	else if (unformat (input, "%U/%d",
			   unformat_ip6_address, &matching_address, &mask_len))
	    matching = 1;

	else if (unformat (input, "%U", unformat_ip6_address, &matching_address))
	    matching = 1;

	else if (unformat (input, "table %d", &table_id))
	    ;
	else if (unformat (input, "index %d", &fib_index))
	    ;
	else
	    break;
    }

    pool_foreach (fib_table, im6->fibs,
    ({
	fib = pool_elt_at_index(im6->v6_fibs, fib_table->ft_index);
	if (table_id >= 0 && table_id != (int)fib->table_id)
	    continue;
	if (fib_index != ~0 && fib_index != (int)fib->index)
	    continue;

	vlib_cli_output (vm, "%s, fib_index %d, flow hash: %U", 
			 fib_table->ft_desc, fib->index,
			 format_ip_flow_hash_config, fib->flow_hash_config);

	/* Show summary? */
	if (! verbose)
	{
	    BVT(clib_bihash) * h = &im6->ip6_table[IP6_FIB_TABLE_NON_FWDING].ip6_hash;
	    int len;

	    vlib_cli_output (vm, "%=20s%=16s", "Prefix length", "Count");

	    memset (ca, 0, sizeof(*ca));
	    ca->fib_index = fib->index;

	    BV(clib_bihash_foreach_key_value_pair)
		(h, count_routes_in_fib_at_prefix_length, ca);

	    for (len = 128; len >= 0; len--)
            {
		if (ca->count_by_prefix_length[len])
		    vlib_cli_output (vm, "%=20d%=16lld", 
				     len, ca->count_by_prefix_length[len]);
            }
	    continue;
	}

	if (!matching)
	{
	    ip6_fib_table_show_all(fib, vm);
	}
	else
	{
	    ip6_fib_table_show_one(fib, vm, &matching_address, mask_len);
	}
    }));

    return 0;
}
 

/*?
 * This command displays the IPv6 FIB Tables (VRF Tables) and the route
 * entries for each table.
 *
 * @note This command will run for a long time when the FIB tables are
 * comprised of millions of entries. For those senarios, consider displaying
 * in summary mode.
 *
 * @cliexpar
 * @parblock
 * Example of how to display all the IPv6 FIB tables:
 * @cliexstart{show ip6 fib}
 * ipv6-VRF:0, fib_index 0, flow hash: src dst sport dport proto
 * @::/0
 *   unicast-ip6-chain
 *   [@0]: dpo-load-balance: [index:5 buckets:1 uRPF:5 to:[0:0]]
 *     [0] [@0]: dpo-drop ip6
 * fe80::/10
 *   unicast-ip6-chain
 *   [@0]: dpo-load-balance: [index:10 buckets:1 uRPF:10 to:[0:0]]
 *     [0] [@2]: dpo-receive
 * ff02::1/128
 *   unicast-ip6-chain
 *   [@0]: dpo-load-balance: [index:8 buckets:1 uRPF:8 to:[0:0]]
 *     [0] [@2]: dpo-receive
 * ff02::2/128
 *   unicast-ip6-chain
 *   [@0]: dpo-load-balance: [index:7 buckets:1 uRPF:7 to:[0:0]]
 *     [0] [@2]: dpo-receive
 * ff02::16/128
 *   unicast-ip6-chain
 *   [@0]: dpo-load-balance: [index:9 buckets:1 uRPF:9 to:[0:0]]
 *     [0] [@2]: dpo-receive
 * ff02::1:ff00:0/104
 *   unicast-ip6-chain
 *   [@0]: dpo-load-balance: [index:6 buckets:1 uRPF:6 to:[0:0]]
 *     [0] [@2]: dpo-receive
 * ipv6-VRF:8, fib_index 1, flow hash: src dst sport dport proto
 * @::/0
 *   unicast-ip6-chain
 *   [@0]: dpo-load-balance: [index:21 buckets:1 uRPF:20 to:[0:0]]
 *     [0] [@0]: dpo-drop ip6
 * @::a:1:1:0:4/126
 *   unicast-ip6-chain
 *   [@0]: dpo-load-balance: [index:27 buckets:1 uRPF:26 to:[0:0]]
 *     [0] [@4]: ipv6-glean: af_packet0
 * @::a:1:1:0:7/128
 *   unicast-ip6-chain
 *   [@0]: dpo-load-balance: [index:28 buckets:1 uRPF:27 to:[0:0]]
 *     [0] [@2]: dpo-receive: @::a:1:1:0:7 on af_packet0
 * fe80::/10
 *   unicast-ip6-chain
 *   [@0]: dpo-load-balance: [index:26 buckets:1 uRPF:25 to:[0:0]]
 *     [0] [@2]: dpo-receive
 * fe80::fe:3eff:fe3e:9222/128
 *   unicast-ip6-chain
 *   [@0]: dpo-load-balance: [index:29 buckets:1 uRPF:28 to:[0:0]]
 *     [0] [@2]: dpo-receive: fe80::fe:3eff:fe3e:9222 on af_packet0
 * ff02::1/128
 *   unicast-ip6-chain
 *   [@0]: dpo-load-balance: [index:24 buckets:1 uRPF:23 to:[0:0]]
 *     [0] [@2]: dpo-receive
 * ff02::2/128
 *   unicast-ip6-chain
 *   [@0]: dpo-load-balance: [index:23 buckets:1 uRPF:22 to:[0:0]]
 *     [0] [@2]: dpo-receive
 * ff02::16/128
 *   unicast-ip6-chain
 *   [@0]: dpo-load-balance: [index:25 buckets:1 uRPF:24 to:[0:0]]
 *     [0] [@2]: dpo-receive
 * ff02::1:ff00:0/104
 *   unicast-ip6-chain
 *   [@0]: dpo-load-balance: [index:22 buckets:1 uRPF:21 to:[0:0]]
 *     [0] [@2]: dpo-receive
 * @cliexend
 *
 * Example of how to display a summary of all IPv6 FIB tables:
 * @cliexstart{show ip6 fib summary}
 * ipv6-VRF:0, fib_index 0, flow hash: src dst sport dport proto
 *     Prefix length         Count
 *          128                3
 *          104                1
 *          10                 1
 *           0                 1
 * ipv6-VRF:8, fib_index 1, flow hash: src dst sport dport proto
 *     Prefix length         Count
 *          128                5
 *          126                1
 *          104                1
 *          10                 1
 *           0                 1
 * @cliexend
 * @endparblock
 ?*/
/* *INDENT-OFF* */

 /*VLIB_REGISTER_THREAD (cuckoo_read_reg, static) = {
	 .name = "cuckoo-filter-read",
	 .function = cuckoo_read_thread_fn,
	 .fixed_count = 1,
	 .count = 1,
	 .no_data_structure_clone = 1,
 };
*/

VLIB_CLI_COMMAND (ip6_show_fib_command, static) = {
    .path = "show ip6 fib",
    .short_help = "show ip6 fib [summary] [table <table-id>] [index <fib-id>] [<ip6-addr>[/<width>]]",
    .function = ip6_show_fib,
};

/*
VLIB_CLI_COMMAND (ip6_show_cuckoo_command, static) = {
       .path = "show cuckoo_filter",
       .short_help = "show cuckoo_filter [<ip6-addr>[/<width>]]",
       .function = show_cuckoo_filter,
};
*/

VLIB_CLI_COMMAND (ip6_show_lookups_command, static) = {
       .path = "show lookups",
       .short_help = "show lookups",
       .function = show_number_of_lookups,
};

VLIB_CLI_COMMAND (clear_lookups_command, static) = {
       .path = "clear lookups",
       .short_help = "clear lookups counters",
       .function = clear_lookups,
};

VLIB_CLI_COMMAND (set_cuckoo_command, static) = {
       .path = "set cuckoo",
       .short_help = "set cuckoo",
       .function = use_cuckoo_filter,
};

VLIB_CLI_COMMAND (set_bloom_command, static) = {
       .path = "set bloom",
       .short_help = "set bloom",
       .function = use_bloom_filter,
};

VLIB_CLI_COMMAND (set_mtrie_command, static) = {
       .path = "set mtrie",
       .short_help = "set mtrie",
       .function = use_mtrie,
};

/* *INDENT-ON* */
