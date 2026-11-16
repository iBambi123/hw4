#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key,Value>* n);
    void rotateRight(AVLNode<Key,Value>* n);
    void insertFix(AVLNode<Key,Value>*p, AVLNode<Key,Value>* n);
    void removeFix(AVLNode<Key,Value>*n, int8_t diff);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
     if(!this->root_) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        return;
    }

    AVLNode<Key, Value>* p = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* n = nullptr;
    while(true) {
        if(new_item.first < p->getKey()) {
            if(!p->getLeft()) {
                n = new AVLNode<Key, Value>(new_item.first, new_item.second, p);
                p->setLeft(n);
                break;
            }
            p = p->getLeft();
        }
        else if(p->getKey() < new_item.first) {
            if(!p->getRight()) {
                n = new AVLNode<Key, Value>(new_item.first, new_item.second, p);
                p->setRight(n);
                break;
            }
            p = p->getRight();
        }
        else {
            // Key already exists: overwrite the value
            p->setValue(new_item.second);
            return;
        }
    }


    if(p->getBalance() == -1 || p->getBalance() == 1) {
        p->setBalance(0);
    }
    else {
        if(n == p->getLeft()) {
            p->setBalance(-1);
        }
        else {
            p->setBalance(1);
        }
        insertFix(p, n);
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
        AVLNode<Key, Value>* n = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if(!n) {
        return;
    }

    // Two children: swap with the predecessor so n has at most one child
    if(n->getLeft() && n->getRight()) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(
            BinarySearchTree<Key, Value>::predecessor(n));
        nodeSwap(n, pred);
    }

    AVLNode<Key, Value>* p = n->getParent();

    // diff is how the parent's balance changes once n is gone
    int8_t diff = 0;
    if(p) {
        if(n == p->getLeft()) {
            diff = 1;
        }
        else {
            diff = -1;
        }
    }

    // n has 0 or 1 children; promote the child (may be nullptr)
    AVLNode<Key, Value>* child = n->getLeft();
    if(!child) {
        child = n->getRight();
    }
    if(child) {
        child->setParent(p);
    }
    if(!p) {
        this->root_ = child;
    }
    else if(p->getLeft() == n) {
        p->setLeft(child);
    }
    else {
        p->setRight(child);
    }
    delete n;

    removeFix(p, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* n)
{
    AVLNode<Key,Value>* c = n->getRight();
    AVLNode<Key,Value>* p = n->getParent();

    n->setRight(c->getLeft());
    if(c->getLeft()) {
      c->getLeft()->setParent(n);
    }
    c->setLeft(n);
    n->setParent(c);

    c->setParent(p);
    if(!p) {
      this->root_ = c;
    }else if(p->getLeft() == n) {
      p->setLeft(c);
    }else {
      p->setRight(c);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* n)
{
    AVLNode<Key,Value>* c = n->getLeft();
    AVLNode<Key,Value>* p = n->getParent();

    n->setLeft(c->getRight());
    if(c->getRight()) {
      c->getRight()->setParent(n);
    }
    c->setRight(n);
    n->setParent(c);

    c->setParent(p);
    if(!p) {
      this->root_ = c;
    }else if(p->getLeft() == n) {
      p->setLeft(c);
    }else {
      p->setRight(c);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n)
{
    if(!p || !p->getParent()) {
        return;
    }
    AVLNode<Key, Value>* g = p->getParent();

    if(p == g->getLeft()) {
        // p is the left child of g
        g->updateBalance(-1);
        if(g->getBalance() == 0) {
            return;
        }
        else if(g->getBalance() == -1) {
            insertFix(g, p);
        }
        else {
            // g's balance is -2
            if(n == p->getLeft()) {
                // zig-zig
                rotateRight(g);
                p->setBalance(0);
                g->setBalance(0);
            }
            else {
                // zig-zag
                rotateLeft(p);
                rotateRight(g);
                if(n->getBalance() == -1) {
                    p->setBalance(0);
                    g->setBalance(1);
                }
                else if(n->getBalance() == 0) {
                    p->setBalance(0);
                    g->setBalance(0);
                }
                else {
                    p->setBalance(-1);
                    g->setBalance(0);
                }
                n->setBalance(0);
            }
        }
    }
    else {
        // p is the right child of g (mirror image)
        g->updateBalance(1);
        if(g->getBalance() == 0) {
            return;
        }
        else if(g->getBalance() == 1) {
            insertFix(g, p);
        }
        else {
            // g's balance is +2
            if(n == p->getRight()) {
                // zig-zig
                rotateLeft(g);
                p->setBalance(0);
                g->setBalance(0);
            }
            else {
                // zig-zag
                rotateRight(p);
                rotateLeft(g);
                if(n->getBalance() == 1) {
                    p->setBalance(0);
                    g->setBalance(-1);
                }
                else if(n->getBalance() == 0) {
                    p->setBalance(0);
                    g->setBalance(0);
                }
                else {
                    p->setBalance(1);
                    g->setBalance(0);
                }
                n->setBalance(0);
            }
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* n, int8_t diff)
{
    if(!n) {
        return;
    }

    // Work out the next recursive call's arguments before rotating
    AVLNode<Key, Value>* p = n->getParent();
    int8_t ndiff = 0;
    if(p) {
        if(n == p->getLeft()) {
            ndiff = 1;
        }
        else {
            ndiff = -1;
        }
    }

    if(diff == -1) {
        if(n->getBalance() + diff == -2) {
            // Left side is too tall
            AVLNode<Key, Value>* c = n->getLeft();
            if(c->getBalance() == -1) {
                // zig-zig
                rotateRight(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }
            else if(c->getBalance() == 0) {
                // zig-zig, height unchanged so stop
                rotateRight(n);
                n->setBalance(-1);
                c->setBalance(1);
            }
            else {
                // zig-zag
                AVLNode<Key, Value>* g = c->getRight();
                rotateLeft(c);
                rotateRight(n);
                if(g->getBalance() == 1) {
                    n->setBalance(0);
                    c->setBalance(-1);
                }
                else if(g->getBalance() == 0) {
                    n->setBalance(0);
                    c->setBalance(0);
                }
                else {
                    n->setBalance(1);
                    c->setBalance(0);
                }
                g->setBalance(0);
                removeFix(p, ndiff);
            }
        }
        else if(n->getBalance() + diff == -1) {
            // Height unchanged, stop
            n->setBalance(-1);
        }
        else {
            // Now balanced but shorter, keep going up
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
    else if(diff == 1) {
        if(n->getBalance() + diff == 2) {
            // Right side is too tall (mirror image)
            AVLNode<Key, Value>* c = n->getRight();
            if(c->getBalance() == 1) {
                // zig-zig
                rotateLeft(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }
            else if(c->getBalance() == 0) {
                // zig-zig, height unchanged so stop
                rotateLeft(n);
                n->setBalance(1);
                c->setBalance(-1);
            }
            else {
                // zig-zag
                AVLNode<Key, Value>* g = c->getLeft();
                rotateRight(c);
                rotateLeft(n);
                if(g->getBalance() == -1) {
                    n->setBalance(0);
                    c->setBalance(1);
                }
                else if(g->getBalance() == 0) {
                    n->setBalance(0);
                    c->setBalance(0);
                }
                else {
                    n->setBalance(-1);
                    c->setBalance(0);
                }
                g->setBalance(0);
                removeFix(p, ndiff);
            }
        }
        else if(n->getBalance() + diff == 1) {
            // Height unchanged, stop
            n->setBalance(1);
        }
        else {
            // Now balanced but shorter, keep going up
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
