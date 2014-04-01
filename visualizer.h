//
//  visualizer.h
//  Tree exercises
//
//  Created by Eric on 3/21/14.
//  Copyright (c) 2014 erflink. All rights reserved.
//

#ifndef __Tree_exercises__visualizer__
#define __Tree_exercises__visualizer__

#include "StringNode.h"
#include "graphviz/cgraph.h"
#include "graphviz/gvc.h"

#define OUTPUTFILENAME "TreeGraph.png"
class Visualize
{
public:
    Visualize(StringNode const *graphRoot)
    {
        theRoot = graphRoot;
    }
    
    void makeVisualization();
    
private:
    StringNode const *theRoot;
    void buildGraph(Agraph_t *theGraph, TreeNode const *theNode);
    void addDummies(Agraph_t *theGraph, Agnode_t *theGNode, const StringNode *theNode, TreeNode::NodeDirection);
};
#endif /* defined(__Tree_exercises__visualizer__) */
