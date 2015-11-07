#ifndef RBTREE_H
#define RBTREE_H

////////////////////////////////////////////////////////////////////////////////
/// Following is copied from include/linux/rbtree.h
////////////////////////////////////////////////////////////////////////////////

#include <stdhdr.h>

struct rb_node {
    unsigned long  __rb_parent_color;
    rb_node_t *rb_right;
    rb_node_t *rb_left;
} __attribute__((aligned(sizeof(long))));   // The alignment might seem pointless, but allegedly CRIS needs it
typedef rb_node_t rb_node_t;

struct rb_root {
    rb_node_t *rb_node;
};
typedef struct rb_root rb_root_t;

#define rb_parent(r)   ((rb_node_t *)((r)->__rb_parent_color & ~3))

#define RB_ROOT (struct rb_root) { NULL, }
#define rb_entry(ptr, type, member) container_of(ptr, type, member)

#define RB_EMPTY_ROOT(root)  ((root)->rb_node == NULL)

/* 'empty' nodes are nodes that are known not to be inserted in an rbtree */
#define RB_EMPTY_NODE(node) ((node)->__rb_parent_color == (unsigned long)(node))
#define RB_CLEAR_NODE(node) ((node)->__rb_parent_color = (unsigned long)(node))

extern void rb_insert_color(rb_node_t *, struct rb_root *);
extern void rb_erase(rb_node_t *, struct rb_root *);


/* Find logical next and previous nodes in a tree */
extern rb_node_t *rb_next(const rb_node_t *);
extern rb_node_t *rb_prev(const rb_node_t *);
extern rb_node_t *rb_first(const struct rb_root *);
extern rb_node_t *rb_last(const struct rb_root *);

/* Postorder iteration - always visit the parent after its children */
extern rb_node_t *rb_first_postorder(const struct rb_root *);
extern rb_node_t *rb_next_postorder(const rb_node_t *);

/* Fast replacement of a single node without remove/rebalance/add/rebalance */
extern void rb_replace_node(rb_node_t *victim, rb_node_t *newnode,
                struct rb_root *root);

static inline void rb_link_node(rb_node_t * node, rb_node_t * parent,
                rb_node_t ** rb_link)
{
    node->__rb_parent_color = (unsigned long)parent;
    node->rb_left = node->rb_right = nullptr;

    *rb_link = node;
}

#define rb_entry_safe(ptr, type, member) \
    ({ typeof(ptr) ____ptr = (ptr); \
       ____ptr ? rb_entry(____ptr, type, member) : NULL; \
    })

/**
 * rbtree_postorder_for_each_entry_safe - iterate over rb_root in post order of
 * given type safe against removal of rb_node entry
 *
 * @pos:    the 'type *' to use as a loop cursor.
 * @n:      another 'type *' to use as temporary storage
 * @root:   'rb_root *' of the rbtree.
 * @field:  the name of the rb_node field within 'type'.
 */
#define rbtree_postorder_for_each_entry_safe(pos, n, root, field) \
    for (pos = rb_entry_safe(rb_first_postorder(root), typeof(*pos), field); \
         pos && ({ n = rb_entry_safe(rb_next_postorder(&pos->field), \
            typeof(*pos), field); 1; }); \
         pos = n)

#endif // RBTREE_H