#ifndef __RCYH_RBTREE_RC_H__
#define __RCYH_RBTREE_RC_H__

#include "rbtree.h"
#include <stdlib.h>

struct container
{
	struct rb_node rb_node;
	unsigned char data[];
};

#define CONT_ENTRY(node) \
    rb_entry(node, struct container, rb_node)

#define ALLOC_CONT(cont_ptr, type) \
    cont_ptr = calloc(1, sizeof(struct container) + sizeof(type))

#define FREE_CONT(cont_ptr) \
    free(cont_ptr)

#define CONT_DATA(cont_ptr, type) \
    ((type *)((cont_ptr)->data))

/*
 * Initializing rbtree root
 */
extern void 
init_rbtree(struct rb_root *root);

/*
 * Search key node from rbtree
 */
extern struct container *
container_search(const struct rb_root *root, const void *key, int (*cmp)(const void *x, const void *y));

/*
 * Insert key node into rbtree
 */
extern int 
container_insert(struct rb_root *root, struct container *cont, int (*cmp)(const void *x, const void *y));

/*
 * Delete the key node from rbtree
 *     delete node from rbtree, return node pointer
 */
extern struct container *
container_delete(struct rb_root *root, const void *key, int (*cmp)(const void *x, const void *y));

/*
 * Replace the key node from rbtree for new container
 *    replace node from rbtree, return old node pointer
 */
extern struct container *
container_replace(struct rb_root *root, const void *key, struct container *con, int (*cmp)(const void *x, const void *y));

extern void
container_traverse(const struct rb_root *root, void (*func)(const struct container *cont));

extern int
rb_cmp(const struct rb_root *root1, const struct rb_root *root2, int (*cmp)(const void *x, const void *y));

extern void container_destroy(struct rb_root *root);

#endif /* __RCYH_RBTREE_RC_H__ */
