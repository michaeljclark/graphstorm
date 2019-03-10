// See LICENSE for license details.

#pragma once

struct EGGraphFloydWarshall
{
    EGGraphPtr graph;
    EGint **adjacency;
    
    EGGraphFloydWarshall(EGGraphPtr graph) : graph(graph) {}
    
    void computeMinPaths()
    {
        EGGraphNodeList *nodes = graph->getNodeList();
        EGGraphEdgeList *edges = graph->getEdgeList();
        
        // initialized adjacency matrix
        EGsize n = nodes->size();
        const EGint infinity = ((std::numeric_limits<EGint>::max)() >> 1);
        
        // create adjacency matrix
        adjacency = new EGint*[n];
        for (EGint i = 0; i < n; i++) {
            adjacency[i] = new EGint[n];
        }
        
        // initialize all values to infinity
        for (EGint i = 0; i < n; i++) {
            for (EGint j = 0; j < n; j++) {
                adjacency[i][j] = infinity;
            }
        }
        
        // set indicies on all nodes
        EGint i = 0;
        for (EGGraphNodeList::iterator ni = nodes->begin(); ni != nodes->end(); ni++) {
            EGGraphNode *node = *ni;
            node->setIndex(i++);
        }
        
        // set adjacency[i][j] to the weight of edge i, j
        for (EGGraphEdgeList::iterator ei = edges->begin(); ei != edges->end(); ei++) {
            EGGraphEdge *edge = *ei;
            EGint i = edge->getSource()->getIndex();
            EGint j = edge->getTarget()->getIndex();
            adjacency[i][j] = (EGint)edge->getLength();
        }
        
        // adjacency[i][i] should be zero for all i
        for (EGint i = 0; i < n; i++) {
            adjacency[i][i] = 0;
        }
        
        // floyd warshall
        for (EGint k = 0; k < n; k++) {
            for (EGint i = 0; i < n; i++) {
                for (EGint j = 0; j < n; j++) {
                    adjacency[i][j] = std::min( adjacency[i][j], adjacency[i][k] + adjacency[k][j] );
                }
            }
        }
    }
    
    ~EGGraphFloydWarshall()
    {
        // free adjacency matrix
        for (EGint i = 0; i < n; i++) {
            delete [] adjacency[i];
        }
        delete [] adjacency;
    }
}
