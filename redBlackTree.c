/*
rules
1. every node is either red or black
2. root is always black
3. no adjacent red nodes (red node cannot have a red parent or red child)
4. every path from root to a null node has been number of black nodes (one sub stree can be twice as long)


1. (no parent) N is root -> color it black
2. parent is black -> do nothing
3. parent is red
  look at uncle
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BLACK true
#define RED false

typedef struct node {
  bool color;
  int datum;
  struct node *parent;
  struct node *left;
  struct node *right;
} Node;

Node tmp = {BLACK, (int) NULL, NULL, NULL, NULL};
Node *LEAF = &tmp;

int print(Node *tree);
int clear(Node **tree);
int insert(Node **tree, int datum);
int insertBST(Node **tree, Node *new);
void rebalance(Node *new);
Node * getUncle(Node  *node);
Node * getParent(Node *node);
Node * getGrandparent(Node *node);
Node * getSibling(Node *node);
void rotateLeft(Node *node);
void rotateRight(Node *node);
void delete(Node **tree, int value);
Node * successor(Node *node);
void delete_case1(Node *node);
void delete_case2(Node *node);
void delete_case3(Node *node);
void delete_case4(Node *node);
void delete_case5(Node *node);
void delete_case6(Node *node);
void delete_one_child(Node *node);
void replacement(Node *node, Node *child);

int main(void) {
  Node * tree = NULL;

  insert(&tree, 41);
  insert(&tree, 38);
  insert(&tree, 31);
  insert(&tree, 12);
  insert(&tree, 19);
  insert(&tree, 8);
  print(tree);

  delete(&tree, 8);
  delete(&tree, 12);
  delete(&tree, 19);
  delete(&tree, 31);
  delete(&tree, 38);
  delete(&tree, 41);
  print(tree);
  return 0;
}

int print(Node *root) {
  static int depth = 0;
	if(root == NULL)
		return 0;

  ++depth;
	print(root->right);
	--depth;

	for(int i = 0; i < depth; ++i){
		printf("  ");
  }

  //print color of node
  if(root->color == RED){
    printf("\x1b[31m%d\x1b[0m\n", root->datum);
    puts("");
  }else {
    if(root == LEAF) {
      puts("NIL\n");
    } else {
      printf("%d\n", root->datum);
      puts("");
    }
  }
  ++depth;
	print(root->left);
	--depth;

  return 0;
}

//clears tree
int clear(Node **tree){
  if((*tree) == LEAF) {
    return 0;
  }

  clear(&((*tree)->left));
  clear(&((*tree)->right));
  free(*tree);
  *tree = NULL;
  return 0;
}

int insert(Node **tree, int datum){
  Node *new = malloc(sizeof(Node));
  new->color = RED;
  new->datum = datum;
  new->parent = NULL;
  new->left = LEAF;
  new->right = LEAF;

  insertBST(tree, new);
  rebalance(new);

  //set tree back to root
  while(new->parent){
      new = new->parent;
  }
  *tree = new;

  /*
  if uncle is red (grandparent is black)
  1. change color of parent and uncle to black
  2. change color of grand parent as red
  3. change grandparents all the way up
  if uncle is black, we do rotations and/or recolor
  1. left left case - rotate first left to center top & switch colors of second left and new right
  2. left right case - switch right up to center and left down to right & do left left
  3. right right case - same opposite side for left left
  4. right left case - same as left right
  */
  return 0;
}

int insertBST(Node **tree, Node *new){
  //base case of establishing root
  if(!*tree){
    new->color = BLACK;
    (*tree) = new;
    return 0;
  }

  if((*tree)->datum > new->datum) {
    if((*tree)->left != LEAF) {
      return 1 + insertBST(&((*tree)->left), new);
    } else {
      new->parent = (*tree);
      (*tree)->left = new;
    }
  } else {
    if((*tree)->right != LEAF) {
      return 1 + insertBST(&((*tree)->right), new);
    } else {
      new->parent = (*tree);
      (*tree)->right = new;
    }
  }

  return 0;
}

void rebalance(Node * node){
  //condition root
  if(node->parent == NULL) {
    node->color = BLACK;
    return;
  }

  //condition 2: parent is black
  if(node->parent->color == BLACK) {
    return;
  }

  //condition 3: uncle is red
  if(getUncle(node)->color == RED){
    Node *g = getGrandparent(node);
    g->color = RED;
    getParent(node)->color = BLACK;
    getUncle(node)->color = BLACK;
    rebalance(g);
    return;
  }

  //condition 4: uncle is black
  //constant number of rotations
  else {
    //if tree goes left and right
    if(node == (getGrandparent(node)->left->right)){
      rotateLeft(node->parent);
      node = node->left; //node we are now interested in
    } else if(getGrandparent(node)->right->left == node){
        rotateRight(node->parent);
        node = node->right; //node we are now interested in
    }
    if(node->parent->left == node){
        rotateRight(getGrandparent(node));
    }
    else {
        rotateLeft(getGrandparent(node));
    }
    node->parent->color = BLACK;
    getSibling(node)->color = RED;
  }
}

Node * getUncle(Node  *node) {
  Node *p = getParent(node);
  if(!p) {
    return NULL;
  }
  return getSibling(getParent(node));
}

Node * getParent(Node *node) {
  return node->parent;
}

Node * getGrandparent(Node *node) {
  return getParent(getParent(node));
}

Node * getSibling(Node *node) {
  Node *p = getParent(node);
  if(!p) {
      return NULL;
  } else if(p->left == node) {
    return p->right;
  }
  return p->left;
}

void rotateLeft(Node *node){
  Node *pivot = (node)->right;
  node->right = pivot->left;
  node->right->parent = node;
  pivot->left= node;

  if(node->parent) {
    pivot->parent = node->parent;
    if(pivot->parent->right == node){
      pivot->parent->right = pivot;
    } else {
      pivot->parent->left = pivot;
    }
  } else {
    pivot->parent = NULL;
  }

  node->parent = pivot;
}

void rotateRight(Node *node){
  Node *pivot = (node)->left;
  node->left = pivot->right;
  node->left->parent = node;
  pivot->right= node;

  if(node->parent) {
    pivot->parent = node->parent;
    if(pivot->parent->left == node){
      pivot->parent->left = pivot;
    } else {
      pivot->parent->right = pivot;
    }
  } else {
    pivot->parent = NULL;
  }

  node->parent = pivot;
}


void delete(Node **tree, int value) {
  Node * tmp = (*tree);
  //if tree is a leaf
  if((tmp) == LEAF) {
      return;
  }

  //iterate through
  if((tmp)->datum > value) {
      return delete(&((*tree)->left), value);
  }

  if((tmp)->datum < value){
      return delete(&((*tree)->right), value);
  }

  /*
  Cases:
  1. one child --> remove the node & replace it with child
  2. no children --> remove node from tree
  3. two children --> use successor or successor node to replace node
  */

  //check if it has two children
  if((tmp->left) != LEAF && (tmp->right) != LEAF){
    Node * leftMost = successor(tmp);
    tmp->datum = leftMost->datum;
    printf("leftMost:%d\n", leftMost->datum);
    delete(&leftMost, leftMost->datum);
    /*if(leftMost->parent->left == leftMost){
      delete(&(leftMost->parent->left), leftMost->datum);
    } else {
      delete(&(leftMost->parent->right), leftMost->datum);
    }*/
  }

  //check if it has only one child
  if((tmp->left == LEAF) || (tmp->right == LEAF)) {

    ///find which child has datum & assign to to left child
    Node *child = (tmp->left) != LEAF ? tmp->left : tmp->right;

    //check if tmp is red
    if(tmp->color == RED) {
      if(tmp->parent->left == tmp) {
        tmp->parent->left = LEAF;
      } else {
        tmp->parent->right = LEAF;
      }
      free(tmp);
      return;
    }

    //check if tmp is black & child is black
    if(tmp->color == BLACK && child->color == BLACK)
      delete_one_child(tmp);
    while (tmp->parent) {
        tmp = tmp->parent;
    }
    (*tree) = tmp;
  }
}

void delete_one_child(Node *node){
  Node *child = (node->left) != LEAF ? node->left : node->right;
  replacement(node, child);
  if(node->color == BLACK) {
    child->color = BLACK;
  } else {
    delete_case1(child);
  }
  free(node);

}

//case 1: root
void delete_case1(Node *node){
  if(node->parent != NULL) {
    delete_case2(node);
  }
}

/*
case 2: sibling red (black greater than 1)
  reverse colors of p & s; rotateLeft or rotateRight
*/
void delete_case2(Node *node){
  Node *sib = getSibling(node);
  if(sib->color == RED) {
    node->parent->color = RED;
    sib->color = BLACK;
    if(node == node->parent->left){
      rotateLeft(node->parent);
    } else {
      rotateRight(node->parent);
    }
  }
  delete_case3(node);
}

/*
case 3: sibling black (p is black & sib's children are black)
  checks if parent, leaves, and sibling is black -> if not go to case 4
*/
void delete_case3(Node *node) {
  Node *s = getSibling(node);
  Node *p = getParent(node);

  puts("in case 3");
  if ((p->color==BLACK) && (s->color == BLACK) && (s->left->color==BLACK) && (s->right->color ==BLACK)) {
    puts("if statement");
    s->color = RED;

    delete_case1(node->parent);
  } else {
    puts("else");
    delete_case4(node);
  }
}

/*
case 4: s & children are black but parent is red
  exchange colors of S & P if parent is red
*/
void delete_case4(Node *node){
  Node *sib = getSibling(node);

  if((node->parent->color == RED) && (sib->color == BLACK) && (sib->left->color == BLACK) && (sib->right->color == BLACK)) {
    sib->color = RED;
    node->parent->color = BLACK;
  } else {
    delete_case5(node);
  }
}

/*
case 5: s is black, left node is red, right leaf is black, and N is left child
*/
void delete_case5(Node *node){
  Node *sib = getSibling(node);
  if(sib->color == BLACK) {
    if((node == node->parent->left) && (sib->right->color == BLACK) && (sib->left->color == RED)) {
      sib->color = RED;
      sib->left->color = BLACK;
      rotateRight(sib);
    } else if((node == node->parent->right) && (sib->left->color == BLACK) && (sib->right->color == RED)) {
      sib->color = RED;
      sib->right->color = BLACK;
      rotateLeft(sib);
    }
  }
  delete_case6(node);
}

/*
case 6: s black, s's right child is red, N is left child of parent
*/
void delete_case6(Node *node){
  Node *sib = getSibling(node);
  sib->color = node->parent->color;
  node->parent->color = BLACK;
  if(node == node->parent->left){
    sib->right->color = BLACK;
    rotateLeft(node->parent);
  } else {
    sib->left->color = BLACK;
    rotateRight(node->parent);
  }
}

void replacement(Node *node, Node *child) {
  Node *curr = node->parent;
  if (curr) {
      if (curr->left == node) {
          curr->left = child;
      } else {
          curr->right = child;
      }
  }
}

//go right & then left all the way
Node * successor(Node *node){
  if (!node->right) {
    return NULL;
  }
  Node * right = node->right;
  while(right->left != NULL){
    right = right->left;
  }
  return right;
}
