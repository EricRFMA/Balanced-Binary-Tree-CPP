//
//  BinaryTree.h
//  Tree exercises
//
//  Created by Eric on 2/26/14.
//  Copyright (c) 2014 erflink. All rights reserved.
//

#ifndef __Tree_exercises__BinaryTree__
#define __Tree_exercises__BinaryTree__

#include <memory>
#include <assert.h>
#include <iostream>
#include <cstdlib>

#include "TreeNode.h"
#include "NodeWrap.h"
#include "debugprintf.h"

using namespace std;

template <typename btNodeTypeT>
class BinaryTree
{
    typedef btNodeTypeT btNodeType;
    
public:
    BinaryTree()
    {
        treeRoot =  nullptr;
    }
    ~BinaryTree();
    
    void addNode(btNodeType *node);
    
    /// Identify a node as the root node
    bool isRoot(btNodeType *node)
    {
        return (node == treeRoot);
    }
    
    void makeRoot(btNodeType *newRoot)
    {
        assert(newRoot != nullptr);
        treeRoot = newRoot;
    }
    
    btNodeType *getRoot()
    {
        return treeRoot;
    }
    
    void dumpSortedTree(const TreeNode *node);
    void dumpPreOrderTree(const TreeNode *node);
    
    void dumpTreeFromRoot()
    {
        cout << "------------------------------------" << endl;
        cout << "Dumping sorted tree from root" << endl;
        cout << "------------------------------------" << endl;
        cout << "The Root" << endl;
        cout << "Pointer as TreeNode: " << (void *)((TreeNode *)getRoot()) << endl;
        cout << "Pointer as btNodeType: " << (void *)((btNodeType *)getRoot()) << endl;
        dumpNodeInfo(getRoot());
        
        cout << "------------------------------------" << endl;
        
        dumpSortedTree(treeRoot);
        
    }
    
    void getMinMaxDepth(unsigned int &minDepth, unsigned int &maxDepth)
    {
        // Should probably be able to do this as we build the tree
        // but now we can have fun with a DFS!
        drillDownToMaxDepth(treeRoot, minDepth, maxDepth);
    }
    
    
private:
    btNodeType *treeRoot;
    btNodeType *findNode(btNodeType *node, bool &found);
    btNodeType *searchNode(btNodeType * node, btNodeType * root,
                           bool &found);
    void reBalance(btNodeType *wNode);
    btNodeType * doRotation(btNodeType *node,
                            TreeNode::NodeDirection childDir,
                            TreeNode::NodeDirection grandChildDir);
    void drillDownToMaxDepth(btNodeType *node, unsigned int &minDepth, unsigned int &maxDepth)
    {
        if (node == nullptr) return;
        
        // Are we at a leaf?
        if (node->isLeaf())
        {
            if (node->getDepth() > maxDepth)
                maxDepth = node->getDepth();
            if (node->getDepth() < minDepth)
                minDepth = node->getDepth();
        }
        
        drillDownToMaxDepth(dynamic_cast<btNodeType *>(node->leftNode),  minDepth, maxDepth);
        drillDownToMaxDepth(dynamic_cast<btNodeType *>(node->rightNode), minDepth, maxDepth);
        
    }
    
    void dumpNodeInfo(const btNodeType *node)
    {
        cout << "Node: " << (void *)((TreeNode *)node) << " value '" << node->getCValue() << "', level:" << node->getDepth()
        << " (" << (node->isRed() ? "red" : "black") << ")" << endl;
        cout << "\tParent Node: " << (void *)((TreeNode *)(node->parentNode)) << endl;
        cout << "\t\tLeft Node: " << (void *)((TreeNode *)(node->leftNode)) << endl;
        cout << "\t\tRight Node: " << (void *)((TreeNode *)(node->rightNode)) << endl << endl;
    }
};



// This is the tricky bit, since we want a balanced binary tree
template <typename btNodeType>
void BinaryTree<btNodeType>::addNode(btNodeType *node)
{
    debugPrintf2("Adding node %p, with value '%s'\n", node, node->getCValue());
    
    
    node->setToRed();   // all nodes start out red
    
    if (treeRoot == nullptr)
    {
        debugPrintf("\tadding as root\n");
        treeRoot = node;
        treeRoot->setToBlack();
        
        return;
    }
    
    // Step 1: search the tree to see where this node should go.
    //          If found is set to true in findNode, then the node
    //          is already in the tree.  Otherwise, findNode returns the
    //          node that this node should hang off of.
    bool found = false;
    btNodeType *foundNode = findNode(node, found);
    NodeWrap<btNodeType> wFoundNode(foundNode);
    
    if (found)
    {
        debugPrintf2("Value '%s' found at node %p\n", node->getCValue(), foundNode);
        return; // we found it, all done
    }
    
    debugPrintf("Node not found in tree\n");
    
    // Step 2:  see if the branch we need to add to is available
    int compResult = foundNode->compare(node);
    
    // if they're equal, we shouldn't be here
    assert(compResult != 0);
    
    btNodeType * theNode;  // Set theNode to point to either the right or left child
                           // of the foundNode, depending on its value.
                           // Since the foundNode is supposed to be the leaf node
                           // where we're going to add our new node, that child pointer
                           // had better be null!
    
    if (compResult < 0)
    {
        debugPrintf1("Adding '%s' to left node ", node->getCValue());
        theNode = wFoundNode[LEFT];
    }
    else
    {
        debugPrintf1("Adding '%s' to right node ", node->getCValue());
        theNode = wFoundNode[RIGHT];
    }
    
    // Note at this point, foundNode will be the parent (before rebalancing)
    // of the new node
    
    // If we haven't found the node, than the node returned from the search is guaranteed
    // to have a null left or right branch, as approprite
    assert(theNode == nullptr);
    
    // Step 3:  If the branch is free, add the node as a new leaf
    if (compResult < 0)
    {
        foundNode->spliceNodeLeft(node);
        debugPrintf2("%p, depth:%d\n", foundNode->leftNode, foundNode->leftNode->getDepth());
    }
    else
    {
        foundNode->spliceNodeRight(node);
        debugPrintf2("%p, depth:%d\n", foundNode->rightNode, foundNode->rightNode->getDepth());
    }
    
    // Rebalance from the new parent node
    reBalance(foundNode);
    dumpPreOrderTree(getRoot());
}

// Search the tree from the root for a node with the same value as the passed
// in node.  If found, the node is returned and the found parameter is set to true.
// Otherwise, the node is returned that should be the parent of the node, should it be
// added. To do this efficiently, we'll keep track of how deep each node is
template <typename btNodeType>
btNodeType *BinaryTree<btNodeType>::findNode(btNodeType *node, bool &found)
{
    debugPrintf2("Searching for '%s' in node '%p'", node->getCValue(), node);
    debugPrintf2(" starting at root '%s', node '%p'\n", treeRoot->getCValue(), treeRoot);
    return searchNode(node, treeRoot, found);
}

// searches the tree rooted at "root" for a node with the value in node "node"
// If such a node is found, the return value "found" is set to true, and a pointer to the
// located node is returned.
// If such a node is NOT found, then a pointer to the node off of which "node" should hang is returned.
template <typename btNodeType>
btNodeType *BinaryTree<btNodeType>::searchNode(btNodeType *node, btNodeType *root, bool &found)
{
    debugPrintf2("Searching for value '%s' from node %p...\n", node->getCValue(), (void *)root);
    int compResult = root->compare(node);
    
    debugPrintf2("compare result with '%s' is %d...\n", root->getCValue(), compResult);
    
    found = false;
    if (compResult == 0) // we found it!
    {
        found = true;
        debugPrintf("Found!\n");
        return node;
    }
    
    TreeNode::NodeDirection nodeDir = (compResult < 0 ? LEFT : RIGHT);
    NodeWrap<btNodeType> wRoot(root);
    debugPrintf1("\twNode[nodeDir] == %p\n", wRoot[nodeDir]);
    if (wRoot[nodeDir] == nullptr)
    {
        return root;
    }
    
    return searchNode(node, wRoot[nodeDir], found);
}

// In-order tree traversal == sorted tree values
template <typename btNodeType>
void BinaryTree<btNodeType>::dumpSortedTree(const TreeNode *node)
{
    // do the left branch, this node, and the right branch
    // should get sorted list
    if (node == nullptr) return;
    
    dumpSortedTree(node->leftNode);
    
    dumpNodeInfo(dynamic_cast<const btNodeType *>(node));
    
    dumpSortedTree(node->rightNode);
}

// Pre-order tree traversal gives us the root first, a little
// easier to look at for debugging
template <typename btNodeType>
void BinaryTree<btNodeType>::dumpPreOrderTree(const TreeNode *node)
{
    // do the left branch, this node, and the right branch
    // should get sorted list
    if (node == nullptr) return;
    
    dumpNodeInfo(dynamic_cast<const btNodeType *>(node));
    
    dumpPreOrderTree(node->leftNode);
    
    dumpPreOrderTree(node->rightNode);
}

/**
 Here are the allowed node configurations:
 
 2-node:         Black
                /    \
 
 3-node:        Black                       Black
                /    \                      /     \
                Red                                Red
                /   \                               /  \
 
 4-node: (must be split!)
 Black
 /     \
 Red     Red
 /   \   /   \
 
 All other configurations are invalid and need to be repaired
 
 Governing rules are:
 
 * A red node can't have a red node child. red nodes abstract the 2-3-4 tree so their
 children have to be black nodes
 * Since the leaves of a 2-3-4 tree are at the same level, the same has to be true of
 the black nodes in a red-black tree.  Red nodes aren't counted since they're "abstract" in
 the red-black tree sense, and a black node and its red children are one abstract 2-3-4 node.
 So... the number of black nodes on any path of a red-black tree must be the same.
 **/
/// The argument node is the node at which we're rebalancing.  We only look locally at the current node,
/// and its children (and maybe grandchildren in some cases)
template <typename btNodeType>
void BinaryTree<btNodeType>::reBalance(btNodeType *node)
{
    if (node == nullptr) return;
    
    // First, if we're at the root, we're done
    // if (isRoot(node)) return;
    
    btNodeType *newNode = node;  // initialize here, so if nothing changes we'll rebalance up the
                                 // tree starting from here
    NodeWrap<btNodeType> wNode(node);
    
    // Check for red violations
    
    // case 1: Black node with two red children
    //          Turn it into a red node, with two black children
    //          Basically splitting up a four node
    if (wNode.isBlack() && TreeNode::isRed(wNode[LEFT]) && TreeNode::isRed(wNode[RIGHT]))
    {
        node->setToRed();
        
        if (wNode[LEFT] != nullptr) wNode[LEFT]->setToBlack();
        if (wNode[RIGHT] != nullptr) wNode[RIGHT]->setToBlack();
    }
    else if (wNode.isBlack() &&
             (TreeNode::isRed(wNode[LEFT]) || TreeNode::isRed(wNode[RIGHT])))
        // do we have two red nodes in a row amongst the children?
    {
        TreeNode::NodeDirection nDir = NONE;
        
        assert(!TreeNode::isRed(wNode[LEFT]) || !(TreeNode::isRed(wNode[RIGHT])));  // they can't both be red
        
        if (TreeNode::isRed(wNode[LEFT])) nDir = LEFT;
        else if (TreeNode::isRed(wNode[RIGHT])) nDir = RIGHT;
        
        assert(nDir != NONE);  // We know one of the children has to be red
        
        if (nDir != NONE)
        {
            // We have some work to do!
            TreeNode::NodeDirection subNodeDir = NONE;
            
            // Due to the way we maintain the tree, only ONE grandchild of the node can
            // be red along the same path (left->left or right->right)
            NodeWrap<btNodeType> childNode(wNode[nDir]);
            
            NodeWrap<btNodeType> grandChildLeft;
            if (childNode[LEFT] != nullptr) grandChildLeft = (childNode[LEFT]);
            NodeWrap<btNodeType> grandChildRight;
            if (childNode[RIGHT] != nullptr) grandChildRight = (childNode[RIGHT]);
            
            assert(!grandChildLeft.isRed() || !grandChildRight.isRed());  // they can't both be red
            
            if (grandChildRight.isRed()) subNodeDir = RIGHT;
            else if (grandChildLeft.isRed()) subNodeDir = LEFT;
            
            if (subNodeDir != NONE && nDir == subNodeDir)
            {
                // do the rotation/flip around "node" given the link directions
                // we found pointing to the red nodes
                newNode = doRotation(node, nDir, subNodeDir);
                
            }
        }
        
    }
    
    // Now, go up the tree and rebalance some more
    reBalance(dynamic_cast<btNodeType *>(newNode->parentNode));
}

template <typename btNodeType>
btNodeType *BinaryTree<btNodeType>::doRotation(btNodeType *node,
                                               TreeNode::NodeDirection childDir,
                                               TreeNode::NodeDirection grandChildDir)
{
    // First, the original node's color and the affected child colors
    // get switched.  It should be black to red, and red to black
    NodeWrap<btNodeType> wNode(node);
    
    debugPrintf("===================\n");
    debugPrintf("\nBefore rotation:\n");
    dumpPreOrderTree(node);
    debugPrintf("===================\n");
    
    node->complementColor();
    wNode[childDir]->complementColor();
    NodeWrap<btNodeType> wChildDir(wNode[childDir]);
    
    // This is the link we're going to have point to the original node
    // It points to the sibling of the red child node of wNode[childDir]
    btNodeType **emptyLink = wChildDir(!grandChildDir);
    
    // make sure the link we're about to use is free
    assert(*emptyLink == nullptr);
    
    *emptyLink = node;
    
    // Ultimately, we want the parent of "node" to point to the rotated
    // up node instead of "node".  This tracks the link from node's parent to "node"
    btNodeType *saveNode = dynamic_cast<btNodeType *>(node->parentNode);
    
    if (saveNode) // if there IS a parent...
    {
        TreeNode::NodeDirection parentNodeDir;
        parentNodeDir = node->getParentDir();
        NodeWrap<btNodeType> wNodeParent(saveNode);  // When we get here, wNodeParent(parentNodeDir) will point to the link
                                                     // we need to fix later on.
        *(wNodeParent(parentNodeDir)) = wNode[childDir];
    }
    
    // this makes the original node a child of ITS red child
    if (isRoot(node))
    {
        debugPrintf2("Making new root after rotation, '%s' in node %p\n",
                     wNode[childDir]->getCValue(), (void *)(wNode[childDir]));
        makeRoot(wNode[childDir]);  // have to set the new root, if needed
        wNode[childDir]->parentNode = nullptr;
    }
    else
    {
        wNode[childDir]->parentNode = saveNode;  // change new parent of node rotated "up"
    }

    
    node->parentNode = wNode[childDir];  // change new parent of this node
    *(wNode(childDir)) = nullptr;  // the child of the node we rotated "down" no longer has
                                   // a child on the formerly red node side, since that's now the root


    
    debugPrintf("===================\n");
    debugPrintf1("\nAfter rotation to the %s:\n", childDir == LEFT ? "left" : "right");
    dumpPreOrderTree(wNode[PARENT]);
    debugPrintf("===================\n");
    
    // Return the new root of this subtree
    return wNode[PARENT];
    
}

#ifdef DEBUG_OUTPUT
template <typename btNodeType>
void littleDumpNode(btNodeType *node)
{
    debugPrintf3("Node: '%s' (%p) %s\n", node->getCValue(), (void *)node, node->isRed() ? "red" : "black");
    btNodeType *right = dynamic_cast<btNodeType *>(node->rightNode);
    btNodeType *left = dynamic_cast<btNodeType *>(node->leftNode);
    btNodeType *parent = dynamic_cast<btNodeType *>(node->parentNode);
    
    debugPrintf("\tLeft: ");
    if (left == nullptr)
        debugPrintf("NULL\n");
    else
        debugPrintf3("'%s' (%p) %s\n", left->getCValue(), (void *)left, left->isRed() ? "red" : "black");
    
    debugPrintf("\tRight: ");
    if (right == nullptr)
        debugPrintf("NULL\n");
    else
        debugPrintf3("'%s' (%p) %s\n", right->getCValue(), (void *)left, right->isRed() ? "red" : "black");
    
    debugPrintf("\tParent: ");
    if (parent == nullptr)
        debugPrintf("NULL\n");
    else
        debugPrintf3("'%s' (%p) %s\n", parent->getCValue(), (void *)parent, parent->isRed() ? "red" : "black");
    
}
#else
template <typename btNodeType>
void littleDumpNode(btNodeType *node)
{
    
}
#endif
#endif /* defined(__Tree_exercises__BinaryTree__) */
