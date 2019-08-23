#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "hashTable.h"

typedef struct
{
	uint16_t family;
	union
	{
		uint32_t v4;
		uint32_t v6[4];
	}addr;
}ipAddrType;

typedef struct
{
	listElem link;
	ipAddrType ip;
}sIpElem;

int ipCompare(void *key1, void *key2)
{
	ipAddrType *addr1 = NULL;
	ipAddrType *addr2 = NULL;

	addr1 = (ipAddrType*)key1;
	addr2 = (ipAddrType*)key2;

	if( addr1->family == addr2->family && addr1->addr.v6[0] == addr2->addr.v6[0] &&
											addr1->addr.v6[1] == addr2->addr.v6[1] &&
											addr1->addr.v6[2] == addr2->addr.v6[2] &&
											addr1->addr.v6[3] == addr2->addr.v6[3] )
	{
		return 1;
	}

	return 0;
}

int ipHash(void *data)
{
	ipAddrType *addr1 = NULL;

	addr1 = (ipAddrType*)data;
	if( addr1->family == AF_INET )
	{
		return ((unsigned char*)&addr1->addr.v4)[0];
	}
	else if( addr1->family == AF_INET6 )
	{
		return ((unsigned char*)&addr1->addr.v4)[0];
	}
}

/***************************************************************************************************************************************************/

typedef struct
{
	listElem link;
	unsigned char ip[INET6_ADDRSTRLEN];
}sIpStrElem;

/*elfhash algorithm*/
int ipStrHash(void *data)
{
	unsigned char *str = NULL;
	unsigned int hash = 0;
	unsigned int x = 0;

	str = (unsigned char*)data;
	while( *str )
	{
		hash = (hash<<4) + *str;
		if( (x=hash & 0xf0000000) != 0 )
		{
			hash ^= (x>>24);
			hash &= ~x;
		}
		str++;
	}

	return (hash & 0x7fffffff);
}

int ipStrCompare(void *key1, void *key2)
{
	const char *str1 = NULL;
	const char *str2 = NULL;

	str1 = (const char *)key1;
	str2 = (const char *)key2;

	if( strcmp(str1, str2) == 0 )
	{
		return 1; 
	}

	return 0;
}

/***************************************************************************************************************************************************/

int getIpType(const char *ip)
{
	int i = 0;

	while( ip[i] != '\0' )
	{
		if( ip[i] == ':' )
		{
			return AF_INET6;
		}
		i++;
	}

	return AF_INET;
}

void testIntHashTable(struct HashTable *htbl)
{
	char *ip_str[] = {"192.168.0.1", "192.168.32.1", "2001:0:0:fe80:20c:29ff:fe00:63c5", "2001:0:0:fe80:20c:29ff:fe00:63c4", "192.168.33.1"};
	ipAddrType *addr = NULL;
	sIpElem *elem = NULL;
	int i = 0;
	int cnt = 0;

	cnt = sizeof(ip_str)/sizeof(ip_str[0]);
	for( ; i < cnt; i++)
	{
		elem = malloc(sizeof(sIpElem));
		memset(elem, 0x00, sizeof(elem));
		addr = &elem->ip;
		addr->family = getIpType(ip_str[i]);
		if( inet_pton(addr->family, ip_str[i], addr->addr.v6) < 0 )
		{
			printf("Convert ip address [%s] failed.\n", ip_str[i]);
			continue;
		}
		hash_table_insert(htbl, addr, elem);
	}

	if( hash_table_find(htbl, addr) )
	{
		printf("Find ip:[%s]\n", ip_str[i-1]);
	}
	else
	{
		printf("Not find ip:[%s]\n", ip_str[i-1]);
	}
}

void testStrHashTable(struct HashTable *htbl)
{
	char *ip_str[] = {"192.168.0.1", "192.168.32.1", "2001:0:0:fe80:20c:29ff:fe00:63c5", "2001:0:0:fe80:20c:29ff:fe00:63c4", "192.168.33.1"};
	sIpStrElem *elem = NULL;
	int i = 0;
	int cnt = 0;

	cnt = sizeof(ip_str)/sizeof(ip_str[0]);
	for( ; i < cnt; i++)
	{
		elem = malloc(sizeof(sIpStrElem));
		memset(elem, 0x00, sizeof(elem));
		strncpy(elem->ip, ip_str[i], INET6_ADDRSTRLEN-1);
		hash_table_insert(htbl, elem->ip, elem);
	}

	if( hash_table_find(htbl, elem->ip) )
	{
		printf("Find ip:[%s]\n", ip_str[i-1]);
	}
	else
	{
		printf("Not find ip:[%s]\n", ip_str[i-1]);
	}
}

int main(int argc, char *argv[])
{
	struct HashTable *intHtbl = NULL;
	struct HashTable *strHtbl = NULL;

	intHtbl = hash_table_create(7, ipCompare, ipHash, NULL, NULL); 
	testIntHashTable(intHtbl);
	assessHashEfficiency(intHtbl);

	strHtbl = hash_table_create(7, ipStrCompare, ipStrHash, NULL, NULL); 
	testStrHashTable(strHtbl);
	assessHashEfficiency(strHtbl);

	return 0;
}


