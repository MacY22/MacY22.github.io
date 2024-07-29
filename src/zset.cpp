#include <assert.h>
#include <string.h>
#include <stdlib.h>
// proj
#include "../include/zset.h"
#include "../include/common.h"

int random_level()
{
    int lv = 1;
    while (rand() < P_FACTOR && lv < MAX_LEVEL)
    {
        lv++;
    }
    return lv;
}

void skl_init(SKLNode* node, int lv)
{
    node->level = lv;
    node->next = (SKLNode**)malloc(sizeof(SKLNode*) * lv);
    for (int i = 0; i < lv; i++)
    {
        node->next[i] = NULL;
    }
}

void skl_destroy(SKLNode* node)
{
    free(node->next);
}

static ZNode *znode_new(const char *name, size_t len, double score)
{
    ZNode *node = (ZNode *)malloc(sizeof(ZNode) + len);
    assert(node);   // not a good idea in real projects
    int lv = random_level();
    skl_init(&node->sklnode, lv);
    node->hmap.next = NULL;
    node->hmap.hcode = str_hash((uint8_t *)name, len);
    node->score = score;
    node->len = len;
    memcpy(&node->name[0], name, len);
    return node;
}

static uint32_t min(size_t lhs, size_t rhs)
{
    return lhs < rhs ? lhs : rhs;
}

// compare by the (score, name) tuple
// less ? true : false
static bool zless(
    SKLNode *lhs, double score, const char *name, size_t len)
{
    ZNode *zl = container_of(lhs, ZNode, sklnode);
    if (zl->score != score)
    {
        return zl->score < score;
    }
    int rv = memcmp(zl->name, name, min(zl->len, len));
    if (rv != 0)
    {
        return rv < 0;
    }
    return zl->len < len;
}

static bool zless(SKLNode *lhs, SKLNode *rhs)
{
    ZNode *zr = container_of(rhs, ZNode, sklnode);
    return zless(lhs, zr->score, zr->name, zr->len);
}

// find the last Znode which less than node on each level
static void skiplist_find(ZSet* zset, ZNode* node, SKLNode** update)
{
    SKLNode* cur = zset->head;
    // search from up to low level
    for (int i = zset->max_level - 1; i >= 0; i--)
    {
        while (cur->next[i] != NULL && zless(cur->next[i], &node->sklnode))
        {
            cur = cur->next[i];
        }
        update[i] = cur;
    }
}

// insert into the skiplist
static void skiplist_add(ZSet *zset, ZNode *node)
{
    SKLNode* update[MAX_LEVEL];
    // search from up to low level
    skiplist_find(zset, node, update);
    int lv = node->sklnode.level;
    if (lv > zset->max_level)
    {
        for (int i = zset->max_level; i < lv; i++)
        {
            update[i] = zset->head;
        }
        zset->max_level = lv;
    }
    for (int i = 0; i < lv; i++)
    {
        node->sklnode.next[i] = update[i]->next[i];
        update[i]->next[i] = &node->sklnode;
    }
}

static void skiplist_del(ZSet* zset, ZNode* node)
{
    SKLNode* update[MAX_LEVEL];
    // search from up to low level
    skiplist_find(zset, node, update);
    for (int i = 0; i < node->sklnode.level; i++)
    {
        update[i]->next[i] = node->sklnode.next[i];
    }
}

// update the score of an existing node 
static void zset_update(ZSet *zset, ZNode *node, double score)
{
    if (node->score == score)
    {
        return;
    }
    skiplist_del(zset, node);
    node->score = score;
    int lv = random_level();
    skl_init(&node->sklnode, lv);
    skiplist_add(zset, node);
}

// add a new (score, name) tuple, or update the score of the existing tuple
bool zset_add(ZSet *zset, const char *name, size_t len, double score)
{
    ZNode *node = zset_lookup(zset, name, len);
    if (node)
    {
        zset_update(zset, node, score);
        return false;
    }
    else
    {
        node = znode_new(name, len, score);
        hm_insert(&zset->hmap, &node->hmap);
        skiplist_add(zset, node);
        return true;
    }
}

// a helper structure for the hashtable lookup
struct HKey
{
    HNode node;
    const char *name = NULL;
    size_t len = 0;
};

static bool hcmp(HNode *node, HNode *key)
{
    ZNode *znode = container_of(node, ZNode, hmap);
    HKey *hkey = container_of(key, HKey, node);
    if (znode->len != hkey->len)
    {
        return false;
    }
    return 0 == memcmp(znode->name, hkey->name, znode->len);
}

// lookup by name
ZNode *zset_lookup(ZSet *zset, const char *name, size_t len)
{
    if (!zset->head)
    {
        return NULL;
    }

    HKey key;
    key.node.hcode = str_hash((uint8_t *)name, len);
    key.name = name;
    key.len = len;
    HNode *found = hm_lookup(&zset->hmap, &key.node, &hcmp);
    return found ? container_of(found, ZNode, hmap) : NULL;
}

// deletion by name
ZNode *zset_pop(ZSet *zset, const char *name, size_t len)
{
    if (!zset->head)
    {
        return NULL;
    }

    HKey key;
    key.node.hcode = str_hash((uint8_t *)name, len);
    key.name = name;
    key.len = len;
    HNode *found = hm_pop(&zset->hmap, &key.node, &hcmp);
    if (!found)
    {
        return NULL;
    }

    ZNode *node = container_of(found, ZNode, hmap);
    skiplist_del(zset, node);
    return node;
}

// find the (score, name) tuple that is greater or equal to the argument.
ZNode *zset_query(ZSet *zset, double score, const char *name, size_t len)
{
    SKLNode* update[MAX_LEVEL];
    ZNode* query = znode_new(name, len, score);
    skiplist_find(zset, query, update);
    SKLNode* found = update[0]->next[0];
    return found ? container_of(found, ZNode, sklnode) : NULL;
}

// offset into the succeeding or preceding node.
ZNode *znode_offset(ZNode *node, int64_t offset)
{
    SKLNode* cur = &node->sklnode;
    for (int64_t i = 0; i < offset; i++)
    {
        if (cur == NULL)
        {
            break;
        }
        cur = cur->next[0];
    }
    return cur ? container_of(cur, ZNode, sklnode) : NULL;
}

void znode_del(ZNode *node)
{
    skl_destroy(&node->sklnode);
    free(node);
}

static void skiplist_dispose(SKLNode *head)
{
    if (!head)
    {
        return;
    }
    skiplist_dispose(head->next[0]);
    znode_del(container_of(head, ZNode, sklnode));
}

// destroy the zset
void zset_dispose(ZSet *zset)
{
    skiplist_dispose(zset->head->next[0]);
    hm_destroy(&zset->hmap);
}

ZSet* zset_init()
{
    SKLNode* head = (SKLNode*)malloc(sizeof(SKLNode));
    skl_init(head, MAX_LEVEL / 2);
    ZSet* zset = new ZSet();
    zset->head = head;
    return zset;
}

void zset_destroy(ZSet* zset)
{
    zset_dispose(zset);
    free(zset->head);
    delete zset;
}
