/*
 * cache.c
 */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "cache.h"
#include "helper.h"
FILE* cfile;
/* cache configuration parameters */
static int cache_split = 1;
static int cache_usize = DEFAULT_CACHE_SIZE;
static int cache_isize = DEFAULT_CACHE_SIZE; 
static int cache_dsize = DEFAULT_CACHE_SIZE;
static int cache_block_size = DEFAULT_CACHE_BLOCK_SIZE;
static int words_per_block = DEFAULT_CACHE_BLOCK_SIZE / WORD_SIZE;
static int cache_assoc = DEFAULT_CACHE_ASSOC;
static int cache_writeback = DEFAULT_CACHE_WRITEBACK;
static int cache_writealloc = DEFAULT_CACHE_WRITEALLOC;

/* cache model data structures */
static Pcache icache;
static Pcache dcache;
static cache c_data;
static cache c_inst;
static cache_stat cache_stat_inst;
static cache_stat cache_stat_data;

/************************************************************/
void set_cache_param(param, value)
  int param;
  int value;
{

  switch (param) {
  case CACHE_PARAM_BLOCK_SIZE:
    cache_block_size = value;
    words_per_block = value / WORD_SIZE;
    break;
  case CACHE_PARAM_USIZE:
    cache_split = FALSE;
    cache_usize = value;
    break;
  case CACHE_PARAM_ISIZE:
    cache_split = TRUE;
    cache_isize = value;
    break;
  case CACHE_PARAM_DSIZE:
    cache_split = TRUE;
    cache_dsize = value;
    break;
  case CACHE_PARAM_ASSOC:
    cache_assoc = value;
    break;
  case CACHE_PARAM_WRITEBACK:
    cache_writeback = TRUE;
    break;
  case CACHE_PARAM_WRITETHROUGH:
    cache_writeback = FALSE;
    break;
  case CACHE_PARAM_WRITEALLOC:
    cache_writealloc = TRUE;
    break;
  case CACHE_PARAM_NOWRITEALLOC:
    cache_writealloc = FALSE;
    break;
  default:
    printf("error set_cache_param: bad parameter value\n");
    exit(-1);
  }

}
/************************************************************/

/************************************************************/
void init_cache()
{
	if(!cache_split){
	  c_data.size = cache_usize;			 
	  c_data.associativity = cache_assoc;		
	  int set_size = c_data.associativity * cache_block_size;
	  c_data.n_sets = cache_usize/set_size;
	  //printf("no of sets -> %d\n", c_data.n_sets);
	  int temp = LOG2(cache_block_size) + LOG2(c_data.n_sets);
	  c_data.index_mask = (pow(2,temp) - 1);		
	  c_data.index_mask_offset = LOG2(cache_block_size);	
	  c_data.LRU_head = (Pcache_line *)malloc(sizeof(Pcache_line)*c_data.n_sets);	
	  c_data.LRU_tail = (Pcache_line *)malloc(sizeof(Pcache_line)*c_data.n_sets);
	  c_data.set_contents = (int *)malloc(sizeof(int)*c_data.n_sets);		
	}
	else{
	  c_data.size = cache_dsize;			 
	  c_data.associativity = cache_assoc;		
	  int set_size = c_data.associativity * cache_block_size;
	  c_data.n_sets = cache_dsize/set_size;
	  //printf("no of sets -> %d\n", c_data.n_sets);
	  int temp = LOG2(cache_block_size) + LOG2(c_data.n_sets);
	  c_data.index_mask = (pow(2,temp) - 1);		
	  c_data.index_mask_offset = LOG2(cache_block_size);	
	  c_data.LRU_head = (Pcache_line *)malloc(sizeof(Pcache_line)*c_data.n_sets);	
	  c_data.LRU_tail = (Pcache_line *)malloc(sizeof(Pcache_line)*c_data.n_sets);
	  c_data.set_contents = (int *)malloc(sizeof(int)*c_data.n_sets);		

	  c_inst.size = cache_isize;			 
	  c_inst.associativity = cache_assoc;		
	  set_size = c_inst.associativity * cache_block_size;
	  c_inst.n_sets = cache_isize/set_size;
	 // printf("no of sets -> %d\n", c_inst.n_sets);
	  temp = LOG2(cache_block_size) + LOG2(c_inst.n_sets);
	  c_inst.index_mask = (pow(2,temp) - 1);		
	  c_inst.index_mask_offset = LOG2(cache_block_size);	
	  c_inst.LRU_head = (Pcache_line *)malloc(sizeof(Pcache_line)*c_inst.n_sets);	
	  c_inst.LRU_tail = (Pcache_line *)malloc(sizeof(Pcache_line)*c_inst.n_sets);
	  c_inst.set_contents = (int *)malloc(sizeof(int)*c_inst.n_sets);		
	}  
  cache_stat_data.accesses = 0;			
  cache_stat_data.misses = 0;			
  cache_stat_data.replacements = 0;		
  cache_stat_data.demand_fetches = 0;	
  cache_stat_data.copies_back = 0;
  
  cache_stat_inst.accesses = 0;			
  cache_stat_inst.misses = 0;			
  cache_stat_inst.replacements = 0;		
  cache_stat_inst.demand_fetches = 0;	
  cache_stat_inst.copies_back = 0;
}
/************************************************************/

/************************************************************/
void perform_access(addr, access_type)
  unsigned addr, access_type;
{
	//printf("addr -> %d\n",addr);
	if(!cache_split){
		unsigned index;
		index = (addr & c_data.index_mask) >> c_data.index_mask_offset;
		unsigned current_tag = (addr - (addr & c_data.index_mask)) >> (LOG2(cache_block_size) + LOG2(c_data.n_sets));
		int hit = 0;
		int i = 0;
		Pcache_line current = c_data.LRU_head[index];
		while(current != NULL){
			i++;
			if(current->tag == current_tag){
				hit = 1;
				break;
			}
			current = current->LRU_next;
		}
		if( i > c_data.associativity ){
			printf("***** Fatal error.. This cannot happen\n *****");
		}
		if(hit == 1){
			delete(&c_data.LRU_head[index],&c_data.LRU_tail[index], current);
			current->LRU_next = NULL;
			current->LRU_prev = NULL;
			Pcache_line item = (Pcache_line)malloc(sizeof(Pcache_line));
			item->tag = current_tag;
			item->dirty = current->dirty;
			current = NULL;
			if(access_type == 1){
				item->dirty = 1;
			}
			item->LRU_next = NULL;
			item->LRU_prev = NULL;
			insert(&c_data.LRU_head[index],&c_data.LRU_tail[index], item);
			item  = NULL;
		}
		else{
			if(access_type == 2){
				cache_stat_inst.misses++;
			}
			else{
				cache_stat_data.misses++;
			}
			Pcache_line item;
			item = (Pcache_line)malloc(sizeof(Pcache_line));
			item->tag = current_tag;
			if( access_type == 0 || access_type == 2){
				item->dirty = 0;
			}
			else{
				item->dirty = 1;
			}
			item->LRU_next = NULL;
			item->LRU_prev = NULL;
			if(!cache_writealloc && access_type == 1 && cache_writeback){
				cache_stat_data.copies_back = cache_stat_data.copies_back + 1;
			}
			else{
				if(access_type == 1 && !cache_writealloc){

				}
				else{
					if(i < c_data.associativity )
					{
						insert(&c_data.LRU_head[index],&c_data.LRU_tail[index], item);
						item = NULL;
						c_data.set_contents[index]++;
					}
					else{
						Pcache_line last = c_data.LRU_tail[index];
						if(cache_writeback){
							if(last->dirty == 1){
								cache_stat_data.copies_back = cache_stat_data.copies_back + words_per_block;
							} 
						}
						delete(&c_data.LRU_head[index],&c_data.LRU_tail[index], last);
						if(access_type == 2){
							cache_stat_inst.replacements = cache_stat_inst.replacements + 1;
						}
						else{
							cache_stat_data.replacements = cache_stat_data.replacements + 1;
						}
						last->LRU_next = NULL;
						last->LRU_prev = NULL;
						last = NULL;
						insert(&c_data.LRU_head[index],&c_data.LRU_tail[index], item);
						item = NULL;
					}
				}
			}
		}
		if(access_type == 0){
			cache_stat_data.accesses++;
			if( hit == 0){
				cache_stat_data.demand_fetches = cache_stat_data.demand_fetches + words_per_block; 
			}
		}
		else if(access_type == 1){
			cache_stat_data.accesses++;
			if( hit == 0 && cache_writealloc){
				cache_stat_data.demand_fetches = cache_stat_data.demand_fetches + words_per_block;
			}
			if(!cache_writeback && cache_writealloc){
				cache_stat_data.copies_back = cache_stat_data.copies_back + 1;	
			}
		}
		else{
			cache_stat_inst.accesses++;
			if( hit == 0){
				cache_stat_inst.demand_fetches = cache_stat_inst.demand_fetches + words_per_block;
			}
		}
	}
	else{
		if(access_type == 0 || access_type == 1){
			unsigned index;
			index = (addr & c_data.index_mask) >> c_data.index_mask_offset;
			unsigned current_tag = (addr - (addr & c_data.index_mask)) >> (LOG2(cache_block_size) + LOG2(c_data.n_sets));
			int hit = 0;
			int i = 0;
			Pcache_line current = c_data.LRU_head[index];
			while(current != NULL){
				i++;
				if(current->tag == current_tag){
					hit = 1;
					break;
				}
				current = current->LRU_next;
			}
			if( i > c_data.associativity ){
				printf("***** Fatal error.. This cannot happen\n *****");
			}
			if(hit == 1){
				delete(&c_data.LRU_head[index],&c_data.LRU_tail[index], current);
				current->LRU_next = NULL;
				current->LRU_prev = NULL;
				Pcache_line item = (Pcache_line)malloc(sizeof(Pcache_line));
				item->tag = current_tag;
				item->dirty = current->dirty;
				current = NULL;
				if(access_type == 1){
					item->dirty = 1;
				}
				item->LRU_next = NULL;
				item->LRU_prev = NULL;
				insert(&c_data.LRU_head[index],&c_data.LRU_tail[index], item);
				item  = NULL;
			}
			else{
				if(access_type == 2){
				}
				else{
					cache_stat_data.misses++;
				}
				Pcache_line item;
				item = (Pcache_line)malloc(sizeof(Pcache_line));
				item->tag = current_tag;
				if( access_type == 0 || access_type == 2){
					item->dirty = 0;
				}
				else{
					item->dirty = 1;
				}
				item->LRU_next = NULL;
				item->LRU_prev = NULL;
				if(!cache_writealloc && access_type == 1 && cache_writeback){
					cache_stat_data.copies_back = cache_stat_data.copies_back + 1;
				}
				else{
					if(access_type == 1 && !cache_writealloc){

					}
					else{
						if(i < c_data.associativity )
						{
							insert(&c_data.LRU_head[index],&c_data.LRU_tail[index], item);
							item = NULL;
							c_data.set_contents[index]++;
						}
						else{
							Pcache_line last = c_data.LRU_tail[index];
							if(cache_writeback){
								if(last->dirty == 1){
									cache_stat_data.copies_back = cache_stat_data.copies_back + words_per_block;
								} 
							}
							delete(&c_data.LRU_head[index],&c_data.LRU_tail[index], last);
							if(access_type == 2){
							}
							else{
								cache_stat_data.replacements = cache_stat_data.replacements + 1;
							}
							last->LRU_next = NULL;
							last->LRU_prev = NULL;
							last = NULL;
							insert(&c_data.LRU_head[index],&c_data.LRU_tail[index], item);
							item = NULL;
						}
					}
				}
			}
			if(access_type == 0){
				cache_stat_data.accesses++;
				if( hit == 0){
					cache_stat_data.demand_fetches = cache_stat_data.demand_fetches + words_per_block; 
				}
			}
			else if(access_type == 1){
				cache_stat_data.accesses++;
				if( hit == 0 && cache_writealloc){
					cache_stat_data.demand_fetches = cache_stat_data.demand_fetches + words_per_block;
				}
				if(!cache_writeback && cache_writealloc){
					cache_stat_data.copies_back = cache_stat_data.copies_back + 1;	
				}
			}
		}
		else{
			unsigned index;
			index = (addr & c_inst.index_mask) >> c_inst.index_mask_offset;
			unsigned current_tag = (addr - (addr & c_inst.index_mask)) >> (LOG2(cache_block_size) + LOG2(c_inst.n_sets));
			int hit = 0;
			int i = 0;
			Pcache_line current = c_inst.LRU_head[index];
			while(current != NULL){
				i++;
				if(current->tag == current_tag){
					hit = 1;
					break;
				}
				current = current->LRU_next;
			}
			if( i > c_inst.associativity ){
				printf("***** Fatal error.. This cannot happen\n *****");
			}
			if(hit == 1){
				delete(&c_inst.LRU_head[index],&c_inst.LRU_tail[index], current);
				current->LRU_next = NULL;
				current->LRU_prev = NULL;
				Pcache_line item = (Pcache_line)malloc(sizeof(Pcache_line));
				item->tag = current_tag;
				item->dirty = current->dirty;
				current = NULL;
				if(access_type == 1){
					item->dirty = 1;
				}
				item->LRU_next = NULL;
				item->LRU_prev = NULL;
				insert(&c_inst.LRU_head[index],&c_inst.LRU_tail[index], item);
				item  = NULL;
			}
			else{
				if(access_type == 2){
					cache_stat_inst.misses++;
				}
				else{
				}
				Pcache_line item;
				item = (Pcache_line)malloc(sizeof(Pcache_line));
				item->tag = current_tag;
				if( access_type == 0 || access_type == 2){
					item->dirty = 0;
				}
				else{
					item->dirty = 1;
				}
				item->LRU_next = NULL;
				item->LRU_prev = NULL;
				if(!cache_writealloc && access_type == 1 && cache_writeback){
				}
				else{
					if(access_type == 1 && !cache_writealloc){

					}
					else{
						if(i < c_inst.associativity )
						{
							insert(&c_inst.LRU_head[index],&c_inst.LRU_tail[index], item);
							item = NULL;
						}
						else{
							Pcache_line last = c_inst.LRU_tail[index];
							if(cache_writeback){
								if(last->dirty == 1){
								} 
							}
							delete(&c_inst.LRU_head[index],&c_inst.LRU_tail[index], last);
							if(access_type == 2){
								cache_stat_inst.replacements = cache_stat_inst.replacements + 1;
							}
							else{
							}
							last->LRU_next = NULL;
							last->LRU_prev = NULL;
							last = NULL;
							insert(&c_inst.LRU_head[index],&c_inst.LRU_tail[index], item);
							item = NULL;
						}
					}
				}
			}
			if(access_type == 0){
			}
			else if(access_type == 1){
			}
			else{
				cache_stat_inst.accesses++;
				if( hit == 0){
					cache_stat_inst.demand_fetches = cache_stat_inst.demand_fetches + words_per_block;
				}
			}
		}
	}
	
}
/************************************************************/

/************************************************************/
void flush()
{

  /* flush the cache */
  int i = 0;
  for(i = 0; i < c_data.n_sets;i++){
	  Pcache_line current_line = c_data.LRU_head[i];
	  Pcache_line current_line1 = c_data.LRU_tail[i];
	  if(c_data.LRU_head[i] == NULL){
		  continue;
	  }
	  else{
	  	Pcache_line runner = current_line;
	  	while(runner != NULL){
	  		if(runner->dirty == 1 && cache_writeback){
	  			cache_stat_data.copies_back = cache_stat_data.copies_back + words_per_block;
		  	}
		  	runner = runner->LRU_next;
	  	}
	  	current_line = NULL;
	  	c_data.LRU_head[i] = NULL;
	  }
	  current_line1 = NULL;
	  c_data.LRU_tail[i] = NULL;
  }
  /*if(c_data.LRU_head[455]== NULL){
		printf("Correct\n");
	}
*/
}
/************************************************************/

/************************************************************/
void delete(head, tail, item)
  Pcache_line *head, *tail;
  Pcache_line item;
{
  if (item->LRU_prev != NULL) {
	  item->LRU_prev->LRU_next = item->LRU_next;
  } else {  
    /* item at head */
    *head = item->LRU_next;
  }
  if (item->LRU_next != NULL) {
	  item->LRU_next->LRU_prev = item->LRU_prev;
  } else {
    /* item at tail */
	*tail = item->LRU_prev;
  }
}
/************************************************************/
/************************************************************/
/* inserts at the head of the list */
void insert(head, tail, item)
  Pcache_line *head, *tail;
  Pcache_line item;
{
  item->LRU_next = *head;
  item->LRU_prev = NULL;
  if (item->LRU_next != NULL)
    item->LRU_next->LRU_prev = item;
  else
    *tail = item;

  *head = item;
}
/************************************************************/

/************************************************************/
void dump_settings()
{
  printf("*** CACHE SETTINGS ***\n");
  if (cache_split) {
    printf("  Split I- D-cache\n");
    printf("  I-cache size: \t%d\n", cache_isize);
    printf("  D-cache size: \t%d\n", cache_dsize);
  } else {
    printf("  Unified I- D-cache\n");
    printf("  Size: \t%d\n", cache_usize);
  }
  printf("  Associativity: \t%d\n", cache_assoc);
  printf("  Block size: \t%d\n", cache_block_size);
  printf("  Write policy: \t%s\n", 
	 cache_writeback ? "WRITE BACK" : "WRITE THROUGH");
  printf("  Allocation policy: \t%s\n",
	 cache_writealloc ? "WRITE ALLOCATE" : "WRITE NO ALLOCATE");
}
/************************************************************/

/************************************************************/
void print_stats(FILE ** out)
{
	dump_settings();
	FILE * output = *out;
  //fprintf(output,"\n*** CACHE STATISTICS ***\n");

	fprintf(output,"Cache Summary\n");
	fprintf(output,"Cache L1-I\n");
	fprintf(output,"num cache accesses,%d\n",cache_stat_inst.accesses);
	fprintf(output,"num cache misses,%d\n",cache_stat_inst.misses);
	if (!cache_stat_inst.accesses)
    fprintf(output,"  miss rate: 0 (0)\n"); 
  	else
    fprintf(output,"  miss rate, %2.4f%%\n", 
	 ((float)cache_stat_inst.misses / (float)cache_stat_inst.accesses)*100.0);
	fprintf(output,"Cache L1-D\n");
	fprintf(output,"num data misses,%d\n",cache_stat_data.misses);
	if (!cache_stat_data.accesses)
    fprintf(output,"  miss rate: 0 (0)\n"); 
  	else
    fprintf(output,"  miss rate, %2.4f%%\n", 
	 ((float)cache_stat_data.misses / (float)cache_stat_data.accesses)*100.0);
	fprintf(output, "DRAM Summary\n");
	fprintf(output,"num dram accesses,%d\n",cache_stat_inst.accesses + cache_stat_data.accesses);
	fprintf(output,"average dram access latency (ns),%d\n", 45);


  
}
/************************************************************/
void setvalue(){
	int flag = 0, i = -10;
	char frequency[10];
	char latency[10];
	char string[40];
	while(!feof(cfile)){
		fscanf(cfile, "%s" , string);
		//printf("%s\n", string);
		//printf("pppppppppppp\n");
		if(strcmp(string, "=") == 0)
			continue;
		if(strcmp(string, "[perf_model/l1_icache]") == 0){
			flag = 1;
			continue;
		}
		else if(strcmp(string, "[perf_model/l1_dcache]") == 0){
			flag = 2;
			continue;
		}
		else if(strcmp(string, "[perf_model/core]") == 0){
			flag = 3;
			continue;
		}
		else if(strcmp(string, "[perf_model/dram]") == 0){
			flag = 4;
			continue;
		}
		if(flag == 1){
			if(i == 0)
				{}
			else if(i == 1)
				set_cache_param(CACHE_PARAM_ISIZE, atoi(string));
			else if(i == 2)
				set_cache_param(CACHE_PARAM_ASSOC, atoi(string));
			else if(i == 3){
				if(strcmp(string, "lru") != 0){
					printf("ONLY LRU REPLACEMENT POLICY IMPLEMENTED\n");
					exit(0);
				}
			}
			else if(i == 4){
				if(atoi(string) == 0){
					set_cache_param(CACHE_PARAM_WRITEBACK, 1);
				}
				else
					set_cache_param(CACHE_PARAM_WRITETHROUGH, 1);	
			}
				
			else if(i == 5)
				set_cache_param(CACHE_PARAM_BLOCK_SIZE, atoi(string));
			i = -10;
		}
		else if(flag == 2){
			if(i == 0)
				{}
			else if(i == 1)
				set_cache_param(CACHE_PARAM_DSIZE, atoi(string));
			else if(i == 2)
				set_cache_param(CACHE_PARAM_ASSOC, atoi(string));
			else if(i == 3){
				if(strcmp(string, "lru") != 0){
					printf("ONLY LRU REPLACEMENT POLICY IMPLEMENTED\n");
					exit(0);
				}
			}
			else if(i == 4){
				if(atoi(string) == 0){
					set_cache_param(CACHE_PARAM_WRITEBACK, 1);
				}
				else
					set_cache_param(CACHE_PARAM_WRITETHROUGH, 1);	
			}
				
			else if(i == 5)
				set_cache_param(CACHE_PARAM_BLOCK_SIZE, atoi(string));
			i = -10;
		}
		/*else if(flag == 3){
			if(i == 6)
				strcpy(frequency, string);
			i = -10;
		}
		else if(flag == 4){
			if(i == 7)
				strcpy(latency, string);
			i = -10;
		}*/
		if(flag == 1 || flag == 2 || flag == 3 || flag == 4){
			if(strcmp(string, "perfect") == 0)
				i = 0;
			else if(strcmp(string, "cache_size") == 0)
				i = 1;
			else if(strcmp(string, "associativity") == 0)
				i = 2;
			else if(strcmp(string, "replacement_policy") == 0)
				i = 3;
			else if(strcmp(string, "writethrough") == 0)
				i = 4;
			else if(strcmp(string, "block_size") == 0)
				i = 5;
			else if(strcmp(string, "frequency") == 0)
				i = 6;
			else if(strcmp(string, "latency") == 0)
				i = 7;
			else
				i = -10;
		}
	}
}
