/*************************************************************************
	> File Name: rbtree.c
	> Author: yangfei
	> Mail: yangfei321_good@163.com 
 ************************************************************************/
//  红黑树的性质
//  1. 一个节点要么是红要么是黑
//  2. 根节点要是黑的
//  3. 每个叶节点（nil节点）是黑的
//  4. 如果一个节点是红的，那么他的两个儿子都是黑的
//  5. 对于每个节点，从该节点到他子孙的所有节点路径包含相同的黑节点个数
#include<stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#define BLACKCOLOR 1
#define REDCOLOR 0
typedef unsigned char u_char;
typedef unsigned int u_int;
typedef struct rbtree_node_s  rbtree_node_t; 
struct rbtree_node_s {
	rbtree_node_t *parent;
	rbtree_node_t *left;
	rbtree_node_t *right;
	// 这里key就是data进行测试
	u_int key;
	u_char color;
};
typedef struct rbtree_s  rbtree_t; 
struct rbtree_s {
	rbtree_node_t *root;
	//哨兵节点
	rbtree_node_t *sentinel;
};
#define rbtree_init(tree, s)                                          \
	rbtree_sentinel_init(s);                                          \
    (tree)->root = s;                                                 \
    (tree)->sentinel = s;                                             \


#define rbt_red(node)               ((node)->color = 1)
#define rbt_black(node)             ((node)->color = 0)
#define rbt_is_red(node)            ((node)->color)
#define rbt_is_black(node)          (!ngx_rbt_is_red(node))
#define rbt_copy_color(n1, n2)      (n1->color = n2->color)

#define rbtree_sentinel_init(node)  rbt_black(node)
//最小节点
rbtree_node_t *rbtree_min(rbtree_node_t *node, rbtree_node_t *sentinel);
rbtree_node_t *rbtree_successor(rbtree_node_t *node, rbtree_node_t *sentinel);
rbtree_node_t *init_rbtree_node(u_int key, u_char color, rbtree_node_t *sentinel);
rbtree_t *init_rbtree();
void rbtree_addnode(rbtree_t *tree, rbtree_node_t *node);
void rbtree_add_fixup(rbtree_node_t *node, rbtree_t *t);
void rbtree_delnode(rbtree_t *tree, rbtree_node_t *node);
void  rbtree_del_fixup(rbtree_t *tree, rbtree_node_t *node);
rbtree_node_t *rbtree_search(rbtree_t *tree, int key);
void rbtree_left_rotate(rbtree_node_t *x, rbtree_t *t);
void rbtree_right_rotate(rbtree_node_t *y, rbtree_t *t);

rbtree_node_t *rbtree_search(rbtree_t *tree, int key) {
	rbtree_node_t *p = tree->root;
	rbtree_node_t *sentinel = tree->sentinel;
	while(p != sentinel) {
		if (p->key == key) {
			break;
		} else if (key > p->key) {
			p = p->right;
		} else {
			p = p->left;
		}
	}
	return p;
}
void rbtree_delnode(rbtree_t *tree, rbtree_node_t *node) {
	rbtree_node_t *z = node;
	rbtree_node_t *y;
	rbtree_node_t *x;
	rbtree_node_t *sentinel = tree->sentinel;
	if (z->left == sentinel || z->right == sentinel) {
		y = z;
	} else {
		y = rbtree_successor(z, sentinel);
	}
	if (y->left != sentinel) {
		x = y->left;
	} else {
		x = y->right;
	}
	x->parent = y->parent;
	if (y->parent == sentinel) {
		tree->root = x;
	} else {
		if (y == y->parent->left) {
			y->parent->left = x;
		} else {
			y->parent->right = x;
		}
	}

	if (y->key != z->key) {
		z->key = y->key;
	}
	if (y->color == BLACKCOLOR) {
		rbtree_del_fixup(tree, x);
	}
}
void  rbtree_del_fixup(rbtree_t *tree, rbtree_node_t *x) {
	while (x != tree->root && x->color == BLACKCOLOR) {
		if (x == x->parent->left) {
			rbtree_node_t *w = x->parent->right;
			if (w->color == REDCOLOR) {
				w->color = BLACKCOLOR;
				x->parent->color = REDCOLOR;
				rbtree_left_rotate(x->parent, tree);
				w = x->parent->right;
			}
			if (w->left->color == BLACKCOLOR && w->right->color == BLACKCOLOR) {
				w->color = REDCOLOR;
				x = x->parent;
			} else {
				if (w->right->color == BLACKCOLOR) {
					w->left->color = BLACKCOLOR;
					w->color = REDCOLOR;
					rbtree_right_rotate(w, tree);
					w = x->parent->right;
				}

				w->color = x->parent->color;
				x->parent->color = BLACKCOLOR;
				w->right->color = BLACKCOLOR;
				rbtree_left_rotate(x->parent, tree);
				x = tree->root;
			}
		} else {
			rbtree_node_t *w = x->parent->left;
			if (w->color == REDCOLOR) {
				w->color = BLACKCOLOR;
				x->parent->color = REDCOLOR;
				rbtree_right_rotate(x->parent, tree);
				w = x->parent->left;
			}
			if (w->left->color == BLACKCOLOR && w->right->color == BLACKCOLOR) {
				w->color = REDCOLOR;
				x = x->parent;
			} else {
				if (w->left->color == BLACKCOLOR) {
					w->left->color = BLACKCOLOR;
					w->color = REDCOLOR;
					rbtree_left_rotate(w, tree);
					w = x->parent->left;
				}

				w->color = x->parent->color;
				x->parent->color = BLACKCOLOR;
				w->left->color = BLACKCOLOR;
				rbtree_right_rotate(x->parent, tree);
				x = tree->root;
			}
		}
	}
	x->color = BLACKCOLOR;
}
rbtree_node_t *init_rbtree_node(u_int key, u_char color, rbtree_node_t *sentinel) {
	rbtree_node_t *node = (rbtree_node_t*)malloc(sizeof(rbtree_node_t));
	node->key = key;
	node->color = color;
	node->left = sentinel;
	node->right = sentinel;
	node->parent = sentinel;
	return node;
}
rbtree_t *init_rbtree() {
	rbtree_t *tree = (rbtree_t*)malloc(sizeof(rbtree_t));
	rbtree_node_t *sentinel = init_rbtree_node(0, BLACKCOLOR, NULL);
	tree->sentinel = sentinel;
	tree->root = sentinel;
}
rbtree_node_t *rbtree_min(rbtree_node_t *node, rbtree_node_t *sentinel) {
	while (node->left != sentinel) {
		node = node->left;
	}
	return node;
}
rbtree_node_t *rbtree_successor(rbtree_node_t *node, rbtree_node_t *sentinel) {
	if (node->right != sentinel) {
		return rbtree_min(node->right, sentinel);
	}
	rbtree_node_t *p = node->parent;
	while (p != sentinel && node == p->right) {
		node = p;
		p = node->parent;
	}
	return p;
}
void rbtree_addnode(rbtree_t *tree, rbtree_node_t *node) {
	rbtree_node_t *p = tree->root;
	rbtree_node_t *sentinel = tree->sentinel;
	if (p == sentinel) {
		node->color = BLACKCOLOR;
		tree->root = node;
		return ;
	}
	while(1) {
		if (node->key > p->key) {
			if (p->right != sentinel) {
				p = p->right;
			} else {
				p->right = node;
				node->parent = p;
				break;
			}
		} else {
			if (p->left != sentinel) {
				p = p->left;
			} else {
				p->left = node;
				node->parent = p;
				break;
			}
		}
	}
	rbtree_add_fixup(node, tree);
}
void rbtree_add_fixup(rbtree_node_t *node, rbtree_t *t) {
	while (node->parent->color == REDCOLOR) {
		if (node->parent == node->parent->parent->left) {
			rbtree_node_t *y = node->parent->parent->right;
			if (y->color == REDCOLOR) {
				node->parent->color = BLACKCOLOR;
				y->color = BLACKCOLOR;
				node->parent->parent->color = REDCOLOR;
				node = node->parent->parent;
			} else {
				if (node == node->parent->right) {
					node = node->parent;
					rbtree_left_rotate(node, t);

					node->parent->color = BLACKCOLOR;
					node->parent->parent->color = REDCOLOR;
					rbtree_right_rotate(node->parent->parent, t);
				} else {
					node->parent->color = BLACKCOLOR;
					node->parent->parent->color = REDCOLOR;
					rbtree_right_rotate(node->parent->parent, t);
				}
			}
		} else {
			rbtree_node_t *y = node->parent->parent->left;
			if (y->color == REDCOLOR) {
				node->parent->color = BLACKCOLOR;
				y->color = BLACKCOLOR;
				node->parent->parent->color = REDCOLOR;
				node = node->parent->parent;
			} else {
				if (node == node->parent->left) {
					node = node->parent;
					rbtree_right_rotate(node, t);

					node->parent->color = BLACKCOLOR;
					node->parent->parent->color = REDCOLOR;
					rbtree_left_rotate(node->parent->parent, t);
				} else {
					node->parent->color = BLACKCOLOR;
					node->parent->parent->color = REDCOLOR;
					rbtree_left_rotate(node->parent->parent, t);
				}
			}
		}
	}
	t->root->color = BLACKCOLOR;
}
// 对树r在节点x上进行左旋
// 左旋或者右旋就是对三条线的修改，可以见算法导论
// 就是下面这三条线
//          |                      |
//          x     ---left--->      y
//         / \    <---right--     / \
//        𝒶   y                  x   ℬ
void rbtree_left_rotate(rbtree_node_t *x, rbtree_t *t) {
	rbtree_node_t *sentinel = t->sentinel;
	rbtree_node_t *y = x->right;
	
	x->right = y->left;
	y->left->parent = x;

	y->parent = x->parent;
	if (x->parent != sentinel) {
		if (x == x->parent->left) {
			x->parent->left = y;
		} else {
			x->parent->right = y;
		}
	} else {
		t->root = y;
	}

	x->parent = y;
	y->left = x;
}
void rbtree_right_rotate(rbtree_node_t *y, rbtree_t *t) {
	rbtree_node_t *sentinel = t->sentinel;
	rbtree_node_t *x = y->left;
	
	y->left = x->right;
	x->right->parent = y;

	x->parent = y->parent;
	if (y->parent != sentinel) {
		if (y == y->parent->left) {
			y->parent->left = x;
		} else {
			y->parent->right = x;
		}
	} else {
		t->root = x;
	}

	y->parent = x;
	x->right = y;
}
void print_node(rbtree_node_t *p, rbtree_node_t *sentinel) {
	if (p->color == REDCOLOR) {
		printf("%d-red-",p->key);
	} else {
		printf("%d-black-",p->key);
	}
	if (p->parent != sentinel) {
		printf("p[%d]-",p->parent->key);
	}
	if (p->left != sentinel) {
		printf("l[%d]-",p->left->key);
	}
	if (p->right != sentinel) {
		printf("r[%d]-",p->right->key);
	}
	printf("\n");
}
void print_rbree(rbtree_node_t *p, rbtree_node_t *sentinel) {
	if (p == sentinel) {
		return;
	}
	print_node(p, sentinel);
	print_rbree(p->left, sentinel);
	print_rbree(p->right, sentinel);
}
int leftblackdepth(rbtree_node_t *p, rbtree_node_t *sentinel) {
	if (p == sentinel) {
		return 1;
	}
	if (p->color == BLACKCOLOR) {
		return leftblackdepth(p->left, sentinel) + 1;
	} else {
		return leftblackdepth(p->left, sentinel);
	}
}
int rightblackdepth(rbtree_node_t *p, rbtree_node_t *sentinel) {
	if (p == sentinel) {
		return 1;
	}
	if (p->color == BLACKCOLOR) {
		return rightblackdepth(p->right, sentinel) + 1;
	} else {
		return rightblackdepth(p->right, sentinel);
	}
}
int is_rbtree(rbtree_node_t *p, rbtree_node_t *sentinel) {
	int leftdepth = leftblackdepth(p->left, sentinel);
	int rightdepth = rightblackdepth(p->right, sentinel);
	printf("%d-%d\n",leftdepth,rightdepth);
	if (leftdepth != rightdepth) {
		return 0;
	}
	return 1;
}
int main() {
	rbtree_t *tree = init_rbtree();
	int i = 0,j=0;
	for(i=0;i<10000000;i++) {
		rbtree_node_t *node = init_rbtree_node(i+1, REDCOLOR, tree->sentinel);
		rbtree_addnode(tree, node);
	}
	struct timeval tv1,tv2,tv3;
	gettimeofday(&tv1,NULL);
	for(i=100;i<10000;i++) {
		rbtree_search(tree, i);
	}
	gettimeofday(&tv2,NULL);
	for(i=100;i<10000;i++) {
		for(j=0;j<10000000;j++) {
			if (i == j) {
				break;
			}
		}
	}
	gettimeofday(&tv3,NULL);
	printf("rbtree查找时间:%d\n",tv2.tv_usec - tv1.tv_usec);
	printf("模拟o(n)数据结构如链表的查找时间:%d\n",tv3.tv_usec - tv2.tv_usec);
}
