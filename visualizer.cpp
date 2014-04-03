//
//  visualizer.cpp
//  Tree exercises
//
//  Created by Eric on 3/21/14.
//  Copyright (c) 2014 erflink. All rights reserved.
//

#include <iostream>

#include "visualizer.h"
#include "debugprintf.h"

/// Make the magic happen!

void Visualize::makeVisualization()
{
    Agraph_t *theGraph = agopen((char *)"Tree Graph", Agdirected, NULL);
    
    /// Set up attributes we're going to set in the dummy nodes and edges
    /// we need to create to keep the graph displayed in a nice binary way
    agattr(theGraph, AGNODE, (char *)"penwidth", (char *)"1");
    agattr(theGraph, AGEDGE, (char *)"penwidth", (char *)"1");
    agattr(theGraph, AGEDGE, (char *)"dir", (char *)"forward");
    agattr(theGraph, AGEDGE, (char *)"label", (char *)"");
    agattr(theGraph, AGEDGE, (char *)"weight", (char *)"1");
    agattr(theGraph, AGEDGE, (char *)"textcolor", (char *)"black");
    agattr(theGraph, AGNODE, (char *)"label", (char *)"\\N");
    agattr(theGraph, AGNODE, (char *)"color", (char *)"black");
    agattr(theGraph, AGNODE, (char *)"fontcolor", (char *)"white");
    agattr(theGraph, AGNODE, (char *)"style", (char *)"filled");
    buildGraph(theGraph, theRoot);
    
    GVC_t *gvc = gvContext();
    gvLayout(gvc, theGraph, "dot");
    gvRenderFilename(gvc, theGraph, "png", OUTPUTFILENAME);
    
    cout << "Rendering complete to " << OUTPUTFILENAME << endl;
}

/// Builds the cGraph from our graph.  We pass in TreeNodes even though
/// they're really StringNodes
void Visualize::buildGraph(Agraph_t *theGraph, TreeNode const *theNode)
{
    if (theNode == nullptr)
        return;
    
    // Add this node to the graph
    StringNode const *strNode = dynamic_cast<StringNode const *>(theNode);
    
    debugPrintf3("buildGraph for node %p(%s)... parent %p\n", (void *)theNode, strNode->getCValue(), strNode->parentNode);
    
    Agnode_t *aNode = agnode(theGraph, (char *)strNode->getCValue(), true);

    // Set node color depending on red/black value of node
    if (theNode->isRed())
    {
        agset(aNode, (char *)"color", (char *)"red");
    }
    else
    {
        agset(aNode, (char *)"color", (char *)"black");
        agset(aNode, (char *)"fontcolor", (char *)"white");
    }

    // Now add an edge to its parent node
    if (theNode->parentNode != nullptr)
    {
        StringNode *parNode = dynamic_cast<StringNode *>(theNode->parentNode);
        
        Agnode_t *parentNode = agnode(theGraph, (char *)(parNode->getCValue()), true);
        
        const char *edgeName;
        
        switch (theNode->getParentDir())
        {
            case LEFT:
                edgeName = "left";
                break;
            case RIGHT:
                edgeName = "right";
                break;
            case NONE:
                edgeName = "root";
                break;
            default:
                assert(false);
        }
        
        debugPrintf4("Adding edge from %p(%s) to %p(%s)\n", theNode->parentNode, parNode->getCValue(), theNode, strNode->getCValue() );
        
        Agedge_t *theEdge = agedge(theGraph,  parentNode, aNode, (char *)edgeName, true);
        agset(theEdge, (char *)"label", (char *)edgeName);
        
    }
    
    /// Here, we need to check if left or right is null.  If either one (but not both!) are,
    /// then create an invisible node with an invisible edge so that hopefully things will look more tree-like
    /// when the nodes are layed-out
#define NODENAMELENGTH  100
    if (theNode->leftNode)
    {
        buildGraph(theGraph, theNode->leftNode);
    }
    else  // create a dummy node and edge so the graph will look like a binary tree
    {
        addDummies(theGraph, aNode, strNode, LEFT);
    }
    
    if (theNode->rightNode)
    {
        buildGraph(theGraph, theNode->rightNode);
    }
    else // create a dummy node and edge so the graph will look like a binary tree
    {
        addDummies(theGraph, aNode, strNode, RIGHT);
    }
    
}

/// Add dummy node and edge to make layout more binary tree-ish.  Make them
/// invisible, by making the penwidth zero for the node, edge and edge arrow.
void Visualize::addDummies(Agraph_t *theGraph, Agnode_t *theGNode,
                           const StringNode *theNode, TreeNode::NodeDirection dir)
{
    char dummyNodeName[NODENAMELENGTH];
    const char *dirName;
    if (dir == LEFT)
        dirName = "DummyLeft";
    else if (dir == RIGHT)
        dirName = "DummyRight";
    else assert(false);
    
    sprintf(dummyNodeName, "%s-%s", theNode->getCValue(), dirName);

    Agnode_t *dummyNode = agnode(theGraph, dummyNodeName, true);
    Agedge_t *dummyEdge = agedge(theGraph, theGNode, dummyNode, dummyNodeName, true);
    int setNodeAttr = agset(dummyNode, (char *)"penwidth", (char *)"0");   // make the node invisible
    setNodeAttr = agset(dummyNode, (char *)"label", (char *)"");
    agset(dummyNode, (char *)"style", (char *)"invis");
    int setEdgeAttr = agset(dummyEdge, (char *)"penwidth", (char *)"0");   // make the edge invisible
    setEdgeAttr = agset(dummyEdge, (char *)"dir", (char *)"none");
    agset(dummyEdge, (char *)"style", (char *)"invis");
    
    cout << "setNodeAttr: " << setNodeAttr << " setEdgeAttr: " << setEdgeAttr << endl;
}