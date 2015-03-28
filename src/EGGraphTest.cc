/*
 *  EGGraphTest.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGResource.h"
#include "EGGraph.h"
#include "EGGraphTest.h"


/* EGGraphTestImpl */

EGGraphTestAllPairs        EGGraphTestImpl::allPairs16(16);
EGGraphTestAllPairs        EGGraphTestImpl::allPairs64(64);
EGGraphTestAllPairs        EGGraphTestImpl::allPairs256(256);
EGGraphTestRandomPairs     EGGraphTestImpl::randomPairs64p0125(64, 0.125f);
EGGraphTestRandomPairs     EGGraphTestImpl::randomPairs256p0125(256, 0.125f);
EGGraphTestRandomPairs     EGGraphTestImpl::randomPairs1024p0125(1024, 0.125f);
EGGraphTestRandomScaleFree EGGraphTestImpl::randomScaleFree64(64);
EGGraphTestRandomScaleFree EGGraphTestImpl::randomScaleFree256(256);
EGGraphTestRandomScaleFree EGGraphTestImpl::randomScaleFree1024(1024);
EGGraphTestLoopStar        EGGraphTestImpl::loopStar1x16x0(1, 16, 0);
EGGraphTestLoopStar        EGGraphTestImpl::loopStar1x64x0(1, 64, 0);
EGGraphTestLoopStar        EGGraphTestImpl::loopStar1x256x16(1, 256, 16);
EGGraphTestLoopStar        EGGraphTestImpl::loopStar2x2x2(2, 2, 2);
EGGraphTestLoopStar        EGGraphTestImpl::loopStar4x4x4(4, 4, 4);
EGGraphTestLoopStar        EGGraphTestImpl::loopStar8x8x8(8, 8, 8);
EGGraphTestLoopStar        EGGraphTestImpl::loopStar16x16x16(16, 16, 16);
EGGraphTestGrid            EGGraphTestImpl::grid16x16(16, 16);
EGGraphTestGrid            EGGraphTestImpl::grid32x32(32, 32);
EGGraphTestGrid            EGGraphTestImpl::grid64x64(64, 64);
EGGraphTestGrid            EGGraphTestImpl::grid128x128(128, 128);
EGGraphTestResource        EGGraphTestImpl::graphResource(EGResource::getResource("Resources/metadata.bundle/graph.txt"));

EGGraphTest* EGGraphTestImpl::testgraphs[] = {
    &EGGraphTestImpl::allPairs16,
    &EGGraphTestImpl::allPairs64,
    &EGGraphTestImpl::allPairs256,
    &EGGraphTestImpl::randomPairs64p0125,
    &EGGraphTestImpl::randomPairs256p0125,
    &EGGraphTestImpl::randomPairs1024p0125,
    &EGGraphTestImpl::randomScaleFree64,
    &EGGraphTestImpl::randomScaleFree256,
    &EGGraphTestImpl::randomScaleFree1024,
    &EGGraphTestImpl::loopStar1x16x0,
    &EGGraphTestImpl::loopStar1x16x0,
    &EGGraphTestImpl::loopStar1x64x0,
    &EGGraphTestImpl::loopStar1x256x16,
    &EGGraphTestImpl::loopStar2x2x2,
    &EGGraphTestImpl::loopStar4x4x4,
    &EGGraphTestImpl::loopStar8x8x8,
    &EGGraphTestImpl::loopStar16x16x16,
    &EGGraphTestImpl::grid16x16,
    &EGGraphTestImpl::grid32x32,
    &EGGraphTestImpl::grid64x64,
    &EGGraphTestImpl::grid128x128,
    &EGGraphTestImpl::graphResource,
    NULL
};


/* EGGraphTestAllPairs */

EGGraphTestAllPairs::EGGraphTestAllPairs(EGint numNodes) : numNodes(numNodes)
{
    std::stringstream ss;
    ss << "All Pairs " << numNodes;
    name = ss.str();
}

void EGGraphTestAllPairs::populate(EGGraphPtr graph)
{
    // create super nodes
    EGGraphNode **nodes = new EGGraphNode*[numNodes];
    for (EGint i = 0; i < numNodes; i++) {
        std::stringstream ss;
        ss << "node-" << (i+1);
        EGGraphNode *node = nodes[i] = new EGGraphNode(ss.str());
        graph->addNode(node);
    }
    
    // connect all nodes to each other
    for (int i = 0; i < numNodes; i++) {
        for (int j = i + 1; j < numNodes; j++) {
            graph->addEdge(new EGGraphEdge(nodes[i], nodes[j]));
        }
    }
    
    delete [] nodes;
}


/* EGGraphTestRandomPairs */

EGGraphTestRandomPairs::EGGraphTestRandomPairs(EGint numNodes, EGfloat p) : numNodes(numNodes), p(p)
{
    std::stringstream ss;
    ss << "Random Pairs " << numNodes << " p=" << p;
    name = ss.str();
}

void EGGraphTestRandomPairs::populate(EGGraphPtr graph)
{
    // create super nodes
    EGGraphNode **nodes = new EGGraphNode*[numNodes];
    for (EGint i = 0; i < numNodes; i++) {
        std::stringstream ss;
        ss << "node-" << (i+1);
        EGGraphNode *node = nodes[i] = new EGGraphNode(ss.str());
        graph->addNode(node);
    }
    
    // connect nodes to each other randomly
    for (int i = 0; i < numNodes; i++) {
        for (int j = i + 1; j < numNodes; j++) {
            EGfloat r = ((EGfloat)rand() / (EGfloat)RAND_MAX);
            if (r < p) {
                graph->addEdge(new EGGraphEdge(nodes[i], nodes[j]));
            }
        }
    }
    
    delete [] nodes;
}


/* EGGraphTestRandomScaleFree */

EGGraphTestRandomScaleFree::EGGraphTestRandomScaleFree(EGint numNodes) : numNodes(numNodes)
{
    std::stringstream ss;
    ss << "Random Scale Free " << numNodes;
    name = ss.str();
}

void EGGraphTestRandomScaleFree::populate(EGGraphPtr graph)
{
    // create super nodes
    EGGraphNode **nodes = new EGGraphNode*[numNodes];
    for (EGint i = 0; i < numNodes; i++) {
        std::stringstream ss;
        ss << "node-" << (i+1);
        EGGraphNode *node = nodes[i] = new EGGraphNode(ss.str());
        graph->addNode(node);
    }
    
    // connect nodes to each other based on the sqrt of the node number of the total number of nodes
    for (int i = 0; i < numNodes; i++) {
        for (int j = i + 1; j < numNodes; j++) {
            EGfloat r = ((EGfloat)rand() / (EGfloat)RAND_MAX);
            if (r < sqrtf(i) / numNodes) {
                graph->addEdge(new EGGraphEdge(nodes[i], nodes[j]));
            }
        }
    }
    
    delete [] nodes;
}


/* EGGraphTestLoopStar */

EGGraphTestLoopStar::EGGraphTestLoopStar(EGint numSuperNodes, EGint numSubNodes, EGint numSubSubNodes)
    : numSuperNodes(numSuperNodes), numSubNodes(numSubNodes), numSubSubNodes(numSubSubNodes)
{
    std::stringstream ss;
    ss << "Loop Star " << numSuperNodes << "x" << numSubNodes << "x" << numSubSubNodes;
    name = ss.str();
}

void EGGraphTestLoopStar::populate(EGGraphPtr graph)
{
    // create super nodes
    EGGraphNode **superNodes = new EGGraphNode*[numSuperNodes];
    for (EGint i = 0; i < numSuperNodes; i++) {
        std::stringstream ss;
        ss << "super-" << (i+1);
        EGGraphNode *supernode = superNodes[i] = new EGGraphNode(ss.str());
        graph->addNode(supernode);
        // create sub nodes for each super node
        for (EGint j = 0; j < numSubNodes; j++) {
            std::stringstream ss;
            ss << "sub-" << (j+1);
            EGGraphNode *subnode = new EGGraphNode(ss.str());
            graph->addNode(subnode);
            graph->addEdge(new EGGraphEdge(supernode, subnode));
            // create sub sub nodes for each sub node
            for (EGint k = 0; k < numSubSubNodes; k++) {
                std::stringstream ss;
                ss << "ssub-" << (k+1);
                EGGraphNode *subsubnode = new EGGraphNode(ss.str());
                graph->addNode(subsubnode);
                graph->addEdge(new EGGraphEdge(subnode, subsubnode));
            }
        }
    }
    // create edges between adjacent super nodes
    if (numSuperNodes > 1) {
        for (EGint i = 0; i < numSuperNodes; i++) {
            graph->addEdge(new EGGraphEdge(superNodes[i], superNodes[i + 1 < numSuperNodes ? i + 1 : 0]));
        }
    }
    delete [] superNodes;
}


/* EGGraphTestGrid */

EGGraphTestGrid::EGGraphTestGrid(EGint gridX, EGint gridY) : gridX(gridX), gridY(gridY)
{
    std::stringstream ss;
    ss << "Grid " << gridX << "x" << gridY;
    name = ss.str();
}

void EGGraphTestGrid::populate(EGGraphPtr graph)
{
    EGGraphNode ***grid;
    
    // create temporary grid array
    grid = new EGGraphNode**[gridX];
    for (EGint x = 0; x < gridX; x++) {
        grid[x] = new EGGraphNode*[gridY];
    }
    
    // initialize all values to infinity
    for (EGint x = 0; x < gridX; x++) {
        for (EGint y = 0; y < gridY; y++) {
            std::stringstream ss;
            ss << (x+1) << "-" << (y+1);;
            grid[x][y] = new EGGraphNode(ss.str());
            graph->addNode(grid[x][y]);
        }
    }
    for (EGint x = 0; x < gridX; x++) {
        for (EGint y = 0; y < gridY; y++) {
            if (x != gridX - 1) graph->addEdge(new EGGraphEdge(grid[x][y], grid[x + 1][y]));
            if (y != gridY - 1) graph->addEdge(new EGGraphEdge(grid[x][y], grid[x][y + 1]));
        }
    }
    
    // delete temporary grid array
    for (EGint x = 0; x < gridX; x++) {
        delete [] grid[x];
    }
    delete [] grid;
}


/* EGGraphTestResource */

EGGraphTestResource::EGGraphTestResource(EGResourcePtr rsrc) : rsrc(rsrc)
{
    name = rsrc->getBasename();
}

void EGGraphTestResource::populate(EGGraphPtr graph)
{
    char buf[1024];
    char *line;
    
    std::map<std::string,EGGraphNode*> nodemap;
    std::map<std::string,EGGraphNode*>::iterator ni;
    
    // read data using readLine
    EGint numnodes = 0, numedges = 0;
    rsrc->open(0);
    while ((line = rsrc->readLine(buf, sizeof(buf)))) {
        char *tab = strstr(line, "\t");
        if (!tab) continue;
        *tab = '\0';
        std::string node1name = line;
        std::string node2name = tab + 1;
        EGGraphNode *node1, *node2;
        ni = nodemap.find(node1name);
        if (ni != nodemap.end()) {
            node1 = (*ni).second;
        } else {
            node1 = new EGGraphNode(node1name);
            graph->addNode(node1);
            nodemap.insert(std::pair<std::string,EGGraphNode*>(node1name, node1));
            numnodes++;
        }
        ni = nodemap.find(node2name);
        if (ni != nodemap.end()) {
            node2 = (*ni).second;
        } else {
            node2 = new EGGraphNode(node2name);
            graph->addNode(node2);
            nodemap.insert(std::pair<std::string,EGGraphNode*>(node2name, node2));
            numnodes++;
        }
        graph->addEdge(new EGGraphEdge(node1, node2));
        numedges++;
    }
    rsrc->close();
}
