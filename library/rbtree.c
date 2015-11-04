// #include "rbtree.h"

/* this is the wheel implementation of red-black tree.
 * a RB tree has 4 properties:
 * 1. every node is either red (R) or black (B)
 * 2. root and leaves are all black
 * 3. the child of every red node is black
 * 4. all simple path from node x to its desendent leaves have same number of
 *    black nodes, a.k.a. the black-height is same.
 * Note: black-height does not include node itself.
 * Note: leaf nodes are null pointers
 */

struct rb_node {
    struct rb_node *left;
    struct rb_node *right;
} __attribute__((packed));
typedef struct rb_node rb_node_t;

/* left rotate the given tree, and return the new root pointer
 *   [X]                               [Y]
 *   / \                               / \
 * [a] [Y]   ---- left rotate --->   [X] [c]
 *     / \                           / \
 *   [b] [c]                       [a] [b]
 * in the above diagram, `X` and `Y` are two non-NULL nodes,
 * `a`, `b` and `c` are 3 subtrees, which can be NULL.
 */
rb_node_t* left_rotate(rb_node_t *x) {
    rb_node_t *y = x->right;
    x->right = y->left;
    y->left = x;
    return y;
}

/* left rotate the given tree, and return the new root pointer
 *   [X]                                [Y]
 *   / \                                / \
 * [a] [Y]   <--- right rotate ----   [X] [c]
 *     / \                            / \
 *   [b] [c]                        [a] [b]
 * in the above diagram, `X` and `Y` are two non-NULL nodes,
 * `a`, `b` and `c` are 3 subtrees, which can be NULL.
 */
rb_node_t* right_rotate(rb_node_t *y) {
    rb_node_t *x = y->left;
    y->left = x->right;
    x->right = y;
    return x;
}