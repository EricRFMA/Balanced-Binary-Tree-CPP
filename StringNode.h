//
//  StringNode.h
//  Tree exercises
//
//  Created by Eric on 2/26/14.
//  Copyright (c) 2014 erflink. All rights reserved.
//

#ifndef __Tree_exercises__StringNode__
#define __Tree_exercises__StringNode__
#include <iostream>
#include <memory>
#include <assert.h>

#include "TreeNode.h"

using namespace std;

class StringNode :  public TreeNode
{
public:
    StringNode()
    {
        nodeValue = new string("");
    }
    
    StringNode(string &val)
    {
        nodeValue = new string(val);
    }
    
    StringNode(const char *value)
    {
        nodeValue = new string(value);
    }
    
    StringNode(StringNode *oldNode) : TreeNode(oldNode)
    {
        nodeValue = new string(oldNode->getCValue());
    }
    
    StringNode(const StringNode &oldNode) : TreeNode(oldNode)
    {
        nodeValue = new string(oldNode.getCValue());
    }
    
    
    virtual ~StringNode()
    {
        
    }

    int compare(const StringNode &rhs) const
    {
        return rhs.nodeValue->compare(*this->nodeValue);
    }
    
    int compare(StringNode *rhs) const
    {
        return rhs->nodeValue->compare(*this->nodeValue);
    }
    
    void setValue(string *value)
    {
        nodeValue = value;
    }
    
    string *getValue()
    {
        return nodeValue;
    }
    
    const char *getCValue() const
    {
        return nodeValue->c_str();
    }
    

private:
        string *nodeValue;
};

#endif /* defined(__Tree_exercises__StringNode__) */
