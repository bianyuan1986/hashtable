#!/bin/bash

set -x

gcc -O3 -g hashTable.c test_hash.c -o test_hash 
