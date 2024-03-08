#pragma once

#include <stddef.h>
#include <stdint.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include "hashtable.h"

#define MAX_LEVEL 16 // level of skiplist
#define P_FACTOR (RAND_MAX >> 2) // 0.25 factor of node to present in higher level

struct SKLNode
{
    int level;
    SKLNode** next; // array of next nodes in every level
};

struct ZSet
{
    SKLNode *head = NULL;
    HMap hmap;
    int max_level;
};

struct ZNode
{
    SKLNode sklnode;
    HNode hmap;
    double score = 0;
    size_t len = 0;
    char name[0];
};

bool zset_add(ZSet *zset, const char *name, size_t len, double score);
ZNode *zset_lookup(ZSet *zset, const char *name, size_t len);
ZNode *zset_pop(ZSet *zset, const char *name, size_t len);
ZNode *zset_query(ZSet *zset, double score, const char *name, size_t len);
void zset_dispose(ZSet *zset);
ZNode *znode_offset(ZNode *node, int64_t offset);
void znode_del(ZNode *node);
ZSet* zset_init();
void zset_destroy(ZSet* zset);
