//
//  TreeNode.h
//  Tree exercises
//
//  Created by Eric on 3/15/14.
//  Copyright (c) 2014 erflink. All rights reserved.
//

#ifndef __Tree_exercises__TreeNode__
#define __Tree_exercises__TreeNode__
#include <assert.h>

/// Short hand for leftChild and rightChild enum values
/// which have to be defined here, because they're used within the TreeNode
/// class definition
#define LEFT TreeNode::NodeDirection::leftChild
#define RIGHT TreeNode::NodeDirection::rightChild
#define PARENT TreeNode::NodeDirection::parent
#define NONE TreeNode::NodeDirection::noChild

class TreeNode
{
    
public:
    TreeNode()
    {
        leftNode = nullptr;
        
        rightNode = nullptr;
        nodeIsRed = false;
        parentNode = nullptr;
        depth = 0;
    }
    
    TreeNode(const TreeNode &origNode) :
    leftNode(origNode.leftNode),
    rightNode(origNode.rightNode),
    nodeIsRed(origNode.nodeIsRed),
    parentNode(origNode.parentNode),
    depth(origNode.depth)
    {
        
    }
    
    TreeNode(TreeNode *origNode) :
    leftNode(origNode->leftNode),
    rightNode(origNode->rightNode),
    nodeIsRed(origNode->nodeIsRed),
    parentNode(origNode->parentNode),
    depth(origNode->depth)
    {
        
    }

    virtual ~TreeNode()
    {
        
    }
    
    /// Indicates whether we're dealing with a right or left child node
    /// Defined in the class because of linker errors with duplicate symbol Parity
    /// when defined in outer scope.
    enum  Parity
    {
        leftChild,
        rightChild,
        parent,
        noChild    // the "unset" or "undefined" value
    };
    
    typedef TreeNode::Parity NodeDirection;
    
    // splice a node into the tree
    void spliceNodeRight(TreeNode *targetNode)
    {
        assert(this->rightNode == nullptr);  // don't want to step on an existing pointer
        
        // add it into the tree
        this->rightNode = targetNode;
        this->rightNode->depth = this->depth + 1;
        targetNode->parentNode = this;
        targetNode->setToRed();
    }
    
    void spliceNodeLeft(TreeNode *targetNode)
    {
        assert(this->leftNode == nullptr);  // don't want to step on an existing pointer
        
        this->leftNode = targetNode;
        this->leftNode->depth = this->depth + 1;
        targetNode->parentNode = this;
        targetNode->setToRed();
    }
    
    unsigned int getDepth() const
    {
        return depth;
    }
    
    void setDepth(unsigned int newDepth)
    {
        depth = newDepth;
    }
    
    // correct tree depths after rotation, or
    // for the whole tree if we want.
    void fixDepths(unsigned int thisDepth)
    {
        setDepth(thisDepth);
        if (leftNode != nullptr) leftNode->fixDepths(thisDepth+1);
        if (rightNode != nullptr) rightNode->fixDepths(thisDepth+1);
    }
    
    // Some handy helper functions
    
    /// Check if a node is a leaf
    bool isLeaf()
    {
        return leftNode == nullptr && rightNode == nullptr;
    }
    
    /// Check if what we have is a "red" node
    bool isRed()
    {
        return nodeIsRed;
    }
    
    bool isRed() const
    {
        return nodeIsRed;
    }
    
    /// For completeness, check if we have a "black" node
    bool isBlack()
    {
        return !isRed();
    }
    
    bool isBlack() const
    {
        return  !isRed();
    }
    
    /// Black and red checking routines that take a node argument
    /// and check for a null pointer
    static bool isRed(TreeNode *tn)
    {
        if (tn) return tn->isRed();
        else return false;
    }
    
    static bool isBlack(TreeNode *tn)
    {
        if (tn) return tn->isBlack();
        else return false;
    }
    
    /// Make a node black
    void setToBlack()
    {
        nodeIsRed = false;
    }
    
    /// Make a node red
    void setToRed()
    {
        nodeIsRed = true;
    }
    
    /// Complement a node's color, black to red or red to black
    void complementColor()
    {
        if (isRed()) setToBlack();
        else setToRed();
    }
    
    /// Find which child of the parent node we are
    NodeDirection getParentDir() const 
    {
        if (parentNode == nullptr)
            return NONE;
        
        if (parentNode->leftNode == this) return(LEFT);
        else if (parentNode->rightNode == this) return(RIGHT);
        else return (NONE);
    }
    
    /// This method needs to be overloaded in a derived class to create an ordering
    /// of data values in the nodes
    virtual int compare(TreeNode &node)
    {
        assert(false);  // don't call base class compare
        return (0);
    }
    
    TreeNode *leftNode;
    TreeNode *rightNode;
    bool nodeIsRed;
    TreeNode *parentNode; // need this for rotation and balancing (like tires)
    
private:
    unsigned int depth; /// \todo May not want to try to maintain depth, and just infer it on the fly if we need it
    
    
};

/// Here's the logical "NOT" for parity !LEFT = RIGHT and !RIGHT = LEFT
/// Still, two wrongs don't make a RIGHT
TreeNode::NodeDirection operator!(TreeNode::NodeDirection dir);
const char *directionString(TreeNode::NodeDirection dir);
#endif /* defined(__Tree_exercises__TreeNode__) */
