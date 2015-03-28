/*
 *  EGGraphLayoutFactory.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGGraphLayoutFactory_H
#define EGGraphLayoutFactory_H

struct EGGraphLayoutFactory;
typedef std::vector<EGGraphLayoutFactory*> EGGraphLayoutFactoryList;


/* EGGraphLayoutImpl */

struct EGGraphLayoutImpl
{
    static EGuint nextLayoutId;

    virtual ~EGGraphLayoutImpl() {}

#if USE_OPENCL
    virtual EGbool initOpenCL(EGOpenCLContextPtr, EGOpenCLCommandQueuePtr, EGbool) { return true; }
#endif
    virtual EGuint getLayoutId() = 0;
    virtual EGGraphLayoutFactory* getLayoutFactory() = 0;
    virtual EGGraphLayoutBasePtr getLayout() = 0;
    virtual void updateSettings(EGGraphLayoutSettings &settings) = 0;
    virtual void update(EGViewportPtr &viewport, EGbool refresh = true) = 0;
    virtual void draw3D(EGViewportPtr &viewport) = 0;
    virtual void draw2D(EGViewportPtr &viewport) = 0;
    virtual void setViewDimensions(EGint viewDimensions) = 0;
    virtual void setVisibleDimensions(std::vector<EGint> visibleDimensions) = 0;
    virtual void setDefaultNodeShape(EGGraphRendererParamNodeShape nodeShape) = 0;
    virtual void setDefaultNodeColor(EGColor nodeColor) = 0;
    virtual void setDefaultNodePointSize(EGfloat nodePointSize) = 0;
    virtual void setDefaultNodeShowLabels(EGbool showNodeLabels) = 0;
    virtual void setDefaultEdgeColor(EGColor edgeColor) = 0;
    virtual void setDefaultEdgeLineWidth(EGfloat edgeLineWidth) = 0;
    virtual void setDefaultEdgeShowLabels(EGbool showEdgeLabels) = 0;
    
    /* delegate to EGGraphLayout */
    
    virtual void init() = 0;
    virtual void clear() = 0;
    virtual void refreshPositions() = 0;
    virtual void deleteArrays() = 0;
    virtual void createArrays() = 0;
    virtual EGint getNumIterations() = 0;
    virtual void step() = 0;
    virtual EGfloat totalEnergy() = 0;
    virtual EGbool isRunning() = 0;
    virtual void stop() = 0;
    virtual EGGraphNode* findNodeFromScreenPosition(EGViewportPtr &viewport, EGint x, EGint y) = 0;
};

typedef std::shared_ptr<EGGraphLayoutImpl> EGGraphLayoutImplPtr;


/* EGGraphLayoutFactory */

struct EGGraphLayoutFactory
{
    static std::vector<std::string> factoryNames;
    static EGGraphLayoutFactoryList factories;
    
    static void addFactory(EGGraphLayoutFactory *factory);
    static EGGraphLayoutFactory* getFactory(std::string name, int numDimensions, EGbool requiresOpenCL);
    static void init();
    
    virtual ~EGGraphLayoutFactory() {}
    virtual int numDimensions() = 0;
    virtual EGbool requiresOpenCL() = 0;
    virtual EGGraphLayoutImplPtr create(EGGraphPtr graph, EGGraphLayoutSettings &s, EGbool useOpenCLSharing) = 0;
    virtual std::string name() = 0;
};


/* EGGraphLayoutImplBase */

template <typename VecType, typename SpringLayoutType>
struct EGGraphLayoutImplBase : EGGraphLayoutImpl
{
    typedef typename EGGraphEdgeRenderer<VecType>::RendererPtr EGGraphEdgeRendererPtr;
    typedef typename EGGraphNodeRenderer<VecType>::RendererPtr EGGraphNodeRendererPtr;
    typedef typename EGBoundsRenderer<VecType>::RendererPtr EGBoundsRendererPtr;
    typedef typename EGTreeRenderer<VecType>::RendererPtr EGTreeRendererPtr;
    typedef EGGraphPoint<VecType> EGGraphPointType;
    typedef EGGraphLayout<VecType> EGGraphLayoutType;
    typedef typename EGGraphLayoutType::LayoutPtr EGGraphLayoutPtr;
    
    const EGuint layoutId;
    EGGraphLayoutFactory *layoutFactory;
    EGGraphLayoutPtr graphLayout;
    EGGraphEdgeRendererPtr edgeRenderer;
    EGGraphEdgeRendererPtr edgeLabelRenderer;
    EGGraphNodeRendererPtr nodeRenderer;
    EGGraphNodeRendererPtr nodeLabelRenderer;
    EGBoundsRendererPtr boundsRenderer;
    EGTreeRendererPtr treeRenderer;
    EGGraphRendererParams<VecType> rendererParams;
#if USE_OPENCL
    EGOpenCLCommandQueuePtr clcmdqueue;
#endif
    
    EGGraphLayoutImplBase(EGGraphLayoutFactory *layoutFactory,
                          EGGraphLayoutPtr graphLayout,
                          EGGraphEdgeRendererPtr edgeRenderer,
                          EGGraphEdgeRendererPtr edgeLabelRenderer,
                          EGGraphNodeRendererPtr nodeRenderer,
                          EGGraphNodeRendererPtr nodeLabelRenderer,
                          EGBoundsRendererPtr boundsRenderer,
                          EGTreeRendererPtr treeRenderer)
        : layoutId(EGGraphLayoutImpl::nextLayoutId++),
          layoutFactory(layoutFactory), graphLayout(graphLayout),
          edgeRenderer(edgeRenderer), edgeLabelRenderer(edgeLabelRenderer),
          nodeRenderer(nodeRenderer), nodeLabelRenderer(nodeLabelRenderer),
          boundsRenderer(boundsRenderer), treeRenderer(treeRenderer)
    {
        // view bounds with offset
        rendererParams.viewBound = VecType(2.0f);
        rendererParams.viewOffset = VecType(-1.0f);
        rendererParams.viewDimensions = (std::min)(layoutFactory->numDimensions(), 3);
        for (int i = 0; i < rendererParams.viewDimensions; i++) {
            rendererParams.visibleDimensions.push_back(i);
        }
        rendererParams.defaultNodeShape = EGGraphRendererParamNodeShapeCube;
        rendererParams.defaultNodeSize = EGGraphRendererParamNodeSizeConstant;
        rendererParams.defaultNodeColor = EGColor(0.8f, 0.8f, 0.8f, 1.0f);
        rendererParams.defaultNodePointSize = 6.0f;
        rendererParams.defaultNodeShowLabels = false;
        rendererParams.defaultEdgeColor = EGColor(0.6f, 0.6f, 0.6f, 1.0f);
        rendererParams.defaultEdgeLineWidth = 1.0f;
        rendererParams.defaultEdgeShowLabels = false;
        rendererParams.defaultShowBounds = true;
    }
    
    EGuint getLayoutId() { return layoutId; }
    EGGraphLayoutFactory* getLayoutFactory() { return layoutFactory; }
    EGGraphLayoutBasePtr getLayout() { return graphLayout; }
    
    virtual void updateSettings(EGGraphLayoutSettings &settings)
    {
        SpringLayoutType& springLayout = static_cast<SpringLayoutType&>(*EGGraphLayoutImplBase<VecType,SpringLayoutType>::graphLayout);
        springLayout.updateSettings(static_cast<typename SpringLayoutType::SettingsType&>(settings));
    }
    
    virtual void setViewDimensions(EGint viewDimensions) { rendererParams.viewDimensions = viewDimensions; }
    virtual void setVisibleDimensions(std::vector<EGint> visibleDimensions) { rendererParams.visibleDimensions = visibleDimensions; }
    virtual void setDefaultNodeShape(EGGraphRendererParamNodeShape nodeShape) { rendererParams.defaultNodeShape = nodeShape; }
    virtual void setDefaultNodeSize(EGGraphRendererParamNodeSize nodeSize) { rendererParams.defaultNodeSize = nodeSize; }
    virtual void setDefaultNodeColor(EGColor nodeColor) { rendererParams.defaultNodeColor = nodeColor; }
    virtual void setDefaultNodePointSize(EGfloat nodePointSize) { rendererParams.defaultNodePointSize = nodePointSize; }
    virtual void setDefaultNodeShowLabels(EGbool nodeShowLabels) { rendererParams.defaultNodeShowLabels = nodeShowLabels; }
    virtual void setDefaultEdgeColor(EGColor edgeColor) { rendererParams.defaultEdgeColor = edgeColor; }
    virtual void setDefaultEdgeLineWidth(EGfloat edgeLineWidth) { rendererParams.defaultEdgeLineWidth = edgeLineWidth; }
    virtual void setDefaultEdgeShowLabels(EGbool edgeShowLabels) { rendererParams.defaultEdgeShowLabels = edgeShowLabels; }
    virtual void init() { graphLayout->init(); }
    virtual void clear() { graphLayout->clear(); }
    virtual void refreshPositions() { graphLayout->refreshPositions(true); }
    virtual void deleteArrays() { graphLayout->deleteArrays(); }
    virtual void createArrays() { graphLayout->createArrays(); }
    virtual EGint getNumIterations() { return graphLayout->getNumIterations(); }
    virtual void step() { graphLayout->step(); }
    virtual EGfloat totalEnergy() { return graphLayout->totalEnergy(); }
    virtual EGbool isRunning() { return graphLayout->isRunning(); }
    virtual void stop() { return graphLayout->stop(); }

    EGGraphNode* findNodeFromScreenPosition(EGViewportPtr &viewport, EGint x, EGint y)
    {
        // get graph bounds
        graphLayout->getBounds(rendererParams.minBound, rendererParams.maxBound);

        // refresh point positions - no-op unless GPU layout
        graphLayout->refreshPositions(true);
        
        // project points to screen coordinates
        return graphLayout->findNodeFromScreenPosition(viewport, rendererParams, x, y);
    }
    
    void update(EGViewportPtr &viewport, EGbool refresh = true)
    {
        if (refresh) {
#if 1
            static EGuint lastLayoutId = 0;
            if (layoutId != lastLayoutId) {
                lastLayoutId = layoutId;
                EGDebug("EGGraphLayout:%s id=%u layoutFactory='%s' numDimensions=%d OpenCL=%s\n",
                        __func__, layoutId, layoutFactory->name().c_str(), layoutFactory->numDimensions(),
                        layoutFactory->requiresOpenCL() ? "true" : "false");
            }
#endif

            // get graph bounds
            graphLayout->getBounds(rendererParams.minBound, rendererParams.maxBound);
                        
            // refresh point positions - no-op unless GPU layout with node or edge labels enabled
            graphLayout->refreshPositions(rendererParams.defaultEdgeShowLabels || rendererParams.defaultNodeShowLabels);

            // refresh vertex buffers
            graphLayout->updateBuffers(viewport, rendererParams);

            // update graph edges
            edgeRenderer->update(viewport, rendererParams);
            
            // update graph nodes
            nodeRenderer->update(viewport, rendererParams);
            
            // update graph edge labels
            if (rendererParams.defaultEdgeShowLabels) {
                edgeLabelRenderer->update(viewport, rendererParams);
            }

            // update graph node labels
            if (rendererParams.defaultNodeShowLabels) {
                nodeLabelRenderer->update(viewport, rendererParams);
            }

            if (boundsRenderer) {
                // update bounds
                boundsRenderer->update(viewport, rendererParams);
            }

            if (treeRenderer) {
                // update quadtree/octree
                treeRenderer->update(viewport, rendererParams);
            }
        }
    }
    
    void draw3D(EGViewportPtr &viewport)
    {
#if 1
        static EGuint lastLayoutId = 0;
        if (layoutId != lastLayoutId) {
            lastLayoutId = layoutId;
            EGDebug("EGGraphLayout:%s id=%u layoutFactory='%s' numDimensions=%d OpenCL=%s\n",
                    __func__, layoutId, layoutFactory->name().c_str(), layoutFactory->numDimensions(),
                    layoutFactory->requiresOpenCL() ? "true" : "false");
        }
#endif

        // render graph edges
        edgeRenderer->draw(viewport, rendererParams);
   
        // render graph nodes
        nodeRenderer->draw(viewport, rendererParams);
        
        if (boundsRenderer) {
            // render bounds
            boundsRenderer->draw(viewport, rendererParams);
        }

        if (treeRenderer) {
            // render quadtree/octree
            treeRenderer->draw(viewport, rendererParams);
        }
    }

    void draw2D(EGViewportPtr &viewport)
    {
#if 1
        static EGuint lastLayoutId = 0;
        if (layoutId != lastLayoutId) {
            lastLayoutId = layoutId;
            EGDebug("EGGraphLayout:%s id=%u layoutFactory='%s' numDimensions=%d OpenCL=%s\n",
                    __func__, layoutId, layoutFactory->name().c_str(), layoutFactory->numDimensions(),
                    layoutFactory->requiresOpenCL() ? "true" : "false");
        }
#endif

        // render graph edge labels
        if (rendererParams.defaultEdgeShowLabels) {
            edgeLabelRenderer->draw(viewport, rendererParams);
        }
        
        // render graph node labels
        if (rendererParams.defaultNodeShowLabels) {
            nodeLabelRenderer->draw(viewport, rendererParams);
        }
    }
};


/* EGGraphLayoutImplStandard */

template <typename VecType, typename SpringLayoutType>
struct EGGraphLayoutImplStandard : EGGraphLayoutImplBase<VecType,SpringLayoutType>
{
    typedef typename EGGraphEdgeRenderer<VecType>::RendererPtr EGGraphEdgeRendererPtr;
    typedef typename EGGraphNodeRenderer<VecType>::RendererPtr EGGraphNodeRendererPtr;
    typedef typename EGBoundsRenderer<VecType>::RendererPtr EGBoundsRendererPtr;
    typedef typename EGTreeRenderer<VecType>::RendererPtr EGTreeRendererPtr;
    typedef EGGraphPoint<VecType> EGGraphPointType;
    typedef EGGraphLayout<VecType> EGGraphLayoutType;
    typedef typename EGGraphLayoutType::LayoutPtr EGGraphLayoutPtr;
    
    EGGraphLayoutImplStandard(EGGraphLayoutFactory *layoutFactory, EGGraphLayoutPtr graphLayout,
                              EGGraphEdgeRendererPtr edgeRenderer, EGGraphEdgeRendererPtr edgeLabelRenderer,
                              EGGraphNodeRendererPtr nodeRenderer, EGGraphNodeRendererPtr nodeLabelRenderer,
                              EGBoundsRendererPtr boundsRenderer, EGTreeRendererPtr treeRenderer = EGTreeRendererPtr())
        : EGGraphLayoutImplBase<VecType,SpringLayoutType>(layoutFactory, graphLayout,
                                                          edgeRenderer, edgeLabelRenderer,
                                                          nodeRenderer, nodeLabelRenderer,
                                                          boundsRenderer, treeRenderer) {}
};


/* EGGraphLayoutFactoryStandard */

template <typename VecType, int nDim, typename SpringLayoutType>
struct EGGraphLayoutFactoryStandard : EGGraphLayoutFactory
{
    typedef typename EGGraphEdgeRenderer<VecType>::RendererPtr EGGraphEdgeRendererPtr;
    typedef typename EGGraphNodeRenderer<VecType>::RendererPtr EGGraphNodeRendererPtr;
    typedef typename EGBoundsRenderer<VecType>::RendererPtr EGBoundsRendererPtr;
    typedef typename EGTreeRenderer<VecType>::RendererPtr EGTreeRendererPtr;
    typedef EGGraphPoint<VecType> EGGraphPointType;
    typedef EGGraphLayout<VecType> EGGraphLayoutType;
    typedef typename EGGraphLayoutType::LayoutPtr EGGraphLayoutPtr;
    
    typedef EGGraphEdgeRendererES2<VecType,SpringLayoutType> EGGraphEdgeRendererType;
    typedef EGGraphNodeRendererES2<VecType,SpringLayoutType> EGGraphNodeRendererType;
    typedef EGGraphEdgeLabelRenderer<VecType,SpringLayoutType> EGGraphEdgeLabelRendererType;
    typedef EGGraphNodeLabelRenderer<VecType,SpringLayoutType> EGGraphNodeLabelRendererType;
    typedef EGBoundsRendererES2<VecType> EGBoundsRendererType;

    int numDimensions() { return nDim; }
    EGbool requiresOpenCL() { return false; }
    
    virtual EGGraphLayoutImplPtr create(EGGraphPtr graph, EGGraphLayoutSettings &s, EGbool)
    {
        EGGraphLayoutPtr graphLayout(new SpringLayoutType(static_cast<typename SpringLayoutType::SettingsType&>(s)));
        EGGraphEdgeRendererPtr edgeRenderer(new EGGraphEdgeRendererType(graph, graphLayout));
        EGGraphEdgeRendererPtr edgeLabelRenderer(new EGGraphEdgeLabelRendererType(graph, graphLayout));
        EGGraphNodeRendererPtr nodeRenderer(new EGGraphNodeRendererType(graph, graphLayout));
        EGGraphNodeRendererPtr nodeLabelRenderer(new EGGraphNodeLabelRendererType(graph, graphLayout));
        EGBoundsRendererPtr boundsRenderer(new EGBoundsRendererType());
        return EGGraphLayoutImplPtr(new EGGraphLayoutImplStandard<VecType,SpringLayoutType>(this, graphLayout,
                                                                                            edgeRenderer, edgeLabelRenderer,
                                                                                            nodeRenderer, nodeLabelRenderer,
                                                                                            boundsRenderer));
    }
};


// Spring Layout

template <typename VecType = EGVec2f, int nDim = 2, typename SpringLayoutType = EGSpringLayout2D<VecType>>
struct EGSpringLayout2DFactory : EGGraphLayoutFactoryStandard<VecType,nDim,SpringLayoutType>
{
    std::string name() { return "Spring Layout"; }
};

template <typename VecType = EGVec3f, int nDim = 3, typename SpringLayoutType = EGSpringLayout3D<VecType>>
struct EGSpringLayout3DFactory : EGGraphLayoutFactoryStandard<VecType,nDim,SpringLayoutType>
{
    std::string name() { return "Spring Layout"; }
};

template <typename VecType = EGVec4f, int nDim = 4, typename SpringLayoutType = EGSpringLayout3D<VecType>>
struct EGSpringLayout4DFactory : EGGraphLayoutFactoryStandard<VecType,nDim,SpringLayoutType>
{
    std::string name() { return "Spring Layout"; }
};


// Spring Layout with Barnes Hut Optimization

template <typename VecType, int nDim, typename SpringLayoutType>
struct EGSpringLayoutBarnesHutFactory : EGGraphLayoutFactoryStandard<VecType,nDim,SpringLayoutType>
{
    typedef typename EGGraphEdgeRenderer<VecType>::RendererPtr EGGraphEdgeRendererPtr;
    typedef typename EGGraphNodeRenderer<VecType>::RendererPtr EGGraphNodeRendererPtr;
    typedef typename EGBoundsRenderer<VecType>::RendererPtr EGBoundsRendererPtr;
    typedef typename EGTreeRenderer<VecType>::RendererPtr EGTreeRendererPtr;
    typedef EGGraphLayout<VecType> EGGraphLayoutType;
    typedef typename EGGraphLayoutType::LayoutPtr EGGraphLayoutPtr;
    
    typedef EGTreeRendererES2<typename SpringLayoutType::PointType*,typename SpringLayoutType::TreeType::EGTreeNodeType,VecType> EGSpringTreeRenderer;
    typedef EGGraphEdgeRendererES2<VecType,SpringLayoutType> EGGraphEdgeRendererType;
    typedef EGGraphNodeRendererES2<VecType,SpringLayoutType> EGGraphNodeRendererType;
    typedef EGGraphEdgeLabelRenderer<VecType,SpringLayoutType> EGGraphEdgeLabelRendererType;
    typedef EGGraphNodeLabelRenderer<VecType,SpringLayoutType> EGGraphNodeLabelRendererType;
    typedef EGBoundsRendererES2<VecType> EGBoundsRendererType;

    virtual EGGraphLayoutImplPtr create(EGGraphPtr graph, EGGraphLayoutSettings &s, EGbool)
    {
        EGGraphLayoutPtr graphLayout(new SpringLayoutType(static_cast<typename SpringLayoutType::SettingsType&>(s)));
        EGGraphEdgeRendererPtr edgeRenderer(new EGGraphEdgeRendererType(graph, graphLayout));
        EGGraphEdgeRendererPtr edgeLabelRenderer(new EGGraphEdgeLabelRendererType(graph, graphLayout));
        EGGraphNodeRendererPtr nodeRenderer(new EGGraphNodeRendererType(graph, graphLayout));
        EGGraphNodeRendererPtr nodeLabelRenderer(new EGGraphNodeLabelRendererType(graph, graphLayout));
        EGBoundsRendererPtr boundsRenderer(new EGBoundsRendererType());
        EGTreeRendererPtr treeRenderer(new EGSpringTreeRenderer(static_cast<SpringLayoutType&>(*graphLayout).getTree()));
        return EGGraphLayoutImplPtr(new EGGraphLayoutImplStandard<VecType,SpringLayoutType>(this, graphLayout,
                                                                                            edgeRenderer, edgeLabelRenderer,
                                                                                            nodeRenderer, nodeLabelRenderer,
                                                                                            boundsRenderer, treeRenderer));
    }
};

template <typename VecType = EGVec2f, int nDim = 2, typename SpringLayoutType = EGSpringLayoutBarnesHut2D<VecType>>
struct EGSpringLayoutBarnesHut2DFactory : EGSpringLayoutBarnesHutFactory<VecType,nDim,SpringLayoutType>
{
    std::string name() { return "Spring Layout (Barnes-Hut)"; }
};

template <typename VecType = EGVec3f, int nDim = 3, typename SpringLayoutType = EGSpringLayoutBarnesHut2D<VecType>>
struct EGSpringLayoutBarnesHut3DFactory : EGSpringLayoutBarnesHutFactory<VecType,nDim,SpringLayoutType>
{
    std::string name() { return "Spring Layout (Barnes-Hut)"; }
};

template <typename VecType = EGVec4f, int nDim = 4, typename SpringLayoutType = EGSpringLayoutBarnesHut2D<VecType>>
struct EGSpringLayoutBarnesHut4DFactory : EGSpringLayoutBarnesHutFactory<VecType,nDim,SpringLayoutType>
{
    std::string name() { return "Spring Layout (Barnes-Hut)"; }
};


#if USE_OPENCL

/* EGGraphLayoutImplOpenCL */

template <typename VecType, typename SpringLayoutType>
struct EGGraphLayoutImplOpenCL : EGGraphLayoutImplBase<VecType,SpringLayoutType>
{
    typedef typename EGGraphEdgeRenderer<VecType>::RendererPtr EGGraphEdgeRendererPtr;
    typedef typename EGGraphNodeRenderer<VecType>::RendererPtr EGGraphNodeRendererPtr;
    typedef typename EGBoundsRenderer<VecType>::RendererPtr EGBoundsRendererPtr;
    typedef typename EGTreeRenderer<VecType>::RendererPtr EGTreeRendererPtr;
    typedef EGGraphLayout<VecType> EGGraphLayoutType;
    typedef typename EGGraphLayoutType::LayoutPtr EGGraphLayoutPtr;
    
    EGbool useOpenCLSharing;
    
    EGGraphLayoutImplOpenCL(EGGraphLayoutFactory *layoutFactory, EGGraphLayoutPtr graphLayout,
                            EGGraphEdgeRendererPtr edgeRenderer, EGGraphEdgeRendererPtr edgeLabelRenderer,
                            EGGraphNodeRendererPtr nodeRenderer, EGGraphNodeRendererPtr nodeLabelRenderer,
                            EGBoundsRendererPtr boundsRenderer, EGTreeRendererPtr treeRenderer = EGTreeRendererPtr())
    : EGGraphLayoutImplBase<VecType,SpringLayoutType>(layoutFactory, graphLayout,
                                                      edgeRenderer, edgeLabelRenderer,
                                                      nodeRenderer, nodeLabelRenderer,
                                                      boundsRenderer, treeRenderer), useOpenCLSharing(false) {}
    
    EGbool initOpenCL(EGOpenCLContextPtr clctx, EGOpenCLCommandQueuePtr clcmdqueue, EGbool useOpenCLSharing)
    {
        this->clcmdqueue = clcmdqueue;
        this->useOpenCLSharing = useOpenCLSharing;
        return static_cast<SpringLayoutType&>(*EGGraphLayoutImplBase<VecType,SpringLayoutType>::graphLayout).initOpenCL(clctx, clcmdqueue, useOpenCLSharing);
    }
};


/* EGGraphLayoutFactoryOpenCL */

template <typename VecType, int nDim, typename SpringLayoutType>
struct EGGraphLayoutFactoryOpenCL : EGGraphLayoutFactory
{
    typedef typename EGGraphEdgeRenderer<VecType>::RendererPtr EGGraphEdgeRendererPtr;
    typedef typename EGGraphNodeRenderer<VecType>::RendererPtr EGGraphNodeRendererPtr;
    typedef typename EGBoundsRenderer<VecType>::RendererPtr EGBoundsRendererPtr;
    typedef typename EGTreeRenderer<VecType>::RendererPtr EGTreeRendererPtr;
    typedef EGGraphLayout<VecType> EGGraphLayoutType;
    typedef typename EGGraphLayoutType::LayoutPtr EGGraphLayoutPtr;
    
    typedef EGGraphEdgeRendererES2CL<VecType,SpringLayoutType> EGGraphEdgeRendererCLType;
    typedef EGGraphNodeRendererES2CL<VecType,SpringLayoutType> EGGraphNodeRendererCLType;
    typedef EGGraphEdgeRendererES2<VecType,SpringLayoutType> EGGraphEdgeRendererType;
    typedef EGGraphNodeRendererES2<VecType,SpringLayoutType> EGGraphNodeRendererType;
    typedef EGGraphEdgeLabelRenderer<VecType,SpringLayoutType> EGGraphEdgeLabelRendererType;
    typedef EGGraphNodeLabelRenderer<VecType,SpringLayoutType> EGGraphNodeLabelRendererType;
    typedef EGBoundsRendererES2<VecType> EGBoundsRendererType;

    int numDimensions() { return nDim; }
    EGbool requiresOpenCL() { return true; }
    
    virtual EGGraphLayoutImplPtr create(EGGraphPtr graph, EGGraphLayoutSettings &s, EGbool useOpenCLSharing)
    {
        EGGraphLayoutPtr graphLayout(new SpringLayoutType(static_cast<typename SpringLayoutType::SettingsType&>(s)));
        EGGraphEdgeRendererPtr edgeLabelRenderer(new EGGraphEdgeLabelRendererType(graph, graphLayout));
        EGGraphNodeRendererPtr nodeLabelRenderer(new EGGraphNodeLabelRendererType(graph, graphLayout));
        EGGraphEdgeRendererPtr edgeRenderer;
        EGGraphNodeRendererPtr nodeRenderer;
        if (useOpenCLSharing) {
            edgeRenderer = EGGraphEdgeRendererPtr(new EGGraphEdgeRendererCLType(graph, graphLayout));
            nodeRenderer = EGGraphNodeRendererPtr(new EGGraphNodeRendererCLType(graph, graphLayout));
        } else {
            edgeRenderer = EGGraphEdgeRendererPtr(new EGGraphEdgeRendererType(graph, graphLayout));
            nodeRenderer = EGGraphNodeRendererPtr(new EGGraphNodeRendererType(graph, graphLayout));
        }
        EGBoundsRendererPtr boundsRenderer(new EGBoundsRendererType());
        return EGGraphLayoutImplPtr(new EGGraphLayoutImplOpenCL<VecType,SpringLayoutType>(this, graphLayout,
                                                                                          edgeRenderer, edgeLabelRenderer,
                                                                                          nodeRenderer, nodeLabelRenderer,
                                                                                          boundsRenderer));
    }
};

// OpenCL Spring Layout

template <typename VecType = EGVec2f, int nDim = 2, typename SpringLayoutType = EGSpringLayout2DCL<VecType>>
struct EGSpringLayout2DCLFactory : EGGraphLayoutFactoryOpenCL<VecType,nDim,SpringLayoutType>
{
    std::string name() { return "Spring Layout"; }
};

template <typename VecType = EGVec3f, int nDim = 3, typename SpringLayoutType = EGSpringLayout3DCL<VecType>>
struct EGSpringLayout3DCLFactory : EGGraphLayoutFactoryOpenCL<VecType,nDim,SpringLayoutType>
{
    std::string name() { return "Spring Layout"; }
};

template <typename VecType = EGVec4f, int nDim = 4, typename SpringLayoutType = EGSpringLayout3DCL<VecType>>
struct EGSpringLayout4DCLFactory : EGGraphLayoutFactoryOpenCL<VecType,nDim,SpringLayoutType>
{
    std::string name() { return "Spring Layout"; }
};

// OpenCL Spring Layout with Barnes Hut Optimization (work in progress)

template <typename VecType = EGVec2f, int nDim = 2, typename SpringLayoutType = EGSpringLayoutBarnesHut2DCL<VecType>>
struct EGSpringLayoutBarnesHut2DCLFactory : EGGraphLayoutFactoryOpenCL<VecType,nDim,SpringLayoutType>
{
    std::string name() { return "Spring Layout (Barnes-Hut)"; }
};

template <typename VecType = EGVec3f, int nDim = 3, typename SpringLayoutType = EGSpringLayoutBarnesHut3DCL<VecType>>
struct EGSpringLayoutBarnesHut3DCLFactory : EGGraphLayoutFactoryOpenCL<VecType,nDim,SpringLayoutType>
{
    std::string name() { return "Spring Layout (Barnes-Hut)"; }
};

template <typename VecType = EGVec4f, int nDim = 4, typename SpringLayoutType = EGSpringLayoutBarnesHut3DCL<VecType>>
struct EGSpringLayoutBarnesHut4DCLFactory : EGGraphLayoutFactoryOpenCL<VecType,nDim,SpringLayoutType>
{
    std::string name() { return "Spring Layout (Barnes-Hut)"; }
};

#endif

struct EGGraphLayoutFactoryImpl
{
    static EGSpringLayout2DFactory<>            springLayout2D;
    static EGSpringLayout3DFactory<>            springLayout3D;
    static EGSpringLayout4DFactory<>            springLayout4D;
    static EGSpringLayoutBarnesHut2DFactory<>   springLayoutBarnesHut2D;
    static EGSpringLayoutBarnesHut3DFactory<>   springLayoutBarnesHut3D;
    static EGSpringLayoutBarnesHut4DFactory<>   springLayoutBarnesHut4D;
    
#if USE_OPENCL
    static EGSpringLayout2DCLFactory<>          springLayout2DCL;
    static EGSpringLayout3DCLFactory<>          springLayout3DCL;
    static EGSpringLayout4DCLFactory<>          springLayout4DCL;
    static EGSpringLayoutBarnesHut2DCLFactory<> springLayoutBarnesHut2DCL;
    static EGSpringLayoutBarnesHut3DCLFactory<> springLayoutBarnesHut3DCL;
    static EGSpringLayoutBarnesHut4DCLFactory<> springLayoutBarnesHut4DCL;
#endif
};

#endif
