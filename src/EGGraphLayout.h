/*
 *  EGGraphLayout.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGGraphLayout_H
#define EGGraphLayout_H

class EGGraphLayoutBase;
typedef std::shared_ptr<EGGraphLayoutBase> EGGraphLayoutBasePtr;
template <class VecType> struct EGGraphPoint;
template <typename VecType> class EGGraphLayout;
class EGViewport;
typedef std::shared_ptr<EGViewport> EGViewportPtr;
template <typename VecType> struct EGGraphRendererParams;


/* EGGraphPointBase */

struct EGGraphPointBase
{
    virtual ~EGGraphPointBase() {}
};


/* EGGraphPoint */

template <typename VecType>
struct EGGraphPoint : public EGGraphPointBase
{
    EGGraphNode *node;
    VecType position;
    EGint index;
    
    EGGraphPoint(EGGraphNode *node, VecType position) : node(node), position(position) {}
    
    EGGraphNode* getNode() { return node; }
    VecType getPosition() { return position; }
    
    void setPosition(VecType position) { this->position = position; }
};


/* EGGraphPointWeightCompare */

template <typename PointType>
struct EGGraphPointWeightCompare
{
    EGbool operator()(PointType *a, PointType *b) { return a->node->getWeight() < b->node->getWeight(); }
};


/* EGGraphPointEdgeCompare */

template <typename PointType>
struct EGGraphPointEdgeCompare
{
    EGbool operator()(PointType *a, PointType *b) { return a->node->getEdges()->size() < b->node->getEdges()->size(); }
};


/* EGGraphPointAdaptor */

template <typename VecType>
struct EGGraphPointAdaptor {
    VecType operator()(EGGraphPoint<VecType>* point) {
        return point->getPosition();
    }
};


/* EGGraphLayoutSettings */

struct EGGraphLayoutSettings
{
    virtual ~EGGraphLayoutSettings() {}
};


/* EGGraphLayoutBase */

class EGGraphLayoutBase
{
public:
    EGGraphLayoutBase() {}
    virtual ~EGGraphLayoutBase() {}
    
    virtual EGGraph* getGraph() = 0;
    virtual void setGraph(EGGraph *graph) = 0;
        
    virtual void addNode(EGGraphNode *node) = 0;
    virtual void addEdge(EGGraphEdge *edge) = 0;
    virtual void removeNode(EGGraphNode *node) = 0;
    virtual void removeEdge(EGGraphEdge *edge) = 0;
    virtual void updateNode(EGGraphNode *node) = 0;
    virtual void updateEdge(EGGraphEdge *edge) = 0;
    
    virtual void deleteArrays() = 0;
    virtual void refreshPositions(EGbool forceRefresh = false) = 0;
    virtual void init() = 0;
    virtual void clear() = 0;
    virtual EGint getNumIterations() = 0;
    virtual void step() = 0;
    virtual EGfloat totalEnergy() = 0;
    virtual EGbool isRunning() = 0;
    virtual void stop() = 0;
};


/* EGGraphLayout */

template <class VecType>
class EGGraphLayout : public EGGraphLayoutBase
{
protected:
    EGGraph *graph;

public:
    typedef std::shared_ptr<EGGraphLayout<VecType>> LayoutPtr;

    EGGraphLayout() : graph(NULL) {}
    
    virtual ~EGGraphLayout() {}

    EGGraph* getGraph() { return graph; }
    
    void setGraph(EGGraph *graph)
    {
        // check we are not setting the same graph
        if (this->graph == graph) return;
        
        // set the new graph
        this->graph = graph;
        
        // clear old data
        clear();
    }
    
    virtual void updateBuffers(EGViewportPtr &viewport, EGGraphRendererParams<VecType> &params) = 0;
    virtual void refreshPositions(EGbool forceRefresh = false) = 0;
    virtual void deleteArrays() = 0;
    virtual void createArrays() = 0;
    virtual EGGraphPoint<VecType>* getPoint(EGGraphNode *node) = 0;
    virtual void getBounds(VecType &minBound, VecType &maxBound) = 0;
    
    EGGraphNode* findNodeFromScreenPosition(EGViewportPtr &viewport, EGGraphRendererParams<VecType> &params, EGint x, EGint y)
    {
        // finds a node from screen position by projecting all nodes positions to screen coordinates then performing a linear search
        // TODO: potentially make this code more efficient by using an octree, or even running on the GPU
        // TODO: the code does not take account of the node size on screen which is dependent on the node shape
        const EGint hw = 5;
        EGGraphNode *selectedNode = nullptr;
        typename VecType::value_type selectedY = std::numeric_limits<typename VecType::value_type>::quiet_NaN();
        for (auto nvi : projectNodesToScreenPosition(viewport, params)) {
            VecType &v = nvi.second;
            if (x > v[0] - hw && x < v[0] + hw && y > v[1] - hw && y < v[1] + hw && (selectedY != selectedY || selectedY > v[2])) {
                selectedNode = nvi.first;
            }
        }
        return selectedNode;
    }

    std::vector<std::pair<EGGraphNode*,VecType>> projectNodesToScreenPosition(EGViewportPtr &viewport, EGGraphRendererParams<VecType> &params)
    {
        // get graph bounds
        VecType bound = params.maxBound - params.minBound;
        
        // fill dimension array
        EGint d[3];
        for (EGint i = 0; i < params.viewDimensions; i++) {
            d[i] = params.visibleDimensions[i];
        }
        
        // project node positions to screen coordinates
        std::vector<std::pair<EGGraphNode*,VecType>> nodeScreenPositions;
        EGGraphNodeList *nodes = EGGraphLayout<VecType>::graph->getNodeList();
        for (auto ni = nodes->begin(); ni != nodes->end(); ni++) {
            VecType proj = (getPoint(*ni)->getPosition() - params.minBound) / bound * params.viewBound + params.viewOffset;
            EGfloat wproj[4];
            if (params.viewDimensions == 2) {
                EGfloat wvec[4] = { proj[d[0]], proj[d[1]], 0, 1 };
                EGMath::multMat4fVec4fHomogenousDivide(wproj, wvec, viewport->modelViewProjectionMatrix);
            } else if (params.viewDimensions == 3) {
                EGfloat wvec[4] = { proj[d[0]], proj[d[1]], proj[d[2]], 1 };
                EGMath::multMat4fVec4fHomogenousDivide(wproj, wvec, viewport->modelViewProjectionMatrix);
            }
            EGfloat x = roundf((wproj[0] + 1.0f) * 0.5f * viewport->screenWidth);
            EGfloat y = roundf((-wproj[1] + 1.0f) * 0.5f * viewport->screenHeight);
            EGfloat z = wproj[2];
            nodeScreenPositions.emplace_back(std::pair<EGGraphNode*,VecType>(*ni,VecType(x, y, z)));
        }
        
        return nodeScreenPositions;
    }
};

#endif
