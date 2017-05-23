#include "cachelab.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>

int hitcounter = 0;
int misscounter = 0;
int evictcounter = 0;

typedef struct{
	int validbit;
	int tag;
	int lru;
}Line;

typedef struct{
	Line *lines; //array of lines is a set
}Set;

typedef struct{
	int linecount;
	int setcount;
	Set *sets; //array of sets in cache
}Cache;

int tagBits(int adr, int s, int b){
	return (0x7FFFFFFF >> (31 - b - s)) & (adr >> (b + s));
}

int setBits(int adr, int s, int b){
	return (0x7FFFFFFF >> (31 - s)) & (adr >> b);
}

int refreshLRU(Cache *cache, int set, int line){
	for (int i = 0; i < cache->linecount; i++){
		if (cache->sets[set].lines[i].validbit &&
		(cache->sets[set].lines[i].lru > cache->sets[set].lines[line].lru))
			cache->sets[set].lines[i].lru--;
	}
	cache->sets[set].lines[line].lru = cache->linecount;
	return 0;
}

void scan(Cache *cache, int s, int E, int b, int adr, char id, int sz){
	int set = setBits(adr, s, b);
	int tag = tagBits(adr, s, b);
	for(int i = 0; i < cache->linecount; i++){
		if(cache->sets[set].lines[i].validbit && cache->sets[set].lines[i].tag == tag){
			hitcounter++;
			if(id == 'M') 
				hitcounter++;
			refreshLRU(cache, set, i);
			return;
		}
	}
	//printf("Miss counter increment.\n");
	misscounter++;
	for(int i = 0; i < cache->linecount; i++){
		if(!cache->sets[set].lines[i].validbit){
			cache->sets[set].lines[i].validbit = 1;
			cache->sets[set].lines[i].tag = tag;
			refreshLRU(cache, set, i);
			if(id == 'M')
				hitcounter++;
			return;
		}
	}
	//printf("Evict counter increment.\n");
	evictcounter++;
	for(int i = 0; i < cache->linecount; i++){
		if(cache->sets[set].lines[i].lru == 1){
			cache->sets[set].lines[i].validbit = 1;
			cache->sets[set].lines[i].tag = tag;
			refreshLRU(cache, set, i);
			if(id == 'M')
				hitcounter++;
			return;
		}
	}
	return;
}

int main(int argc, char **argv)
{
	int b = 0;
	int s = 0;
	int e = 0;
//	printf("Location 0"); //used for debugging
	char *t = NULL;
	opterr = 0;
//	printf("Location 1");
	int cases;
//	int cases = getopt(argc, argv, "s:E:b:t:");
	while ((cases = getopt(argc, argv, "s:E:b:t:")) != -1){
		switch (cases){
			case 'b':
				b = atoi(optarg);
				break;
			case 's':
				s = atoi(optarg);
				break;
			case 'E':
				e = atoi(optarg);
				break;
			case 't':
				t = optarg;
				break;
			default:
				exit(0);
		}
	}
//	printf("Location 2");
	Cache *cache = (Cache *)malloc(sizeof(Cache));
	cache->linecount = e;
	cache->setcount = (2 << s);
	cache->sets = (Set *)malloc(sizeof(Set) * cache->setcount);
	for (int i = 0; i < cache->setcount; i++){
		cache->sets[i].lines = malloc(sizeof(Line) * cache->linecount);
		for (int j = 0; j < cache->linecount; j++){
//			Line temp = cache->sets[i].lines[j]; // for some reason if I replace the lines below with temp, it still says variable unused
			cache->sets[i].lines[j].validbit = 0;
			cache->sets[i].lines[j].tag = 0;
			cache->sets[i].lines[j].lru = 0;
		}
	}
//	printf("Location 3");
	FILE *trace = fopen(t, "r");
//	printf("Location 4");
	char id;
	int adr;
	int sz;
//	printf("Location 5");
	while (fscanf(trace, " %c %x,%d", &id, &adr, &sz) > 0){ //%x for hex
		if (id != 'I' && id != ' '){ // due to the weird spacing of the I
			scan(cache, s, e, b, adr, id, sz);
		}
	}
//	printf("Location 6");
	fclose(trace);
	free(cache);
    printSummary(hitcounter, misscounter, evictcounter);
    return 0;
}