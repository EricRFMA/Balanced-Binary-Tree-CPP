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
#include <random>
#include <sys/stat.h>

#include "TreeNode.h"
#include "BinaryTree.h"
#include "StringNode.h"
#include "common.h"
#include "visualizer.h"

#define MAX_WORD_LENGTH   100
#define DICTIONARY_FILENAME "/usr/share/dict/web2"
#define WORD_MAX  500 // maximum number of words
// #define ARRAY_DATA 1
// #define SORTED_LIST 1
#define RANDOM_LIST 1

uniform_int_distribution<unsigned> *
createUniformDist(unsigned min, off_t max)
{
    return new uniform_int_distribution<unsigned>(min, max);
}

default_random_engine *
createRandomEngine()
{
    return new default_random_engine((unsigned int)time(0));
}

template class BinaryTree<StringNode>;
int main(int argc, const char * argv[])
{
    unsigned int wordcount = 0;

#if ARRAY_DATA
    // here's some dummy data
    string words[] = {
        "the", "quick", "brown", "fox", "jumps", "over", "the", "lazy", "dog", "eric", "fred", "george", "harry", "ignacio"
    };
#endif
    
    // create the empty binary tree
    BinaryTreeType *myTree = new BinaryTreeType;
    clock_t startTime = clock();
#if defined(ARRAY_DATA)
    for (string str : words)
    {
        /// Wrap our nodes in a node wrapper, which allows accessing left and right sub nodes
        /// using [] notation, e.g. wNode[RIGHT]
        StringNode *theStringNode = new StringNode(str);
        myTree->addNode(theStringNode);
        wordcount++;
    }
#elif defined(SORTED_LIST)
    // Let's use some LIVE data
    std::ifstream instream(DICTIONARY_FILENAME, std::ifstream::in);

    
    while (!instream.eof() && wordcount < WORD_MAX)  // put a cap on it for now
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
#elif defined(RANDOM_LIST)
    
    // Use a random list of words from the dictionary
    // Rather than try to read in all 235,886 words, we seek to a random
    // position in the file
    // get file size in bytes
    struct stat buf;
    if (stat(DICTIONARY_FILENAME, &buf) < 0)
    {
        cerr << "Can't stat dictionary file" << endl;
        exit(1);
    }
    
    ifstream dictStream(DICTIONARY_FILENAME, fstream::in);
    if (dictStream.bad())
    {
        cerr << "Can't open dictionary file" << endl;
        exit (2);
    }
    
    uniform_int_distribution<unsigned> *u = createUniformDist(0, buf.st_size);
    default_random_engine *e = createRandomEngine();
    
    /// Vector will be filled with the words from the dictionary
    vector<string> wordList;
    
    for (int i = 0 ; i < WORD_MAX ; i++)
    {
        bool done = false;
        
        // use this do loop to make sure we do this at least once
        // and can retry on errors
        do
        {
            unsigned seekLoc = (*u)(*e);
            dictStream.seekg(seekLoc);
            
            assert(dictStream.good());
            
            // Tricky part... seek backwards until we get a line break
            // Forward might be easier, but this way we don't preclude the
            // first word in the list.
            while (dictStream.peek() != '\n' && dictStream.good())
            { 
                dictStream.seekg(-1, ios_base::cur);
            }
            dictStream.seekg(1, ios_base::cur);
            
            if (dictStream.bad())  // did we seek back TOO far???
                continue;
            
            char inputbuffer[MAX_WORD_LENGTH];
            dictStream.getline(inputbuffer, MAX_WORD_LENGTH);

            string theString(inputbuffer);
            
            wordList.push_back(theString);
            
            done = true;
            
        } while (!done);
    }
    
    // We should have a nice vector of random words here
    for (string str : wordList)
    {
        cerr << str << endl;
        StringNode *theStringNode = new StringNode(str);
        myTree->addNode(theStringNode);
        wordcount++;
    }


    
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


