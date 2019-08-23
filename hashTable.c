#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hashTable.h"

struct HashTableOps
{
	fpCompare cmp;
	fpHash hash;
	mallocPtr mallocFunc;
	freePtr freeFunc;
};

struct HashTable
{
	listElem *bucket;
	int bucketSize;
	int nodeMax;
	int nodeCnt;

	int *statis;

	struct HashTableOps ops;
};

int primeNumber[] = {0};

int getPrime(int size)
{
	int i = 0;
	int cnt = 0;

	cnt = sizeof(primeNumber)/sizeof(primeNumber[0]);
	for( ; i < cnt; i++)
	{
		if( size < primeNumber[i] )
		{
			return primeNumber[i];
		}
	}

	return size;
}

int Init(struct HashTable *htbl, int size)
{
	htbl->nodeMax = size;
	htbl->bucketSize = size;
	htbl->bucket = htbl->ops.mallocFunc(sizeof(listElem)*htbl->bucketSize);
	if( !htbl->bucket )
	{
		printf("Malloc bucket failed.\n");
		return -1;
	}
	memset(htbl->bucket, 0x00, sizeof(listElem)*htbl->bucketSize);
	htbl->statis = htbl->ops.mallocFunc(sizeof(int)*htbl->bucketSize);
	if( !htbl->statis )
	{
		printf("Malloc statis failed.\n");
		return -1;
	}
	memset(htbl->statis, 0x00, sizeof(int)*htbl->bucketSize);

	return 0;
}

int InsertElem(struct HashTable *htbl, void *key, void *elem)
{
	int idx = 0;
	listElem *e= NULL;

	if( htbl->nodeCnt >= htbl->nodeMax )
	{
		return -1;
	}
	e = (listElem*)elem;
	idx = htbl->ops.hash(key)%htbl->bucketSize;
	e->next = htbl->bucket[idx].next;
	htbl->bucket[idx].next = e;
	htbl->nodeCnt++;
	htbl->statis[idx]++;

	return 0;
}

void *FindElem(struct HashTable *htbl, void *key)
{
	int idx = 0;
	listElem *elem = NULL;

	idx = htbl->ops.hash(key)%htbl->bucketSize;
	elem = htbl->bucket[idx].next;
	while( elem )
	{
		if( htbl->ops.cmp(elem->data, key) == 1 )
		{
			break;
		}
		elem = elem->next;
	}

	return (void*)(elem?elem->data:NULL);
}

int DelElem(struct HashTable *htbl, void *key)
{
	int idx = 0;
	listElem *elem = NULL;

	idx = htbl->ops.hash(key)%htbl->bucketSize;
	elem = htbl->bucket[idx].next;
	while( elem )
	{
		if( htbl->ops.cmp(elem->data, key) == 1 )
		{
			return 1;
		}
		elem = elem->next;
	}
	htbl->nodeCnt--;

	return 0;
}

void assessHashEfficiency(struct HashTable *htbl)
{
	int i = 0;
	int maxLen = 0;
	int bucketUsed = 0;
	double usageRate = 0.0;

	for( ; i < htbl->bucketSize; i++)
	{
		if( htbl->statis[i] > maxLen )
		{
			maxLen = htbl->statis[i];
		}
		if( htbl->statis[i] > 0 )
		{
			bucketUsed++;
		}
	}
	printf("maxLen:%d\n", maxLen);
	printf("averageLen:%f\n", (double)htbl->nodeCnt/(double)bucketUsed);
	printf("usageRate:%f\n", (double)bucketUsed/(double)htbl->bucketSize);
}

int hash_table_del(struct HashTable *htbl, void *key)
{
	return DelElem(htbl, key);
}

void *hash_table_find(struct HashTable *htbl, void *key)
{
	if( !htbl || !key )
	{
		return NULL;
	}
	return FindElem(htbl, key);
}

int hash_table_insert(struct HashTable *htbl, void *key, void *elem)
{
	if( !htbl || !key || !elem )
	{
		return -1;
	}
	return InsertElem(htbl, key, elem);
}

struct HashTable *hash_table_create(int size, fpCompare cmp, fpHash hash, mallocPtr mallocFunc, freePtr freeFunc)
{
	struct HashTable *htbl = NULL;
	int ret = 0;

	if( mallocFunc )
	{
		htbl = (struct HashTable*)mallocFunc(sizeof(struct HashTable));
	}
	else
	{
		htbl = (struct HashTable*)malloc(sizeof(struct HashTable));
	}

	if( !htbl )
	{
		printf("Malloc Hash Table failed\n");
		goto FAILED;
	}
	htbl->ops.cmp = cmp;
	htbl->ops.hash = hash;
	htbl->ops.mallocFunc = mallocFunc?mallocFunc:malloc;
	htbl->ops.freeFunc = freeFunc?freeFunc:free;

	ret = Init(htbl, size);
	if( ret < 0 )
	{
		printf("Init Hash Table failed\n");
		goto FAILED;
	}

	return htbl;

FAILED:
	if( htbl )
	{
		hash_table_destroy(htbl);
	}
	return NULL;
}

void hash_table_destroy(struct HashTable *htbl)
{
    htbl->ops.freeFunc(htbl->bucket);	
	htbl->ops.freeFunc(htbl);
}


