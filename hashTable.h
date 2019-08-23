/*
 *
 *  Author: baohuren
 *  Email: baohuren@tencent.com
 *  Data: 2019-8-23
 *
 *
 *
 * This implementation can give you a chance to define any type of struct that you want to store in
 * the hash table. 
 *
 *
 */

#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#ifdef __cplusplus__
extern "C" {
#endif

struct HashTable;
typedef int (*fpCompare)(void *key1, void *key2);
typedef int (*fpHash)(void *data);
typedef void* (*mallocPtr)(size_t size);
typedef void (*freePtr)(void *ptr);

/*
 * typedef struct listElem
 * {
 *     struct listElem *next;
 *     void *key;
 *     void *value;
 * }listElem;
 *
 * If we defined the specific node type, the user can't control the struct type of the 
 * hash node. The user should malloc memory for key and value, and then pass to the
 * hash table when insert node. So we define the node type as a common type. and the 
 * user can define any node type inherited from the common type like this:
 *
 * typedef struct myElem
 * {
 *     struct listElem;
 *
 *     //add anything you want, such as the key and value info
 *     struct userData key_and_value_info;
 * }myElem;
 * 
 * User is supposed to implement fpHash and fpCompare interface. The fpHash interface
 * is used for calcute
 */
typedef struct listElem
{
	struct listElem *next;
	unsigned char data[0];
}listElem;

struct HashTable *hash_table_create(int size, fpCompare cmp, fpHash hash, mallocPtr mallocFunc, freePtr freeFunc);
int hash_table_insert(struct HashTable *htbl, void *key, void *elem);
void *hash_table_find(struct HashTable *htbl, void *key);
int hash_table_del(struct HashTable *htbl, void *key);
void hash_table_destroy(struct HashTable *htbl);

#ifdef __cplusplus__
}
#endif

#endif

