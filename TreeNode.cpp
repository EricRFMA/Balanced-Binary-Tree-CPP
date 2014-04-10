//
//  TreeNode.cpp
//  Tree exercises
//
//  Created by Eric on 3/15/14.
//  Copyright (c) 2014 erflink. All rights reserved.
//
#include <assert.h>
#include "TreeNode.h"

/// Here's the logical "NOT" for parity !LEFT = RIGHT and !RIGHT = LEFT
/// Still, two wrongs don't make a RIGHT
/// Not the "NOT" for "noChild" is "noChild"
TreeNode::NodeDirection operator!(const TreeNode::NodeDirection dir)
{
    if (dir == LEFT) return RIGHT;
    else if (dir == RIGHT) return LEFT;
    else if (dir == NONE) return NONE;
    else assert(false);
}

/// Get string representation of a node direction, mostly for debugging
const char *directionString(const TreeNode::NodeDirection dir)
{
    switch(dir)
    {
        case LEFT:
            return "left";
            
        case RIGHT:
            return "right";
            
        case NONE:
            return "none";
            
        case PARENT:
            return "parent";
    }
}