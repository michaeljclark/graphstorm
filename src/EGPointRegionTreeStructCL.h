// See LICENSE for license details.

#pragma once

// DEBUG OPTION: read the active nodes after the tree has been build to check it is complete
#define TREE_CHECK_BUILD 0

// DEBUG OPTION: read the active nodes after each iteration during tree build to break early
#define TREE_READ_ACTIVE 0

// Sum center of gravity for tree nodes
#define TREE_SUM_CENTER_OF_GRAVITY 1


/* EGPointRegionTreeStructCL */

template <typename VecType, typename ScalarType, int BranchEntries = 4, int LeafEntries = 4, typename VecTypeCL = cl_float4, typename ScalarTypeCL = cl_float>
class EGPointRegionTreeStructCL
{
public:
    typedef std::shared_ptr<EGPointRegionTreeStructCL<VecType,ScalarType,BranchEntries,LeafEntries,VecTypeCL,ScalarTypeCL>> TreePtr;

    struct tree_node
    {
        VecTypeCL containerCenter;              /* node center */
        VecTypeCL containerHalf;                /* node half dimension */
        VecTypeCL centerOfGravity;              /* node center of gravity */
        int entries[LeafEntries];               /* holds either child nodes in branches or point indexes in leafs */
        unsigned int childCount[BranchEntries]; /* holds number of children for each of the indicies */
        unsigned int state;                     /* 0 = inactive, 1 = active, 2 = branch, 3 = leaf */
        int depth;                              /* node depth */
        int parent;                             /* node parent */
	    ScalarType mass;                        /* node mass */
    };
    
    struct tree_global
    {
        int nextNode;
        int activeNodes;
        int depth;
        int pad;
    };
    
protected:
    EGOpenCLContextPtr clctx;
    EGOpenCLCommandQueuePtr clcmdqueue;
    EGOpenCLProgramPtr clprog;

    // CL kernels
    EGOpenCLKernelPtr nodeSizeKernel;
    EGOpenCLKernelPtr initPointsKernel;
    EGOpenCLKernelPtr initNodesKernel;
    EGOpenCLKernelPtr getRootBoundsGroupKernel;
    EGOpenCLKernelPtr getRootBoundsSumKernel;
    EGOpenCLKernelPtr updateChildCountsKernel;
    EGOpenCLKernelPtr createNodesKernel;
    EGOpenCLKernelPtr insertPointsKernel;
    EGOpenCLKernelPtr treeSumNodesKernel;

    // CL buffers
    EGOpenCLBufferPtr nodesBuffer;
    EGOpenCLBufferPtr pointParentBuffer;
    EGOpenCLBufferPtr pointIndexBuffer;
    EGOpenCLBufferPtr globalsBuffer;
    EGOpenCLBufferPtr minBoundsBuffer;
    EGOpenCLBufferPtr maxBoundsBuffer;

    // CL work dimensions
    EGOpenCLDimension globalSizePoints;
    EGOpenCLDimension globalSizePointWorkGroupItems;
    EGOpenCLDimension globalSizeMaxNodes;

    // state variables
    EGint numPoints;
    EGint maxNodes;
    std::vector<size_t> activeNodes;
    size_t pointWorkGroupSize;
    size_t pointWorkGroupItems;
    size_t pointWorkGroupPoints;
    EGbool arraysCreated;

    // host memory buffers
    tree_global globals;
    tree_node *nodes;
    cl_uint* pointParent;
    cl_uint* pointIndex;

public:
    EGPointRegionTreeStructCL(EGOpenCLContextPtr clctx, EGOpenCLCommandQueuePtr clcmdqueue) : clctx(clctx), clcmdqueue(clcmdqueue), arraysCreated(false)
    {
        std::stringstream compilerOpts;
        compilerOpts << "-DBranchEntries=" << BranchEntries << " -DLeafEntries=" << LeafEntries;
        if (sizeof(ScalarTypeCL) == sizeof(cl_double)) {
            compilerOpts << " -DVecType=double3 -DScalarType=double";
        }
        clprog = clctx->createProgram(EGResource::getResource("Resources/cl.bundle/tree_struct.cl"), compilerOpts.str());
        nodeSizeKernel = clprog->getKernel("treeNodeSize");
        initPointsKernel = clprog->getKernel("treeInitPoints");
        initNodesKernel = clprog->getKernel("treeInitNodes");
        getRootBoundsGroupKernel = clprog->getKernel("treeGetRootBoundsGroup");
        getRootBoundsSumKernel = clprog->getKernel("treeGetRootBoundsSum");
        updateChildCountsKernel = clprog->getKernel("treeUpdateChildCounts");
        createNodesKernel = clprog->getKernel("treeCreateNodes");
        insertPointsKernel = clprog->getKernel("treeInsertPoints");
        treeSumNodesKernel = clprog->getKernel("treeSumNodes");
        
        // check tree_node structure size between host and device
        size_t sizeofnode;
        EGOpenCLBufferPtr sizeofnodeBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(size_t), NULL);
        nodeSizeKernel->setArg(0, sizeofnodeBuffer);
        clcmdqueue->enqueueTask(nodeSizeKernel);
        clcmdqueue->enqueueReadBuffer(sizeofnodeBuffer, true, 0, sizeof(size_t), &sizeofnode);
        EGDebug("%s host:sizeof(tree_node)=%d device:sizeof(tree_node)=%d\n", __func__, sizeof(tree_node), sizeofnode);
    }
    
    ~EGPointRegionTreeStructCL()
    {
        deleteArrays();
    }
        
    EGOpenCLBufferPtr& getNodesBuffer() { return nodesBuffer; }
    
    void deleteArrays()
    {
        if (!arraysCreated) return;
        
        // delete host buffers
        delete [] nodes;
        delete [] pointParent;
        delete [] pointIndex;
        
        nodes = NULL;
        pointParent = NULL;
        pointIndex = NULL;
        
        // delete OpenCL buffers
        nodesBuffer = EGOpenCLBufferPtr();
        pointParentBuffer = EGOpenCLBufferPtr();
        pointIndexBuffer = EGOpenCLBufferPtr();
        globalsBuffer = EGOpenCLBufferPtr();
        minBoundsBuffer = EGOpenCLBufferPtr();
        maxBoundsBuffer = EGOpenCLBufferPtr();
        
        arraysCreated = false;
    }
    
    void createArrays(EGint numPoints, size_t pointWorkGroupSize, size_t pointWorkGroupItems, size_t pointWorkGroupPoints)
    {
        this->numPoints = numPoints;
        this->pointWorkGroupSize = pointWorkGroupSize;
        this->pointWorkGroupItems = pointWorkGroupItems;
        this->pointWorkGroupPoints = pointWorkGroupPoints;
        
        maxNodes = numPoints; // * 2 ?
                
        // count active nodes at each level for the pathological worst case depth
        activeNodes.clear();
        int depth = 0;
        EGsize minPointsCovered = 0;
        EGsize activeNodeCount = 0;
        do {
            activeNodeCount += (EGint)pow(BranchEntries, (EGdouble)depth);
            activeNodeCount = (std::min)(activeNodeCount, (EGsize)maxNodes);
            EGDebug("%s depth=%d activeNodeCount=%d minPointsCovered=%d\n",
                    __func__, depth, activeNodeCount, minPointsCovered);
            activeNodes.push_back(activeNodeCount);
            minPointsCovered = (EGint)pow(BranchEntries/2, (EGdouble)depth);
            depth++;
        } while (minPointsCovered <= (size_t)numPoints);
        
        // create host buffers
        nodes = new tree_node[maxNodes];
        pointParent = new cl_uint[numPoints];
        pointIndex = new cl_uint[numPoints];
        
        // create OpenCL buffers
        nodesBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(tree_node) * maxNodes, NULL);
        pointParentBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(cl_uint) * numPoints, NULL);
        pointIndexBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(cl_uint) * numPoints, NULL);
        globalsBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(tree_global), NULL);
        minBoundsBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(VecTypeCL) * pointWorkGroupSize, NULL);
        maxBoundsBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(VecTypeCL) * pointWorkGroupSize, NULL);

        // sum buffer size
        EGsize treeBufferTotal = 0;
        treeBufferTotal += nodesBuffer->getSize();
        treeBufferTotal += pointParentBuffer->getSize();
        treeBufferTotal += pointIndexBuffer->getSize();
        treeBufferTotal += globalsBuffer->getSize();
        treeBufferTotal += minBoundsBuffer->getSize();
        treeBufferTotal += maxBoundsBuffer->getSize();
        
        // create work dimensions
        globalSizePoints = EGOpenCLDimension(numPoints);
        globalSizePointWorkGroupItems = EGOpenCLDimension(pointWorkGroupItems);
        globalSizeMaxNodes = EGOpenCLDimension(maxNodes);
        
        // set up initPoints kernel arguments
        initPointsKernel->setArg(0, pointParentBuffer);

        // set up initNodes kernel arguments
        initNodesKernel->setArg(0, nodesBuffer);

        // set up getRootBoundsGroup kernel arguments
        getRootBoundsGroupKernel->setArg(0, globalsBuffer);
        getRootBoundsGroupKernel->setArg(2, minBoundsBuffer);
        getRootBoundsGroupKernel->setArg(3, maxBoundsBuffer);
        getRootBoundsGroupKernel->setArg(4, (cl_int)pointWorkGroupSize);
        getRootBoundsGroupKernel->setArg(5, numPoints);

        // set up getRootBoundsSum kernel arguments
        getRootBoundsSumKernel->setArg(0, globalsBuffer);
        getRootBoundsSumKernel->setArg(1, nodesBuffer);
        getRootBoundsSumKernel->setArg(2, minBoundsBuffer);
        getRootBoundsSumKernel->setArg(3, maxBoundsBuffer);
        getRootBoundsSumKernel->setArg(4, (cl_int)pointWorkGroupItems);

        // set up updateChildCounts kernel arguments
        updateChildCountsKernel->setArg(1, nodesBuffer);
        updateChildCountsKernel->setArg(2, pointParentBuffer);
        updateChildCountsKernel->setArg(3, pointIndexBuffer);

        // set up createNodes kernel arguments
        createNodesKernel->setArg(0, globalsBuffer);
        createNodesKernel->setArg(1, nodesBuffer);
        createNodesKernel->setArg(2, pointParentBuffer);
        createNodesKernel->setArg(3, pointIndexBuffer);

        // set up insertPoints kernel arguments
        insertPointsKernel->setArg(0, nodesBuffer);
        insertPointsKernel->setArg(1, pointParentBuffer);
        insertPointsKernel->setArg(2, pointIndexBuffer);

#if TREE_SUM_CENTER_OF_GRAVITY
        // setup treeFindLeafs  kernel arguments
        treeSumNodesKernel->setArg(2, nodesBuffer);
#endif
        
        // print buffer size
        EGDebug("%s maxNodes=%d treeBufferTotal=%uK\n", __func__, maxNodes, (EGuint)(treeBufferTotal / 1024));
        
        arraysCreated = true;
    }
    
    void build(EGOpenCLBufferPtr pointPositionBuffer, EGOpenCLBufferPtr pointMassBuffer)
    {
        /*
         *   Build Quad tree
         *
         *   // requires <depth> iterations e.g. ~(6 to 10) iterations for 1024 points
         *   // parallelism increases with each subsequent iteration
         *
         *   initPoints<numPoints>();
         *   initNodes<maxnodes>();
         *   getRootBounds();
         *
         *   *nextNode = 1;
         *   *activeNodes = 1;
         *   do {
         *       updateChildCounts<numPoints>();
         *       createNodes<nextnode>();
         *       insertPoints<numPoints>();
         *       readBuffer(activeNodes)
         *       readBuffer(nextNode)
         *   } while (*activeNodes > 0)
         *
         */
        
        // setup position buffer argument on required kernels
        getRootBoundsGroupKernel->setArg(1, pointPositionBuffer);
        updateChildCountsKernel->setArg(0, pointPositionBuffer);

        // exec kernel initPoints<numPoints>();
        clcmdqueue->enqueueNDRangeKernel(initPointsKernel, globalSizePoints);
        
        // exec kernel initNodes<maxNodes>();
        clcmdqueue->enqueueNDRangeKernel(initNodesKernel, globalSizeMaxNodes);
        
        // exec kernel getRootBoundsGroup<pointWorkGroupItems>();
        clcmdqueue->enqueueNDRangeKernel(getRootBoundsGroupKernel, globalSizePointWorkGroupItems);
        
        // exec task getRootBoundsSum();
        clcmdqueue->enqueueTask(getRootBoundsSumKernel);
        
        int iter;
        for (iter = 0; iter < (int)activeNodes.size(); iter++)
        {            
            // exec kernel updateChildCountsKernel<numPoints>();
            clcmdqueue->enqueueNDRangeKernel(updateChildCountsKernel, globalSizePoints);
            
            // exec kernel createNodes<nextNode>();
            createNodesKernel->setArg(4, iter + 1);
            clcmdqueue->enqueueNDRangeKernel(createNodesKernel, EGOpenCLDimension(activeNodes.at(iter)));
            
            // exec kernel insertPoints<numPoints>();
            clcmdqueue->enqueueNDRangeKernel(insertPointsKernel, globalSizePoints);

#if TREE_READ_ACTIVE
            clcmdqueue->enqueueReadBuffer(globalsBuffer, true, 0, sizeof(tree_global), &globals);
            if (globals.activeNodes == 0) break;
#endif
        }
        
#if TREE_CHECK_BUILD
#if !TREE_READ_ACTIVE
        clcmdqueue->enqueueReadBuffer(globalsBuffer, true, 0, sizeof(tree_global), &globals);
#endif
        if (globals.activeNodes > 0) {
            EGDebug("%s iter=%d nextNode=%d activeNodes=%d\n", __func__, iter, globals.nextNode, globals.activeNodes);
        }
#endif
        
#if TREE_SUM_CENTER_OF_GRAVITY
        treeSumNodesKernel->setArg(0, pointPositionBuffer);
        treeSumNodesKernel->setArg(1, pointMassBuffer);
        
        for (iter = (int)activeNodes.size() - 1; iter >= 0; iter--)
        {
            // exec kernel treeSumNodes<nextnode>();
            treeSumNodesKernel->setArg(3, iter);
            clcmdqueue->enqueueNDRangeKernel(treeSumNodesKernel, EGOpenCLDimension(activeNodes.at(iter)));
        }
#endif
    }
    
    void dumptreeState()
    {
        clcmdqueue->enqueueReadBuffer(globalsBuffer, true, 0, sizeof(tree_global), &globals);
        clcmdqueue->enqueueReadBuffer(nodesBuffer, true, 0, sizeof(tree_node) * maxNodes, nodes);
        clcmdqueue->enqueueReadBuffer(pointParentBuffer, true, 0, sizeof(cl_uint) * numPoints, pointParent);
        clcmdqueue->enqueueReadBuffer(pointIndexBuffer, true, 0, sizeof(cl_uint) * numPoints, pointIndex);
        
        EGDebug("%s nextNode=%d activeNodes=%d depth=%d maxDepth=%d\n", __func__, globals.nextNode, globals.activeNodes, globals.depth, (int)activeNodes.size());
        for (EGint p = 0; p < numPoints; p++) {
            EGDebug("%s p[%04d]  parent=%-10u index=%-10u\n", __func__, p, pointParent[p], pointIndex[p]);
        }
        for (EGint n = 0; n < globals.nextNode; n++) {
            tree_node *node = nodes + n;
            EGfloat *center = (EGfloat*)&node->containerCenter;
            EGfloat *half = (EGfloat*)&node->containerHalf;
            EGfloat *centerOfGravity = (EGfloat*)&node->centerOfGravity;
            EGfloat *mass = (EGfloat*)&node->mass;
            cl_int *entries = (cl_int*)node->entries;
            cl_uint *childCount = (cl_uint*)node->childCount;
            if (node->state == 3) {
                std::stringstream ss;
                for (EGuint i = 0; i < LeafEntries; i++) {
                    if (i != 0) {
                        ss << ", ";
                    }
                    ss << entries[i];
                }
                if (BranchEntries == 4) {
                    EGDebug("%s n[%04d] p[%04d] d=%d s=%d c=(%8.3f,%8.3f) half=(%8.3f,%8.3f) centerOfGravity=(%8.3f,%8.3f) mass=%8.3f entries=[%s]\n",
                            __func__, n, node->parent, node->depth, node->state, center[0], center[1], half[0], half[1],
                            centerOfGravity[0], centerOfGravity[1], mass[0], ss.str().c_str());
                } else if (BranchEntries == 8) {
                    EGDebug("%s n[%04d] p[%04d] d=%d s=%d c=(%8.3f,%8.3f,%8.3f) half=(%8.3f,%8.3f,%8.3f) centerOfGravity=(%8.3f,%8.3f,%8.3f) mass=%8.3f entries=[%s]\n",
                            __func__, n, node->parent, node->depth, node->state, center[0], center[1], center[2], half[0], half[1], half[2],
                            centerOfGravity[0], centerOfGravity[1], centerOfGravity[2], mass[0], ss.str().c_str());
                }
            } else {
                std::stringstream ss1;
                std::stringstream ss2;
                for (EGuint i = 0; i < BranchEntries; i++) {
                    if (i != 0) {
                        ss1 << ", ";
                        ss2 << ", ";
                    }
                    ss1 << childCount[i];
                    ss2 << entries[i];
                }
                if (BranchEntries == 4) {
                    EGDebug("%s n[%04d] p[%04d] d=%d s=%d c=(%8.3f,%8.3f) half=(%8.3f,%8.3f) centerOfGravity=(%8.3f,%8.3f) mass=%8.3f childCount=[%s] children=[%s]\n",
                            __func__, n, node->parent, node->depth, node->state, center[0], center[1], half[0], half[1],
                            centerOfGravity[0], centerOfGravity[1], mass[0], ss1.str().c_str(), ss2.str().c_str());
                } else if (BranchEntries == 8) {
                    EGDebug("%s n[%04d] p[%04d] d=%d s=%d c=(%8.3f,%8.3f,%8.3f) half=(%8.3f,%8.3f,%8.3f) centerOfGravity=(%8.3f,%8.3f,%8.3f) mass=%8.3f childCount=[%s] children=[%s]\n",
                            __func__, n, node->parent, node->depth, node->state, center[0], center[1], center[2], half[0], half[1], half[2],
                            centerOfGravity[0], centerOfGravity[1], centerOfGravity[2], mass[0], ss1.str().c_str(), ss2.str().c_str());
                }
            }
        }
        
    }
};
