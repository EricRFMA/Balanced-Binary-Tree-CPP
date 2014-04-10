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
#include "visualizer.h"

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
        treeRoot->setToBlack();
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
    void reBalance(btNodeType *wNode, TreeNode::NodeDirection dir);
    btNodeType * doRotation(btNodeType *node,
                            TreeNode::NodeDirection childDir);
    btNodeType * doDoubleRotation(btNodeType *node,
                            TreeNode::NodeDirection childDir);
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
        btNodeType *leftNodeName = dynamic_cast<btNodeType *>(node->leftNode);
        btNodeType *rightNodeName = dynamic_cast<btNodeType *>(node->rightNode);
        btNodeType *parentNodeName = dynamic_cast<btNodeType *>(node->parentNode);
        
        cout << "Node: " << (void *)((TreeNode *)node) << " value '" << node->getCValue() << "', level:" << node->getDepth()
        << " (" << (node->isRed() ? "red" : "black") << ")" << endl;
        cout << "\tParent Node: " << (void *)((TreeNode *)(node->parentNode)) <<  " [" << (node->parentNode ? parentNodeName->getCValue() : "NULL") << "]" << endl;
        cout << "\t\tLeft Node: " << (void *)((TreeNode *)(node->leftNode)) << " [" << (node->leftNode ? leftNodeName->getCValue() : "NULL") << "]" << endl;
        cout << "\t\tRight Node: " << (void *)((TreeNode *)(node->rightNode)) << " [" << (node->rightNode ? rightNodeName->getCValue() : "NULL") << "]" << endl << endl;
    }
    
    unsigned int verifyTree(const btNodeType *theRoot);
};



// This is the tricky bit, since we want a balanced binary tree
template <typename btNodeType>
void BinaryTree<btNodeType>::addNode(btNodeType *node)
{
    debugPrintf2("Adding node %p, with value '%s'\n", node, node->getCValue());
    
    if (treeRoot == nullptr)
    {
        debugPrintf("\tadding as root\n");
        treeRoot = node;
        treeRoot->setToBlack();
        
        return;
    }
    
    node->setToRed();   // all nodes start out red except the root
    
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
    TreeNode::NodeDirection whichSide;  // which side (left or right) node was added to
    
    if (compResult < 0)
    {
        debugPrintf1("Adding '%s' to left node ", node->getCValue());
        whichSide = LEFT;
    }
    else
    {
        debugPrintf1("Adding '%s' to right node ", node->getCValue());
        whichSide = RIGHT;
    }
    
    theNode = wFoundNode[whichSide];
    
    // Note at this point, foundNode will be the parent (before rebalancing)
    // of the new node
    
    // If we haven't found the node, than the node returned from the search is guaranteed
    // to have a null left or right branch, as approprite
    assert(theNode == nullptr);
    
    // Step 3:  If the branch is free, add the node as a new leaf
    if (whichSide == LEFT)
    {
        foundNode->spliceNodeLeft(node);
        debugPrintf3("%p, '%s' depth:%d\n", foundNode->leftNode, foundNode->getCValue(), foundNode->leftNode->getDepth());
    }
    else
    {
        foundNode->spliceNodeRight(node);
        debugPrintf3("%p, '%s' depth:%d\n", foundNode->rightNode, foundNode->getCValue(), foundNode->rightNode->getDepth());
    }
    
    // Rebalance from the new parent node
    reBalance(foundNode, whichSide);

#ifdef DEBUG_OUTPUT
    dumpPreOrderTree(getRoot());
#endif
    
    assert (verifyTree(getRoot()) != 0);
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
void BinaryTree<btNodeType>::reBalance(btNodeType *node, TreeNode::NodeDirection whichSide)
{
    if (node == nullptr) return;
    bool treeChanged = false;
    
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
        // The root can't be red!
        if (!isRoot(node))
            node->setToRed();
        
        if (wNode[LEFT] != nullptr) wNode[LEFT]->setToBlack();
        if (wNode[RIGHT] != nullptr) wNode[RIGHT]->setToBlack();
        treeChanged =  true;
    }
    else
    {
        // Check for configurations that need rotation
        // First, two reds in a row on the branch where we just added a node
        // \todo Code it the hard way, make better syntax later
        if (wNode[whichSide] != nullptr && wNode[whichSide]->isRed())
        {
            NodeWrap<btNodeType> wSide(wNode[whichSide]);

            if (wSide[whichSide] != nullptr && wSide[whichSide]->isRed())
            {
                newNode = doRotation(node, !whichSide);
                treeChanged = true;
            }
            else if (wSide[!whichSide] != nullptr && wSide[!whichSide]->isRed())
            {
                newNode = doDoubleRotation(node, !whichSide);
                treeChanged = true;
            }
        }
    }

    
    // Now, go up the tree and rebalance some more, unless we're at the top
    if (newNode->parentNode == nullptr)
        return;
    
    // We want to go up the tree and re-balance as we go.  We want to rebalance the
    // side (right or left) of the subtree from whence we came
    TreeNode::NodeDirection fromWhence = newNode->getParentDir();
    
    assert(fromWhence != NONE);
    
    reBalance(dynamic_cast<btNodeType *>(newNode->parentNode), fromWhence);
}

// We know that the node[rotateDir] is red and node[rotateDir][rotateDir] is red
template <typename btNodeType>
btNodeType *BinaryTree<btNodeType>::doRotation(btNodeType *node,
                                               TreeNode::NodeDirection rotateDir)
{

    NodeWrap<btNodeType> wNode(node);
    btNodeType *save = wNode[!rotateDir];
    NodeWrap<btNodeType> wSave(save);
    
    debugPrintf("===================\n");
    debugPrintf2("\nBefore rotation around '%s' to the %s:\n", node->getCValue(), directionString(rotateDir));
#ifdef DEBUG_OUTPUT
    dumpPreOrderTree(node);
#endif
    debugPrintf("===================\n");
    
    // Core of the rotation
    save->setToBlack();
    node->setToRed();
    
    // Get the old node's parent's pointer so we can reset it
    btNodeType **parentPointer = nullptr;
    
    // Read as: Take the node's link opposite to the direction of rotation, and change it
    // to THAT node's link in the direction of the rotation
    // Put another way node->{opposite to rotation}node = node->{opposite to rotation}node->{rotation direction link}node
    *(wNode(!rotateDir)) = wSave[rotateDir];
    
    // Read as: node->{opposite to rotation}node->{rotation direction link} = original node
    *(wSave(rotateDir)) = node;
    
    // Fix the parent node relationships
    
    // Make sure the new parent nodes point back at their new childer
    if (node->parentNode != nullptr)  // check if we're at the root node
    {
        NodeWrap<btNodeType> wParentNode(dynamic_cast<btNodeType *>(node->parentNode));
        parentPointer = wParentNode(node->getParentDir());
        assert(parentPointer != nullptr);
        *parentPointer = save;  
    }
    
    if (wNode[!rotateDir] != nullptr)
    {
        (*(wNode(!rotateDir)))->parentNode = node;
    }
    
    // New top node's parent is the old original node's parent
    save->parentNode = node->parentNode;
    
    // The original node's parent is now the "top" node
    node->parentNode = save;

    // Do we have a new root?
    if (save->parentNode == nullptr)
    {
        makeRoot(save);
    }
    
    debugPrintf("===================\n");
    debugPrintf2("\nAfter rotation around '%s' to the %s:\n", node->getCValue(), directionString(rotateDir));
#ifdef DEBUG_OUTPUT
    dumpPreOrderTree(save);
#endif
    debugPrintf("===================\n");
    
    // Return the new root of this subtree
    return save;
    
}

template <typename btNodeType>
btNodeType *BinaryTree<btNodeType>::doDoubleRotation(btNodeType *node,
                                               TreeNode::NodeDirection rotateDir)
{
    // First, do a single rotation so that red grandchild is on the same side and the red child
    NodeWrap<btNodeType> wNode(node);
    
    debugPrintf2("*** Double rotation around %s, to the %s\n", node->getCValue(), directionString(rotateDir));
    *(wNode(!rotateDir)) = doRotation(wNode[!rotateDir], !rotateDir); 
    return doRotation(node, rotateDir);
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

#define ERROR_ASSERTS
#ifdef ERROR_ASSERTS
#define VERIFY_ERROR(x)  { \
                            Visualize *vis = new Visualize(getRoot()); \
                            vis->makeVisualization(); \
                            assert(x); \
                         }
#else
#define VERIFY_ERROR(x)  return(x)
#endif
// Verify that a tree is a valid red-black binary tree
template <typename btNodeType>
unsigned int BinaryTree<btNodeType>::verifyTree(const btNodeType *theRoot)
{
    
    int leftBlackCount = 0,
                 rightBlackCount = 0;  // black node depths
    
    if (theRoot == nullptr)
        return 1;
    
    const btNodeType *leftNode = dynamic_cast<const btNodeType *>(theRoot->leftNode);
    const btNodeType *rightNode = dynamic_cast<const btNodeType *>(theRoot->rightNode);
    
    // Check for consecutive red links
    if (theRoot->isRed() &&
        (((leftNode != nullptr) && leftNode->isRed()) || 
        (rightNode != nullptr && rightNode->isRed())))
    {
        cerr << "Red violation, node " << (void *)theRoot << endl;
        VERIFY_ERROR(0);
    }
    
    leftBlackCount = verifyTree(leftNode);
    rightBlackCount = verifyTree(rightNode);
    
    // Check for invalid binary search tree
    if ((leftNode != nullptr && leftNode->compare(theRoot) <= 0) ||
        (rightNode != nullptr && rightNode->compare(theRoot) >= 0))
    {
        cerr << "Bad binary tree at node " << (void *)theRoot << endl;
        VERIFY_ERROR(0);
    }
    
    // Check parentage
    if (theRoot->parentNode != nullptr)
    {
        if (theRoot->parentNode->leftNode != theRoot && theRoot->parentNode->rightNode != theRoot)
            VERIFY_ERROR(0);
    }
    
    // Check for black height
    if (leftBlackCount != 0 && rightBlackCount != 0 && leftBlackCount != rightBlackCount )
    {
        cerr << "Black violation at node " << (void *)theRoot << endl;

        VERIFY_ERROR(0);
        
    }
    
    // return current black height at this node
    // ignore red nodes ('cause they're not black!)
    if (rightBlackCount != 0 && leftBlackCount != 0)
    {
        return theRoot->isRed() ? leftBlackCount : leftBlackCount+1;
    }
    else
    {
        VERIFY_ERROR(0);
    }
}


#endif /* defined(__Tree_exercises__BinaryTree__) */
