/*
 *  EGPointRegionTreeArrayCL.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGPointRegionTreeArrayCL_H
#define EGPointRegionTreeArrayCL_H

// DEBUG OPTION: read the active nodes after the tree has been build to check it is complete
#define TREE_CHECK_BUILD 0

// DEBUG OPTION: read the active nodes after each iteration during tree build to break early
#define TREE_READ_ACTIVE 0

// Sum center of gravity for tree nodes
#define TREE_SUM_CENTER_OF_GRAVITY 1


/* EGPointRegionTreeArrayCL */

template <typename VecType, typename ScalarType, int BranchEntries = 4, int LeafEntries = 4, typename VecTypeCL = cl_float4, typename ScalarTypeCL = cl_float>
class EGPointRegionTreeArrayCL
{
public:
    typedef std::shared_ptr<EGPointRegionTreeArrayCL<VecType,ScalarType,BranchEntries,LeafEntries,VecTypeCL,ScalarTypeCL>> TreePtr;

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
    EGOpenCLKernelPtr initPointsKernel;
    EGOpenCLKernelPtr initNodesKernel;
    EGOpenCLKernelPtr getRootBoundsGroupKernel;
    EGOpenCLKernelPtr getRootBoundsSumKernel;
    EGOpenCLKernelPtr updateChildCountsKernel;
    EGOpenCLKernelPtr createNodesKernel;
    EGOpenCLKernelPtr insertPointsKernel;
    EGOpenCLKernelPtr treeSumNodesKernel;

    // CL buffers
    EGOpenCLBufferPtr nodeCenterBuffer;
    EGOpenCLBufferPtr nodeHalfBuffer;
    EGOpenCLBufferPtr nodeCenterOfGravityBuffer;
    EGOpenCLBufferPtr nodeMassBuffer;
    EGOpenCLBufferPtr nodeEntriesBuffer;
    EGOpenCLBufferPtr nodeChildCountBuffer;
    EGOpenCLBufferPtr nodeStateBuffer;
    EGOpenCLBufferPtr nodeParentBuffer;
    EGOpenCLBufferPtr nodeDepthBuffer;
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
    tree_global globals;            // sizeof(struct tree_global)       - tree globals
    VecTypeCL* nodeCenter;          // sizeof(VecTypeCL) * maxnodes     - node center
    VecTypeCL* nodeHalf;            // sizeof(VecTypeCL) * maxnodes     - node half dimension
    VecTypeCL* nodeCenterOfGravity; // sizeof(VecTypeCL) * maxnodes     - node center of gravity
    ScalarType* nodeMass;           // sizeof(ScalarTypeCL) * maxnodes  - node mass
    cl_int* nodeEntries;            // sizeof(cl_int) * maxnodes * Q    - node child indicies in branches or point indicies in leafs
    cl_uint* nodeChildCount;        // sizeof(cl_uint) * maxnodes * 4   - node child counts for each of the indicies
    cl_uint* nodeState;             // sizeof(cl_uint) * maxnodes       - node state, 0 = inactive, 1 = active, 2 = branch, 3 = leaf
    cl_int* nodeParent;             // sizeof(cl_int) * maxnodes        - node parent
    cl_int* nodeDepth;              // sizeof(cl_int) * maxnodes        - node depth
    cl_uint* pointParent;           // sizeof(uint) * numPoints         - point parent node
    cl_uint* pointIndex;            // sizeof(uint) * numPoints         - point node indice

public:
    EGPointRegionTreeArrayCL(EGOpenCLContextPtr clctx, EGOpenCLCommandQueuePtr clcmdqueue) : clctx(clctx), clcmdqueue(clcmdqueue), arraysCreated(false)
    {
        std::stringstream compilerOpts;
        compilerOpts << "-DBranchEntries=" << BranchEntries << " -DLeafEntries=" << LeafEntries;
        if (sizeof(ScalarTypeCL) == sizeof(cl_double)) {
            compilerOpts << " -DVecType=double3 -DScalarType=double";
        }
        clprog = clctx->createProgram(EGResource::getResource("Resources/cl.bundle/tree_array.cl"), compilerOpts.str());
        initPointsKernel = clprog->getKernel("treeInitPoints");
        initNodesKernel = clprog->getKernel("treeInitNodes");
        getRootBoundsGroupKernel = clprog->getKernel("treeGetRootBoundsGroup");
        getRootBoundsSumKernel = clprog->getKernel("treeGetRootBoundsSum");
        updateChildCountsKernel = clprog->getKernel("treeUpdateChildCounts");
        createNodesKernel = clprog->getKernel("treeCreateNodes");
        insertPointsKernel = clprog->getKernel("treeInsertPoints");
        treeSumNodesKernel = clprog->getKernel("treeSumNodes");
    }
    
    ~EGPointRegionTreeArrayCL()
    {
        deleteArrays();
    }
    
    void deleteArrays()
    {
        if (!arraysCreated) return;
        
        // delete host buffers
        delete [] nodeCenter;
        delete [] nodeHalf;
        delete [] nodeCenterOfGravity;
        delete [] nodeMass;
        delete [] nodeEntries;
        delete [] nodeChildCount;
        delete [] nodeState;
        delete [] nodeParent;
        delete [] nodeDepth;
        delete [] pointParent;
        delete [] pointIndex;
        
        nodeCenter = NULL;
        nodeHalf = NULL;
        nodeCenterOfGravity = NULL;
        nodeMass = NULL;
        nodeEntries = NULL;
        nodeChildCount = NULL;
        nodeState = NULL;
        nodeParent = NULL;
        nodeDepth = NULL;
        pointParent = NULL;
        pointIndex = NULL;
        
        // delete OpenCL buffers
        nodeCenterBuffer = EGOpenCLBufferPtr();
        nodeHalfBuffer = EGOpenCLBufferPtr();
        nodeCenterOfGravityBuffer = EGOpenCLBufferPtr();
        nodeMassBuffer = EGOpenCLBufferPtr();
        nodeEntriesBuffer = EGOpenCLBufferPtr();
        nodeChildCountBuffer = EGOpenCLBufferPtr();
        nodeStateBuffer = EGOpenCLBufferPtr();
        nodeParentBuffer = EGOpenCLBufferPtr();
        nodeDepthBuffer = EGOpenCLBufferPtr();
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
        } while (minPointsCovered <= numPoints);
                
        // create host buffers
        nodeCenter = new VecTypeCL[maxNodes];
        nodeHalf = new VecTypeCL[maxNodes];
        nodeCenterOfGravity = new VecTypeCL[maxNodes];
        nodeMass = new ScalarTypeCL[maxNodes];
        nodeEntries = new cl_int[maxNodes * LeafEntries];
        nodeChildCount = new cl_uint[maxNodes * BranchEntries];
        nodeState = new cl_uint[maxNodes];
        nodeParent = new cl_int[maxNodes];
        nodeDepth = new cl_int[maxNodes];
        pointParent = new cl_uint[numPoints];
        pointIndex = new cl_uint[numPoints];
        
        // create OpenCL buffers
        nodeCenterBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(VecTypeCL) * maxNodes, NULL);
        nodeHalfBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(VecTypeCL) * maxNodes, NULL);
        nodeCenterOfGravityBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(VecTypeCL) * maxNodes, NULL);
        nodeMassBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(ScalarTypeCL) * maxNodes, NULL);
        nodeEntriesBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(cl_int) * maxNodes * LeafEntries, NULL);
        nodeChildCountBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(cl_uint) * maxNodes * BranchEntries, NULL);
        nodeStateBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(cl_uint) * maxNodes, NULL);
        nodeParentBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(cl_int) * maxNodes, NULL);
        nodeDepthBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(cl_int) * maxNodes, NULL);
        pointParentBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(cl_uint) * numPoints, NULL);
        pointIndexBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(cl_uint) * numPoints, NULL);
        globalsBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(tree_global), NULL);
        minBoundsBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(VecTypeCL) * pointWorkGroupSize, NULL);
        maxBoundsBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(VecTypeCL) * pointWorkGroupSize, NULL);
        
        // sum buffer size
        EGsize treeBufferTotal = 0;
        treeBufferTotal += nodeCenterBuffer->getSize();
        treeBufferTotal += nodeHalfBuffer->getSize();
        treeBufferTotal += nodeCenterOfGravityBuffer->getSize();
        treeBufferTotal += nodeMassBuffer->getSize();
        treeBufferTotal += nodeEntriesBuffer->getSize();
        treeBufferTotal += nodeChildCountBuffer->getSize();
        treeBufferTotal += nodeStateBuffer->getSize();
        treeBufferTotal += nodeParentBuffer->getSize();
        treeBufferTotal += nodeDepthBuffer->getSize();
        treeBufferTotal += pointParentBuffer->getSize();
        treeBufferTotal += pointIndexBuffer->getSize();
        treeBufferTotal += globalsBuffer->getSize();
        treeBufferTotal += minBoundsBuffer->getSize();
        treeBufferTotal += maxBoundsBuffer->getSize();
        
        // create work dimensions
        globalSizePoints = EGOpenCLDimension(numPoints);
        globalSizePointWorkGroupItems = EGOpenCLDimension(pointWorkGroupItems);
        globalSizeMaxNodes = EGOpenCLDimension(maxNodes);

        // setup initPoints kernel arguments
        initPointsKernel->setArg(0, pointParentBuffer);

        // setup initNodes kernel arguments
        initNodesKernel->setArg(0, nodeEntriesBuffer);
        initNodesKernel->setArg(1, nodeChildCountBuffer);
        initNodesKernel->setArg(2, nodeStateBuffer);
        initNodesKernel->setArg(3, nodeParentBuffer);
        initNodesKernel->setArg(4, nodeDepthBuffer);

        // setup getRootBoundsGroup kernel arguments
        getRootBoundsGroupKernel->setArg(0, globalsBuffer);
        getRootBoundsGroupKernel->setArg(2, minBoundsBuffer);
        getRootBoundsGroupKernel->setArg(3, maxBoundsBuffer);
        getRootBoundsGroupKernel->setArg(4, (cl_int)pointWorkGroupSize);
        getRootBoundsGroupKernel->setArg(5, numPoints);
        
        // setup getRootBoundsSum kernel arguments
        getRootBoundsSumKernel->setArg(0, globalsBuffer);
        getRootBoundsSumKernel->setArg(1, nodeCenterBuffer);
        getRootBoundsSumKernel->setArg(2, nodeHalfBuffer);
        getRootBoundsSumKernel->setArg(3, nodeStateBuffer);
        getRootBoundsSumKernel->setArg(4, nodeDepthBuffer);
        getRootBoundsSumKernel->setArg(5, minBoundsBuffer);
        getRootBoundsSumKernel->setArg(6, maxBoundsBuffer);
        getRootBoundsSumKernel->setArg(7, (cl_int)pointWorkGroupItems);

        // setup updateChildCounts kernel arguments
        updateChildCountsKernel->setArg(1, nodeCenterBuffer);
        updateChildCountsKernel->setArg(2, nodeStateBuffer);
        updateChildCountsKernel->setArg(3, nodeChildCountBuffer);
        updateChildCountsKernel->setArg(4, pointParentBuffer);
        updateChildCountsKernel->setArg(5, pointIndexBuffer);

        // setup createNodes kernel arguments
        createNodesKernel->setArg(0, globalsBuffer);
        createNodesKernel->setArg(1, nodeCenterBuffer);
        createNodesKernel->setArg(2, nodeHalfBuffer);
        createNodesKernel->setArg(3, nodeStateBuffer);
        createNodesKernel->setArg(4, nodeChildCountBuffer);
        createNodesKernel->setArg(5, nodeEntriesBuffer);
        createNodesKernel->setArg(6, nodeParentBuffer);
        createNodesKernel->setArg(7, nodeDepthBuffer);
        createNodesKernel->setArg(8, pointParentBuffer);
        createNodesKernel->setArg(9, pointIndexBuffer);
        
        // setup insertPoints kernel arguments
        insertPointsKernel->setArg(0, nodeStateBuffer);
        insertPointsKernel->setArg(1, nodeChildCountBuffer);
        insertPointsKernel->setArg(2, nodeEntriesBuffer);
        insertPointsKernel->setArg(3, pointParentBuffer);
        insertPointsKernel->setArg(4, pointIndexBuffer);

#if TREE_SUM_CENTER_OF_GRAVITY
        // setup treeFindLeafs  kernel arguments
        treeSumNodesKernel->setArg(2, nodeStateBuffer);
        treeSumNodesKernel->setArg(3, nodeEntriesBuffer);
        treeSumNodesKernel->setArg(4, nodeDepthBuffer);
        treeSumNodesKernel->setArg(5, nodeMassBuffer);
        treeSumNodesKernel->setArg(6, nodeCenterOfGravityBuffer);
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
            
            // exec kernel createNodes<nextnode>();
            createNodesKernel->setArg(10, iter + 1);
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
            treeSumNodesKernel->setArg(7, iter);
            clcmdqueue->enqueueNDRangeKernel(treeSumNodesKernel, EGOpenCLDimension(activeNodes.at(iter)));
        }
#endif
    }
    
    void dumptreeState()
    {
        clcmdqueue->enqueueReadBuffer(globalsBuffer, true, 0, sizeof(tree_global), &globals);
        clcmdqueue->enqueueReadBuffer(nodeCenterBuffer, true, 0, sizeof(VecTypeCL) * maxNodes, nodeCenter);
        clcmdqueue->enqueueReadBuffer(nodeCenterOfGravityBuffer, true, 0, sizeof(VecTypeCL) * maxNodes, nodeCenterOfGravity);
        clcmdqueue->enqueueReadBuffer(nodeMassBuffer, true, 0, sizeof(ScalarTypeCL) * maxNodes, nodeMass);
        clcmdqueue->enqueueReadBuffer(nodeHalfBuffer, true, 0, sizeof(VecTypeCL) * maxNodes, nodeHalf);
        clcmdqueue->enqueueReadBuffer(nodeEntriesBuffer, true, 0, sizeof(cl_int) * maxNodes * LeafEntries, nodeEntries);
        clcmdqueue->enqueueReadBuffer(nodeChildCountBuffer, true, 0, sizeof(cl_uint) * maxNodes * BranchEntries, nodeChildCount);
        clcmdqueue->enqueueReadBuffer(nodeStateBuffer, true, 0, sizeof(cl_uint) * maxNodes, nodeState);
        clcmdqueue->enqueueReadBuffer(nodeParentBuffer, true, 0, sizeof(cl_int) * maxNodes, nodeParent);
        clcmdqueue->enqueueReadBuffer(nodeDepthBuffer, true, 0, sizeof(cl_int) * maxNodes, nodeDepth);
        clcmdqueue->enqueueReadBuffer(pointParentBuffer, true, 0, sizeof(cl_uint) * numPoints, pointParent);
        clcmdqueue->enqueueReadBuffer(pointIndexBuffer, true, 0, sizeof(cl_uint) * numPoints, pointIndex);
        
        EGDebug("%s nextNode=%d activeNodes=%d depth=%d maxDepth=%d\n", __func__, globals.nextNode, globals.activeNodes, globals.depth, (int)activeNodes.size());
        for (EGint p = 0; p < numPoints; p++) {
            EGDebug("%s p[%04d]  parent=%-10u index=%-10u\n", __func__, p, pointParent[p], pointIndex[p]);
        }
        for (EGint n = 0; n < globals.nextNode; n++) {
            EGfloat *center = (EGfloat*)&nodeCenter[n];
            EGfloat *half = (EGfloat*)&nodeHalf[n];
            EGfloat *centerOfGravity = (EGfloat*)&nodeCenterOfGravity[n];
            EGfloat *mass = (EGfloat*)&nodeMass[n];
            cl_int *entries = nodeEntries + n * LeafEntries;
            cl_uint *childCount = nodeChildCount + n * BranchEntries;
            if (nodeState[n] == 3) {
                std::stringstream ss;
                for (EGuint i = 0; i < LeafEntries; i++) {
                    if (i != 0) {
                        ss << ", ";
                    }
                    ss << entries[i];
                }
                if (BranchEntries == 4) {
                    EGDebug("%s n[%04d] p[%04d] d=%d s=%d c=(%8.3f,%8.3f) h=(%8.3f,%8.3f) centerOfGravity=(%8.3f,%8.3f) mass=%8.3f entries=[%s]\n",
                            __func__, n, nodeParent[n], nodeDepth[n], nodeState[n], center[0], center[1], half[0], half[1],
                            centerOfGravity[0], centerOfGravity[1], mass[0], ss.str().c_str());
                } else if (BranchEntries == 8) {
                    EGDebug("%s n[%04d] p[%04d] d=%d s=%d c=(%8.3f,%8.3f,%8.3f) h=(%8.3f,%8.3f,%8.3f) centerOfGravity=(%8.3f,%8.3f,%8.3f) mass=%8.3f entries=[%s]\n",
                            __func__, n, nodeParent[n], nodeDepth[n], nodeState[n], center[0], center[1], center[2], half[0], half[1], half[2],
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
                    EGDebug("%s n[%04d] p[%04d] d=%d s=%d c=(%8.3f,%8.3f) h=(%8.3f,%8.3f) centerOfGravity=(%8.3f,%8.3f) mass=%8.3f childCount=[%s] children=[%s]\n",
                            __func__, n, nodeParent[n], nodeDepth[n], nodeState[n], center[0], center[1], half[0], half[1],
                            centerOfGravity[0], centerOfGravity[1], mass[0], ss1.str().c_str(), ss2.str().c_str());
                } else if (BranchEntries == 8) {
                    EGDebug("%s n[%04d] p[%04d] d=%d s=%d c=(%8.3f,%8.3f,%8.3f) h=(%8.3f,%8.3f,%8.3f) centerOfGravity=(%8.3f,%8.3f,%8.3f) mass=%8.3f childCount=[%s] children=[%s]\n",
                            __func__, n, nodeParent[n], nodeDepth[n], nodeState[n], center[0], center[1], center[2], half[0], half[1], half[2],
                            centerOfGravity[0], centerOfGravity[1], centerOfGravity[2], mass[0], ss1.str().c_str(), ss2.str().c_str());
                }
            }
        }
    }
};

#endif
