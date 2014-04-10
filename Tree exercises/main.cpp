//
//  main.cpp
//  Tree exercises
//
//  Created by Eric on 2/26/14.
//  Copyright (c) 2014 erflink. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <ctime>
#include <assert.h>

#include "TreeNode.h"
#include "BinaryTree.h"
#include "StringNode.h"
#include "common.h"
#include "visualizer.h"

#define MAX_WORD_LENGTH   100
// #define ARRAY_DATA
#define SORTED_LIST

template class BinaryTree<StringNode>;
int main(int argc, const char * argv[])
{
    unsigned int wordcount = 0;
    
    // here's some dummy data
    string words[] = {
        "the", "quick", "brown", "fox", "jumps", "over", "the", "lazy", "dog", "eric", "fred", "george", "harry", "ignacio"
    };

    
    // create the empty binary tree
    BinaryTreeType *myTree = new BinaryTreeType;
    clock_t startTime = clock();
#ifdef ARRAY_DATA
    for (string str : words)
    {
        /// Wrap our nodes in a node wrapper, which allows accessing left and right sub nodes
        /// using [] notation, e.g. wNode[RIGHT]
        StringNode *theStringNode = new StringNode(str);
        myTree->addNode(theStringNode);
        wordcount++;
    }
#else
#ifdef SORTED_LIST
    // Let's use some LIVE data
    std::ifstream instream("/usr/share/dict/web2a", std::ifstream::in);

    
    while (!instream.eof() && wordcount < 50)  // put a cap on it for now
    {
        char inputbuffer[MAX_WORD_LENGTH];
        instream.getline(inputbuffer, MAX_WORD_LENGTH);
        
        StringNode *theStringNode = new StringNode(inputbuffer);
        myTree->addNode(theStringNode);
        wordcount++;
        
        if (wordcount % 500 == 0)
        {
            cout << "Wordcount is " << wordcount << " at word " << inputbuffer << endl;
        }
    }
    
    instream.close();
#endif

#endif
    
    clock_t endTime = clock();
    clock_t elapsedTime = endTime - startTime;
    
    unsigned int minDepth = (unsigned int)UINTMAX_MAX;
    unsigned int maxDepth = 0;
    
    myTree->dumpSortedTree(myTree->getRoot());
    myTree->getRoot()->fixDepths(0);  // make sure all depths are set correctly
    myTree->getMinMaxDepth(minDepth, maxDepth);
    
    printf("\nDone building tree, min depth %d, max depth: %d\n\n", minDepth, maxDepth);
    printf("Time to find max depth: %f\n", (clock() - endTime) / (double)CLOCKS_PER_SEC);
    printf("\nTotal time: %lu ticks, %f seconds, %d nodes, %f seconds/node\n",
           elapsedTime,
           elapsedTime / (double)CLOCKS_PER_SEC,
           wordcount,
           (elapsedTime / (double)CLOCKS_PER_SEC) / wordcount);
    fflush(stdout);

    Visualize *vis = new Visualize(myTree->getRoot());
    vis->makeVisualization();
    
}


