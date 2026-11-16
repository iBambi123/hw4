#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int leafDepth(Node* node);
int leafDepth(Node* node) {
  if(!node->left && !node->right) {return 1;}
  if(!node->left){
    int r = leafDepth(node->right);
    if(r == -1) {return -1;}
    return r + 1;
  }
  if(!node->right) {
    int l = leafDepth(node->left);
    if(l == -1) {return - 1;}
    return l + 1;
  }
  int l = leafDepth(node->left);
  int r = leafDepth(node->right);
  if(l == -1 || r == -1 || l != r) {return -1;}
  return l +1;
}

bool equalPaths(Node * root)
{
    // Add your code below
    if(!root) {return true;}
    return leafDepth(root) != -1;
}

