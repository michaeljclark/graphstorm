/*
 *  EGGraph.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGMath.h"
#include "EGViewport.h"
#include "EGVector.h"

#include "EGGraph.h"
#include "EGGraphLayout.h"


/* EGGraphNode */

EGGraphNode::EGGraphNode(EGstring name, EGfloat weight)
    : graph(NULL), partition(NULL), name(name), weight(weight) {}

EGGraphDataPtr EGGraphNode::getData(EGuint dataId)
{
    EGGraphDataMap::iterator di = dataMap.find(dataId);
    return (di != dataMap.end()) ? (*di).second : EGGraphDataPtr();
}

EGGraphNode* EGGraphNode::setData(EGGraphDataPair data)
{
    dataMap.erase(data.first);
    dataMap.insert(data);
    return this;
}

EGGraphNode* EGGraphNode::setName(EGstring name)
{
    this->name = name;
    if (graph) graph->updateNode(this);
    return this;
}

EGGraphNode* EGGraphNode::setWeight(EGfloat weight)
{
    this->weight = weight;
    if (graph) graph->updateNode(this);
    return this;
}

void EGGraphNode::addEdge(EGGraphEdge *edge)
{
    EGGraphEdgeList::iterator ei = std::find(edges.begin(), edges.end(), edge);
    if (ei == edges.end()) {
        edges.insert(edges.end(), edge);
    }
}

void EGGraphNode::removeEdge(EGGraphEdge *edge)
{
    EGGraphEdgeList::iterator ei = std::find(edges.begin(), edges.end(), edge);
    if (ei != edges.end()) {
        edges.erase(ei);
    }
}


/* EGGraphEdge */

EGGraphEdge::EGGraphEdge(EGGraphNode *source, EGGraphNode *target, EGstring name, EGfloat length, EGDirection direction)
    : graph(NULL), source(source), target(target), name(name), length(length), direction(direction) {}

EGGraphDataPtr EGGraphEdge::getData(EGuint dataId)
{
    EGGraphDataMap::iterator di = dataMap.find(dataId);
    return (di != dataMap.end()) ? (*di).second : EGGraphDataPtr();
}

EGGraphEdge* EGGraphEdge::setData(EGGraphDataPair data)
{
    dataMap.erase(data.first);
    dataMap.insert(data);
    return this;
}

EGGraphEdge* EGGraphEdge::setLength(EGfloat length)
{
    this->length = length;
    if (graph) graph->updateEdge(this);
    return this;
}

EGGraphEdge* EGGraphEdge::setDirection(EGDirection direction)
{
    this->direction = direction;
    if (graph) graph->updateEdge(this);
    return this;
}


/* EGGraph */

EGGraph::EGGraph() : layout() {}

EGGraph::~EGGraph()
{
    if (layout) {
        layout->setGraph(NULL);
    }
    for (EGGraphPartitionList::iterator pi = partitions.begin(); pi != partitions.end(); pi++) {
        delete *pi;
    }
    for (EGGraphEdgeList::iterator ei = edges.begin(); ei != edges.end(); ei++) {
        delete *ei;
    }
    for (EGGraphNodeList::iterator ni = nodes.begin(); ni != nodes.end(); ni++) {
        delete *ni;
    }
}

EGGraphNode* EGGraph::addNode(EGGraphNode *node)
{
    // set graph on the node
    node->graph = this;
    
    // add node to the graph
    nodes.insert(nodes.end(), node);
    
    // add the node to the layout
    if (layout) {
        layout->addNode(node);
    }

    return node;
}

EGGraphEdge* EGGraph::addEdge(EGGraphEdge *edge)
{
    // set graph on the edge
    edge->graph = this;
    
    // add edge to the graph
    edges.insert(edges.end(), edge);
    edge->getSource()->addEdge(edge);
    edge->getTarget()->addEdge(edge);
    
    // add the edge to the layout
    if (layout) {
        layout->addEdge(edge);
    }

    return edge;
}


EGGraphNode* EGGraph::removeNode(EGGraphNode *node)
{
    // remove the node from layout
    if (layout) {
        layout->removeNode(node);
    }
    
    // first we remove all edges
    EGGraphEdgeList *edges = node->getEdges();
    for (EGGraphEdgeList::iterator ei = edges->begin(); ei != edges->end(); ei++) {
        removeEdge(*ei);
    }
    
    // then remove the node itself
    EGGraphNodeList::iterator ni = std::find(nodes.begin(), nodes.end(), node);
    if (ni != nodes.end()) {
        nodes.erase(ni);
    }

    // remove graph from node
    node->graph = NULL;

    return node;
}

EGGraphEdge* EGGraph::removeEdge(EGGraphEdge *edge)
{
    // remove the edge from layout
    if (layout) {
        layout->removeEdge(edge);
    }
    
    // remove the edge from the nodes it links to
    edge->getSource()->removeEdge(edge);
    edge->getTarget()->removeEdge(edge);
    
    // then remove the edge itself
    EGGraphEdgeList::iterator ei = std::find(edges.begin(), edges.end(), edge);
    if (ei != edges.end()) {
        edges.erase(ei);
    }

    // remove graph from edge
    edge->graph = NULL;

    return edge;
}

EGGraphNode* EGGraph::updateNode(EGGraphNode *node)
{
    // update the node in the layout
    if (layout) {
        layout->updateNode(node);
    }
    
    return node;
}

EGGraphEdge* EGGraph::updateEdge(EGGraphEdge *edge)
{
    // update the edge in the layout
    if (layout) {
        layout->updateEdge(edge);
    }

    return edge;
}

void EGGraph::clearPartitions()
{
    for (EGGraphPartitionList::iterator pi = partitions.begin(); pi != partitions.end(); pi++) {
        delete *pi;
    }
    for (EGGraphNodeList::iterator ni = nodes.begin(); ni != nodes.end(); ni++) {
        (*ni)->setPartition(NULL);
    }
}

void EGGraph::computePartitions(EGGraphPartition *partition, EGGraphNode *node, std::list<EGGraphNode*> &nextnodes)
{
    partition->addNode(node);
    for (size_t i = 0; i < node->edges.size(); i++) {
        EGGraphEdge *edge = node->edges[i];
        if (edge->getSource()->getPartition() == NULL) {
            nextnodes.push_back(edge->getSource());
        }
        if (edge->getTarget()->getPartition() == NULL) {
            nextnodes.push_back(edge->getTarget());
        }
    }
}

void EGGraph::computePartitions()
{
    EGGraphNode *node;
    do {
        node = NULL;
        for (EGGraphNodeList::iterator ni = nodes.begin(); ni != nodes.end(); ni++) {
            if ((*ni)->getPartition() == NULL) {
                EGGraphPartition *partition = new EGGraphPartition();
                partitions.insert(partitions.end(), partition);
                std::list<EGGraphNode*> nextnodes;
                nextnodes.push_back(*ni);
                while (nextnodes.begin() != nextnodes.end()) {
                    node = *nextnodes.begin();
                    nextnodes.pop_front();
                    if (node->getPartition() == NULL) {
                        computePartitions(partition, node, nextnodes);
                    }
                };
            }
        }
    } while (node != NULL);
}

void EGGraph::setLayout(EGGraphLayoutBasePtr layout)
{
    // check if we are settings the same layout
    if (layout == this->layout) {
        return;
    }
    
    // clear graph on old layout
    if (this->layout) {
        this->layout->setGraph(NULL);
    }
    
    // set this graph on new layout and initialize it
    this->layout = layout;
    layout->setGraph(this);
    layout->init();
}

void EGGraph::reinitializeLayout()
{
    // re-initialize layout
    if (layout) {
        layout->clear();
        layout->init();
    }
}
