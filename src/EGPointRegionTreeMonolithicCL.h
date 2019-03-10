// See LICENSE for license details.

#pragma once

// DEBUG OPTION: read the active nodes after the tree has been build to check it is complete
#define TREE_CHECK_BUILD 0


/* EGPointRegionTreeMonolithicCL */

template <typename VecType, typename ScalarType, int BranchEntries = 4, int LeafEntries = 4, typename VecTypeCL = cl_float4, typename ScalarTypeCL = cl_float>
class EGPointRegionTreeMonolithicCL
{
public:
    typedef std::shared_ptr<EGPointRegionTreeMonolithicCL<VecType,ScalarType,BranchEntries,LeafEntries,VecTypeCL,ScalarTypeCL>> TreePtr;
    
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
    EGOpenCLKernelPtr treeBuildKernel;

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

    // CL work dimensions
    EGOpenCLDimension globalWorkSize;

    // state variables
    EGint numPoints;
    EGint maxNodes;
    EGint workSize;
    std::vector<EGint> activeNodes;
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
    EGPointRegionTreeMonolithicCL(EGOpenCLContextPtr clctx, EGOpenCLCommandQueuePtr clcmdqueue) : clctx(clctx), clcmdqueue(clcmdqueue), arraysCreated(false)
    {
        std::stringstream compilerOpts;
        compilerOpts << "-DBranchEntries=" << BranchEntries << " -DLeafEntries=" << LeafEntries;
        if (sizeof(ScalarTypeCL) == sizeof(cl_double)) {
            compilerOpts << " -DVecType=double3 -DScalarType=double";
        }
        clprog = clctx->createProgram(EGResource::getResource("Resources/cl.bundle/tree_monolithic.cl"), compilerOpts.str());
        treeBuildKernel = clprog->getKernel("treeBuild");
    }
    
    ~EGPointRegionTreeMonolithicCL()
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
        
        arraysCreated = false;
    }
    
    void createArrays(EGint numPoints, size_t pointWorkGroupSize, size_t pointWorkGroupItems, size_t pointWorkGroupPoints)
    {
        this->numPoints = numPoints;
        
        maxNodes = numPoints; // * 2 ?
        workSize = (std::min)(256, (EGint)clcmdqueue->getDevice()->getMaxWorkGroupSize());
        
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
        
        // set up work dimensions
        globalWorkSize = EGOpenCLDimension(workSize);
        
        // set up treeBuild kernel arguments
        treeBuildKernel->setArg(0, globalsBuffer);
        treeBuildKernel->setArg(3, EGOpenCLLocalMemParam(sizeof(VecTypeCL) * workSize));
        treeBuildKernel->setArg(4, EGOpenCLLocalMemParam(sizeof(VecTypeCL) * workSize));
        treeBuildKernel->setArg(5, nodeCenterBuffer);
        treeBuildKernel->setArg(6, nodeHalfBuffer);
        treeBuildKernel->setArg(7, nodeMassBuffer);
        treeBuildKernel->setArg(8, nodeCenterOfGravityBuffer);
        treeBuildKernel->setArg(9, nodeStateBuffer);
        treeBuildKernel->setArg(10, nodeParentBuffer);
        treeBuildKernel->setArg(11, nodeDepthBuffer);
        treeBuildKernel->setArg(12, nodeChildCountBuffer);
        treeBuildKernel->setArg(13, nodeEntriesBuffer);
        treeBuildKernel->setArg(14, pointParentBuffer);
        treeBuildKernel->setArg(15, pointIndexBuffer);
        treeBuildKernel->setArg(16, numPoints);
        treeBuildKernel->setArg(17, maxNodes);

        // print buffer size
        EGDebug("%s treeMaxNodes=%d treeWorkSize=%d treeBufferTotal=%uK\n", __func__, maxNodes, workSize, (EGuint)(treeBufferTotal / 1024));
        
        arraysCreated = true;
    }
    
    void build(EGOpenCLBufferPtr pointPositionBuffer, EGOpenCLBufferPtr pointMassBuffer)
    {
        // exec kernel treeBuild<workSize>();
        treeBuildKernel->setArg(1, pointPositionBuffer);
        treeBuildKernel->setArg(2, pointMassBuffer);
        clcmdqueue->enqueueNDRangeKernel(treeBuildKernel, globalWorkSize, globalWorkSize);
        
#if TREE_CHECK_BUILD
        clcmdqueue->enqueueReadBuffer(globalsBuffer, true, 0, sizeof(tree_global), &globals);
        if (globals.activeNodes > 0) {
            EGDebug("%s nextNode=%d activeNodes=%d\n", __func__, globals.nextNode, globals.activeNodes);
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
        
        EGDebug("%s nextNode=%d activeNodes=%d depth=%d\n", __func__, globals.nextNode, globals.activeNodes, globals.depth);
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
