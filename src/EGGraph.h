// See LICENSE for license details.

#pragma once

class EGGraphNode;
typedef std::vector<EGGraphNode*> EGGraphNodeList;
class EGGraphEdge;
class EGGraphEdgeData;
typedef std::vector<EGGraphEdge*> EGGraphEdgeList;
class EGGraphPartition;
typedef std::vector<EGGraphPartition*> EGGraphPartitionList;
class EGGraph;
typedef std::shared_ptr<EGGraph> EGGraphPtr;
class EGGraphLayoutBase;
typedef std::shared_ptr<EGGraphLayoutBase> EGGraphLayoutBasePtr;
class EGGraphData;
typedef std::shared_ptr<EGGraphData> EGGraphDataPtr;
typedef std::map<EGuint,EGGraphDataPtr> EGGraphDataMap;
typedef std::pair<EGuint,EGGraphDataPtr> EGGraphDataPair;

typedef enum {
    EGDirectionNone,
    EGDirectionForward,
    EGDirectionReverse,
} EGDirection;


/* EGGraphData */

class EGGraphData
{
public:
    virtual ~EGGraphData() {}
};


/* EGGraphNode */

class EGGraphNode
{
protected:
    EGGraph *graph;
    EGGraphPartition *partition;
    EGint index;
    EGstring name;
    EGfloat weight;
    EGGraphEdgeList edges;
    EGGraphDataMap dataMap;
    
    friend class EGGraph;

    void addEdge(EGGraphEdge *edge);
    void removeEdge(EGGraphEdge *edge);

public:
    EGGraphNode(EGstring name, EGfloat weight = 1.0f);
    virtual ~EGGraphNode() {}
    
    EGGraph* getGraph() { return graph; }
    EGGraphPartition* getPartition() { return partition; }
    EGint getIndex() { return index; }
    EGGraphDataPtr getData(EGuint dataId);
    EGstring getName() { return name; }
    EGfloat getWeight() { return weight; }
    EGGraphEdgeList* getEdges() { return &edges; }
    
    void setIndex(EGint index) { this->index = index; }
    void setPartition(EGGraphPartition *partition) { this->partition = partition; }
    EGGraphNode* setData(EGGraphDataPair data);
    EGGraphNode* setName(EGstring name);
    EGGraphNode* setWeight(EGfloat weight);    
};


/* EGGraphNodeWeightCompare */

struct EGGraphNodeWeightCompare
{
    EGbool operator()(EGGraphNode *a, EGGraphNode *b) { return a->getWeight() < b->getWeight(); }
};


/* EGGraphEdge */

class EGGraphEdge
{
protected:
    EGGraph *graph;
    EGGraphNode *source;
    EGGraphNode *target;
    EGstring name;
    EGfloat length;
    EGDirection direction;
    EGGraphDataMap dataMap;

    friend class EGGraph;

public:
    EGGraphEdge(EGGraphNode *source, EGGraphNode *target, EGstring name = "", EGfloat length = 1.0f, EGDirection direction = EGDirectionForward);
    virtual ~EGGraphEdge() {}

    EGGraph* getGraph() { return graph; }
    EGGraphDataPtr getData(EGuint dataId);
    EGGraphNode* getSource() { return source; }
    EGGraphNode* getTarget() { return target; }
    EGstring getName() { return name; }
    EGfloat getLength() { return length; }
    EGDirection getDirection() { return direction; }
    
    EGGraphEdge* setData(EGGraphDataPair data);
    EGGraphEdge* setLength(EGfloat length);
    EGGraphEdge* setDirection(EGDirection direction);
};


/* EGGraphPartition */

class EGGraphPartition
{
    EGGraphNodeList nodes;
    
public:
    EGGraphPartition() : nodes() {}

    void addNode(EGGraphNode *node)
    {
        node->setPartition(this);
        nodes.push_back(node);
    }
    
    EGGraphNodeList* getNodes() { return &nodes; }
};


/* EGGraphNodePairOrdered */

struct EGGraphNodePairOrdered : public std::pair<EGGraphNode*,EGGraphNode*>
{
    /* An EGGraphNodePairOrdered is a canonical pair of nodes where the source and
       target order *is* significant for comparison purposes */
    
    EGGraphNodePairOrdered(EGGraphEdge *edge)
        : std::pair<EGGraphNode*,EGGraphNode*>(edge->getSource(), edge->getTarget()) {}
    
    EGGraphNodePairOrdered(EGGraphNode *a, EGGraphNode *b)
        : std::pair<EGGraphNode*,EGGraphNode*>(a, b) {}
};

/* EGGraphNodePairUnordered */

struct EGGraphNodePairUnordered : public std::pair<EGGraphNode*,EGGraphNode*>
{
    /* An EGGraphNodePairUnordered is a canonical pair of nodes where the source and
     target order is not significant for comparison purposes (achieved by sorting) */
    
    EGGraphNodePairUnordered(EGGraphEdge *edge)
        : std::pair<EGGraphNode*,EGGraphNode*>((std::min)(edge->getSource(), edge->getTarget()), (std::max)(edge->getSource(), edge->getTarget())) {}
    
    EGGraphNodePairUnordered(EGGraphNode *a, EGGraphNode *b)
        : std::pair<EGGraphNode*,EGGraphNode*>((std::min)(a, b), (std::max)(a, b)) {}
};


/* EGGraph */

class EGGraph
{
protected:
    EGGraphNodeList nodes;
    EGGraphEdgeList edges;
    EGGraphPartitionList partitions;
    EGGraphLayoutBasePtr layout;
    
    void computePartitions(EGGraphPartition *partition, EGGraphNode *node, std::list<EGGraphNode*> &nextnodes);

public:
    EGGraph();
    virtual ~EGGraph();
    
    EGGraphNodeList* getNodeList() { return &nodes; }
    EGGraphEdgeList* getEdgeList() { return &edges; }

    // add a node to the graph
    virtual EGGraphNode* addNode(EGGraphNode *node);
    
    // add an edge to the graph
    virtual EGGraphEdge* addEdge(EGGraphEdge *edge);
    
    // remove a node from the graph
    virtual EGGraphNode* removeNode(EGGraphNode *node);

    // remove an edge from the graph
    virtual EGGraphEdge* removeEdge(EGGraphEdge *edge);

    // update a node in the graph
    virtual EGGraphNode* updateNode(EGGraphNode *node);
    
    // update an edge in the graph
    virtual EGGraphEdge* updateEdge(EGGraphEdge *edge);
    
    // clear partitions
    virtual void clearPartitions();
    
    // compute parititions
    virtual void computePartitions();

    // set layout method for the graph
    virtual void setLayout(EGGraphLayoutBasePtr layout);
    
    // re-initialize layout starting position
    virtual void reinitializeLayout();
};
