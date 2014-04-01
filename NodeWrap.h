//
//  NodeWrap.h
//  Tree exercises
//
//  Created by Eric on 3/17/14.
//  Copyright (c) 2014 erflink. All rights reserved.
//

#ifndef __Tree_exercises__NodeWrap__
#define __Tree_exercises__NodeWrap__
#include "TreeNode.h"

/// NodeType needs to inherit from TreeNode AND the type being wrapped
/// Why?
/// (1) We need access to NodeDirection, LEFT, RIGHT,etc, which are part of TreeNode \todo can we break that out?
/// (2) We need to get to NodeType to call its constructor
/// (3) The template doesn't know that NodeType inherits from TreeNode
///
/// NodeWrap, inheriting from TreeNode, is the same as TreeNode, only adding additional functions
/// NodeType had also better be derived from TreeNode.
template <typename NodeType>
class NodeWrap : public NodeType
{
    
public:
    NodeWrap()
    {
        
    }
    
    NodeWrap(NodeType *nT) : NodeType(nT)
    {
        realTreeNode = nT;
    }
    
    NodeWrap(const NodeType *nT) : NodeType(nT)
    {
        realTreeNode = nT;
    }
    
    NodeWrap(const NodeType &nT) : NodeType(nT)
    {
        realTreeNode = nT;
    }
    
    virtual ~NodeWrap()
    {
        
    }
    
    /// Overloading operator[] and operator()
    /// operator[] allows you to say node[LEFT] to refer to node->leftNode
    /// operator() gives you a pointer to the pointer to the node, e.g. node[LEFT] &(node->leftNode)
    /// This will reduce some syntactic clutter when referencing a node a couple of links away
    NodeType *operator[](const TreeNode::NodeDirection dir)
    {
        switch (dir)
        {
            case RIGHT:
                return dynamic_cast<NodeType *>(realTreeNode->rightNode);
                break;
                
            case LEFT:
                return dynamic_cast<NodeType *>(realTreeNode->leftNode);
                break;
                
            case PARENT:
                return dynamic_cast<NodeType *>(realTreeNode->parentNode);
                break;
                
            case NONE:
                return nullptr;
                
            default:
                __builtin_trap();
                assert(false);  // the language shouldn't even allow this, but always err on the side of safety
                return nullptr;
        }

    }
    
    
    const NodeType *operator[](const TreeNode::NodeDirection dir) const
    {
        switch (dir)
        {
            case RIGHT:
                return dynamic_cast<NodeType *>(realTreeNode->rightNode);
                break;
                
            case LEFT:
                return dynamic_cast<NodeType *>(realTreeNode->leftNode);
                break;
                
            case PARENT:
                return dynamic_cast<NodeType *>(realTreeNode->parentNode);
                break;
                
            case NONE:
                return nullptr;
                
            default:
                __builtin_trap();
                assert(false);  // the language shouldn't even allow this, but always err on the side of safety
                return nullptr;
        }
        
    }
    
    NodeType **operator()(const TreeNode::NodeDirection dir)
    {
        switch (dir)
        {
            case RIGHT:
                return (NodeType **)(&(realTreeNode->rightNode));
                break;
                
            case LEFT:
                return (NodeType **)(&(realTreeNode->leftNode));
                break;
                
            case PARENT:
                return (NodeType **)(&(realTreeNode->parentNode));
                break;
                
            case NONE:
                return nullptr;
                
            default:
                __builtin_trap();
                assert(false);
                return nullptr;
        }

     }
    
    const NodeType ** const *operator()(const TreeNode::NodeDirection dir) const
    {
        switch (dir)
        {
            case RIGHT:
                return (NodeType **)(&(realTreeNode->rightNode));
                break;
                
            case LEFT:
                return (NodeType **)(&(realTreeNode->leftNode));
                break;
                
            case PARENT:
                return (NodeType **)(&(realTreeNode->parentNode));
                break;
                
            case NONE:
                return nullptr;
                
            default:
                __builtin_trap();
                assert(false);
                return nullptr;
        }
    }
    
    NodeType *getRealNode()
    {
        return realTreeNode;
    }
    
private:
    NodeType *realTreeNode;
};


#endif /* defined(__Tree_exercises__NodeWrap__) */
