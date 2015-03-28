/*
 *  EGSpringLayoutCL.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGSpringLayoutCL_H
#define EGSpringLayoutCL_H

// enable to stop and dump tree after first iteration
#define TREE_DEBUG 1

// select tree implementation
#define TREE_ARRAY 0
#define TREE_STRUCT 1
#define TREE_MONOLITHIC 0

#if TREE_ARRAY
#include "EGPointRegionTreeArrayCL.h"
#endif

#if TREE_STRUCT
#include "EGPointRegionTreeStructCL.h"
#endif

#if TREE_MONOLITHIC
#include "EGPointRegionTreeMonolithicCL.h"
#endif


/* EGSpringCL */

template <typename VecType, typename NodePairType, typename ScalarType>
struct EGSpringCL : EGSpring<VecType,NodePairType,ScalarType>
{
    EGint accelSlotPoint1;
    EGint accelSlotPoint2;
    
    EGSpringCL(NodePairType nodepair, ScalarType length, ScalarType stiffness)
        : EGSpring<VecType,NodePairType,ScalarType>(nodepair, length, stiffness), accelSlotPoint1(-1), accelSlotPoint2(-1) {}
};


/* EGSpringLayoutCL */

template <typename VecType,
          typename NodePairType,
          typename ScalarType,
          EGbool buildTree,
          EGbool barnesHut,
          typename TreeType,
          typename VecTypeCL,
          typename ScalarTypeCL>
class EGSpringLayoutCL : public EGGraphLayout<VecType>
{
public:
    typedef EGSpringLayoutSettings SettingsType;
    typedef EGSpringPoint<VecType,NodePairType> PointType;
    typedef EGSpring<VecType,NodePairType> SpringType;
    typedef std::map<EGGraphNode*,PointType*> PointMap;
    typedef std::map<NodePairType,SpringType*> SpringMap;
    typedef std::pair<EGGraphNode*,PointType*> PointMapValue;
    typedef std::pair<NodePairType,SpringType*> SpringMapValue;
    
    typedef EGSpringCL<VecType,NodePairType,ScalarType> SpringCLType;
    typedef std::vector<SpringType*> SpringList;

protected:
    static const EGbool debug = false;
    
    EGOpenCLContextPtr clctx;
    EGOpenCLCommandQueuePtr clcmdqueue;
    EGOpenCLProgramPtr clspringprog;
    EGOpenCLProgramPtr clbarneshutprog;
    EGbool useOpenCLSharing;
    
    // CL kernels
    EGOpenCLKernelPtr clearPointAccelKernel;
    EGOpenCLKernelPtr sumPointGroupAccelKernel;
    EGOpenCLKernelPtr applyCoulombsLawGroupKernel;
    EGOpenCLKernelPtr applyCoulombsLawKernel;
    EGOpenCLKernelPtr applyCoulombsLawBarnesHutKernel;
    EGOpenCLKernelPtr applyHookesLawKernel;
    EGOpenCLKernelPtr sumPointSpringAccelKernel;
    EGOpenCLKernelPtr attractToCenterKernel;
    EGOpenCLKernelPtr updatePositionKernel;
    EGOpenCLKernelPtr totalEnergyGroupKernel;
    EGOpenCLKernelPtr totalEnergySumKernel;
    EGOpenCLKernelPtr getBoundsGroupKernel;
    EGOpenCLKernelPtr getBoundsGatherKernel;
    EGOpenCLKernelPtr createVertexBufferPointsKernel;
    EGOpenCLKernelPtr createVertexBufferCubesKernel;
    EGOpenCLKernelPtr createVertexBufferEdgesKernel;

    // CL point buffers
    EGOpenCLBufferObject<VecTypeCL> pointPosition;
    EGOpenCLBufferObject<VecTypeCL> pointVelocity;
    EGOpenCLBufferObject<VecTypeCL> pointAcceleration;
    EGOpenCLBufferObject<ScalarTypeCL> pointMass;
    EGOpenCLBufferObject<ScalarTypeCL> pointSize;
    EGOpenCLBufferObject<cl_uint> pointColor;
    EGOpenCLBufferObject<cl_uint> pointSpringAccelSlotOffset;
    EGOpenCLBufferObject<cl_uint> pointSpringAccelSlotCount;
    
    // CL spring buffers
    EGOpenCLBufferObject<ScalarTypeCL> springLength;
    EGOpenCLBufferObject<ScalarTypeCL> springStiffness;
    EGOpenCLBufferObject<cl_uint> springColor;
    EGOpenCLBufferObject<cl_uint> springPointIndex1;
    EGOpenCLBufferObject<cl_uint> springPointIndex2;
    EGOpenCLBufferObject<cl_uint> springAccelSlotPoint1;
    EGOpenCLBufferObject<cl_uint> springAccelSlotPoint2;

    EGOpenCLBufferPtr accelAccumBuffer;
    EGOpenCLBufferPtr energyGroupBuffer;
    EGOpenCLBufferPtr energyBuffer;
    EGOpenCLBufferPtr minBoundsBuffer;
    EGOpenCLBufferPtr maxBoundsBuffer;
    EGOpenCLBufferPtr boundsBuffer;
    EGOpenCLBufferPtr visibleDimensionsBuffer;
    EGOpenCLBufferPtr edgeiboBuffer;
    EGOpenCLBufferPtr edgevboBuffer;
    EGOpenCLBufferPtr nodeiboBuffer;
    EGOpenCLBufferPtr nodevboBuffer;
    
    // CL work dimensions
    EGOpenCLDimension globalSizePoints;
    EGOpenCLDimension globalSizeSprings;
    EGOpenCLDimension globalSizePointSpringAccelSlots;
    EGOpenCLDimension globalSizePointWorkGroupItems;
    EGOpenCLDimension globalSizePointsWorkGroupItems;
    EGOpenCLDimension globalSizeWorkGroupPointsItems;
    EGOpenCLDimension localSizeWorkGroupSize;
    
    // parameters
    ScalarType stiffness;
    ScalarType repulsion;
    ScalarType centerAttraction;
    ScalarType damping;
    ScalarType timeStep;
    ScalarType maxSpeed;
    ScalarType maxEnergy;
    ScalarType multiplier;
    
    // map of points and springs
    PointMap pointmap;
    SpringMap springmap;
    
    // array indexed buffers
    EGint numPoints;
    EGint numPointsPow2;
    EGint numSprings;
    PointType** points;
    SpringCLType** springs;
    
    // state variables
    EGbool arraysCreated;
    EGbool running;
    EGint iterCount;
    ScalarTypeCL energy;

    // CL point buffers
    size_t pointWorkGroupSize;
    size_t pointWorkGroupItems;
    size_t pointWorkGroupPoints;
    
    // OpenGL sharing
    GLuint edgeibo, edgevbo, nodeibo, nodevbo;
    size_t edgeiboSize, edgevboSize, nodeiboSize, nodevboSize;
    EGGraphRendererParamNodeShape nodeShape;
    EGint nodeViewDimensions, edgeViewDimensions;
    std::vector<EGint> lastVisibleDimensions;
    cl_uint visibleDimensions[3];
    
    // CL tree
    typedef typename TreeType::TreePtr TreePtr;
    TreePtr tree;
    
public:
    EGSpringLayoutCL(EGSpringLayoutSettings &s)
        : stiffness(s.stiffness), repulsion(s.repulsion), centerAttraction(s.centerAttraction),
          damping(s.damping), timeStep(s.timeStep), maxSpeed(s.maxSpeed), maxEnergy(s.maxEnergy), multiplier(1.0f),
          numPoints(0), numSprings(0), points(NULL), springs(NULL), arraysCreated(false), running(false),
          iterCount(0), energy((std::numeric_limits<ScalarType>::max)()),
          edgeibo(0), edgevbo(0), nodeibo(0), nodevbo(0),
          edgeiboSize(0), edgevboSize(0), nodeiboSize(0), nodevboSize(0)
    {
        EGDebug("EGSpringLayoutCL:%s barnesHut=%s %s\n", __func__, barnesHut ? "true" : "false", s.toString().c_str());
    }
    
    virtual ~EGSpringLayoutCL()
    {
        deleteArrays();

        EGDebug("EGSpringLayoutCL:%s deleted\n", __func__);
    }
    
    EGbool initOpenCL(EGOpenCLContextPtr clctx, EGOpenCLCommandQueuePtr clcmdqueue, EGbool useOpenCLSharing)
    {
        this->clctx = clctx;
        this->clcmdqueue = clcmdqueue;
        this->useOpenCLSharing = useOpenCLSharing;
        
        std::stringstream compilerOpts;
        if (sizeof(ScalarTypeCL) == sizeof(cl_double)) {
            compilerOpts << " -DVecType=double4 -DScalarType=double";
        }
        clspringprog = clctx->createProgram(EGResource::getResource("Resources/cl.bundle/springlayout.cl"), compilerOpts.str());
        clearPointAccelKernel = clspringprog->getKernel("clearPointAccel");
        sumPointGroupAccelKernel = clspringprog->getKernel("sumPointGroupAccel");
        applyCoulombsLawGroupKernel = clspringprog->getKernel("applyCoulombsLawGroup");
        applyCoulombsLawKernel = clspringprog->getKernel("applyCoulombsLaw");
        applyHookesLawKernel = clspringprog->getKernel("applyHookesLaw");
        sumPointSpringAccelKernel = clspringprog->getKernel("sumPointSpringAccel");
        attractToCenterKernel = clspringprog->getKernel("attractToCenter");
        updatePositionKernel = clspringprog->getKernel("updatePosition");
        totalEnergyGroupKernel = clspringprog->getKernel("totalEnergyGroup");
        totalEnergySumKernel = clspringprog->getKernel("totalEnergySum");
        getBoundsGroupKernel = clspringprog->getKernel("getBoundsGroup");
        getBoundsGatherKernel = clspringprog->getKernel("getBoundsGather");
        createVertexBufferPointsKernel = clspringprog->getKernel("createVertexBufferPoints");
        createVertexBufferCubesKernel = clspringprog->getKernel("createVertexBufferCubes");
        createVertexBufferEdgesKernel = clspringprog->getKernel("createVertexBufferEdges");
        
        if (buildTree) {
            tree = TreePtr(new TreeType(clctx, clcmdqueue));
        }
        if (barnesHut) {
#if TREE_STRUCT
            clbarneshutprog = clctx->createProgram(EGResource::getResource("Resources/cl.bundle/barnes_hut_struct.cl"), compilerOpts.str());
#endif
#if TREE_ARRAY || TREE_MONOLITHIC
            clbarneshutprog = clctx->createProgram(EGResource::getResource("Resources/cl.bundle/barnes_hut_array.cl"), compilerOpts.str());
#endif
            applyCoulombsLawBarnesHutKernel = clbarneshutprog->getKernel("applyCoulombsLawBarnesHut");
        }
        
        // NOTE: add error checking for compile
        
        return true;
    }
    
    void updateSettings(EGSpringLayoutSettings &s)
    {
        stiffness = s.stiffness;
        repulsion = s.repulsion;
        centerAttraction = s.centerAttraction;
        damping = s.damping;
        timeStep = s.timeStep;
        maxSpeed = s.maxSpeed;
        maxEnergy = s.maxEnergy;
        
        // rebind kernel arguments
        bindKernelArguments();
    }
    
    void updateVisibleDimensions(std::vector<EGint> &newVisibleDimensions)
    {
        if (newVisibleDimensions.size() != lastVisibleDimensions.size() ||
            !std::equal(newVisibleDimensions.begin(), newVisibleDimensions.end(), lastVisibleDimensions.begin()))
        {
            for (EGsize i = 0; i < 3; i++) {
                if (i < newVisibleDimensions.size()) {
                    visibleDimensions[i] = newVisibleDimensions[i];
                } else {
                    visibleDimensions[i] = 0;
                }
            }
            lastVisibleDimensions = newVisibleDimensions;
        }
        clcmdqueue->enqueueWriteBuffer(visibleDimensionsBuffer, true, 0, visibleDimensionsBuffer->getSize(), visibleDimensions);
    }
    
    void populateBounds(EGGraphRendererParams<VecType> &params, VecTypeCL &minBound, VecTypeCL &maxBound, VecTypeCL &viewBound, VecTypeCL &viewOffset)
    {
        const size_t nelem = sizeof(VecTypeCL) / sizeof(ScalarTypeCL);
        for (size_t i = 0; i < nelem; i++) {
            minBound.s[i] = i < params.minBound.dim() ? params.minBound[i] : 0.0f;
            maxBound.s[i] = i < params.maxBound.dim() ? params.maxBound[i] : 1.0f;
            viewBound.s[i] = i < params.viewBound.dim() ? params.viewBound[i] : 0.0f;
            viewOffset.s[i] = i < params.viewOffset.dim() ? params.viewOffset[i] : 0.0f;
        }
    }
        
    void updateBuffers(EGViewportPtr &viewport, EGGraphRendererParams<VecType> &params)
    {
        if(!(useOpenCLSharing && clctx->glSharingIsEnabled())) return;

        createArrays();

        if (params.defaultNodeShape == EGGraphRendererParamNodeShapePoint &&
            (nodevboSize == 0 ||
             params.defaultNodeShape != nodeShape ||
             params.viewDimensions != nodeViewDimensions))
        {
            // if the nodeShape or view dimensions have changed, we need to regenerate the buffers
            nodeShape = params.defaultNodeShape;
            nodeViewDimensions = params.viewDimensions;
            
            if (params.viewDimensions == 2) {
                nodevboSize = (numPoints + 1) * sizeof(EGfloat) * 4;
                nodeiboSize = (numPoints + 1) * sizeof(EGVertexIndex);
            }
            if (params.viewDimensions == 3) {
                nodevboSize = (numPoints + 1) * sizeof(EGfloat) * 5;
                nodeiboSize = (numPoints + 1) * sizeof(EGVertexIndex);
            }
            
            // clear OpenCL buffers
            nodeiboBuffer = EGOpenCLBufferPtr();
            nodevboBuffer = EGOpenCLBufferPtr();
            clcmdqueue->finish();
            
            if (nodeibo) {
                glDeleteBuffers(1, &nodeibo);
                EGDebug("EGSpringLayoutCL:%s deleted node vertex index buffer %d\n", __func__, nodeibo);
                nodeibo = 0;
            }
            if (nodevbo) {
                glDeleteBuffers(1, &nodevbo);
                EGDebug("EGSpringLayoutCL:%s deleted node index buffer %d\n", __func__, nodevbo);
                nodevbo = 0;
            }
            glGenBuffers(1, &nodeibo);
            glGenBuffers(1, &nodevbo);
            
            // create OpenGL buffers
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nodeibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, nodeiboSize, NULL, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            
            glBindBuffer(GL_ARRAY_BUFFER, nodevbo);
            glBufferData(GL_ARRAY_BUFFER, nodevboSize, NULL, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glFinish();

            EGDebug("EGSpringLayoutCL:%s created node vertex index buffer %d: size=%d\n", __func__, nodeibo, (int)nodeiboSize);
            EGDebug("EGSpringLayoutCL:%s created node vertex buffer %d: size=%d\n", __func__, nodevbo, (int)nodevboSize);
        }
        
        if (params.defaultNodeShape == EGGraphRendererParamNodeShapeCube &&
            (nodevboSize == 0 ||
             params.defaultNodeShape != nodeShape ||
             params.viewDimensions != nodeViewDimensions))
        {
            // if the nodeShape or view dimensions have changed, we need to regenerate the buffers
            nodeShape = params.defaultNodeShape;
            nodeViewDimensions = params.viewDimensions;
            
            if (params.viewDimensions == 2) {
                nodevboSize = (numPoints + 1) * sizeof(EGfloat) * 12; // 3*4
                nodeiboSize = (numPoints + 1) * sizeof(EGVertexIndex) * 6;
            }
            if (params.viewDimensions == 3) {
                nodevboSize = (numPoints + 1) * sizeof(EGfloat) * 168; // 7*24
                nodeiboSize = (numPoints + 1) * sizeof(EGVertexIndex) * 36;
            }
            
            // clear OpenCL buffers
            nodeiboBuffer = EGOpenCLBufferPtr();
            nodevboBuffer = EGOpenCLBufferPtr();
            clcmdqueue->finish();
            
            if (nodeibo) {
                glDeleteBuffers(1, &nodeibo);
                EGDebug("EGSpringLayoutCL:%s deleted node vertex index buffer %d\n", __func__, nodeibo);
                nodeibo = 0;
            }
            if (nodevbo) {
                glDeleteBuffers(1, &nodevbo);
                EGDebug("EGSpringLayoutCL:%s deleted node vertex buffer %d\n", __func__, nodevbo);
                nodevbo = 0;
            }
            glGenBuffers(1, &nodeibo);
            glGenBuffers(1, &nodevbo);
            
            // create OpenGL buffers
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nodeibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, nodeiboSize, NULL, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            
            glBindBuffer(GL_ARRAY_BUFFER, nodevbo);
            glBufferData(GL_ARRAY_BUFFER, nodevboSize, NULL, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glFinish();

            EGDebug("EGSpringLayoutCL:%s created node vertex index buffer %d: size=%d\n", __func__, nodeibo, (int)nodeiboSize);
            EGDebug("EGSpringLayoutCL:%s created node vertex buffer %d: size=%d\n", __func__, nodevbo, (int)nodevboSize);
        }
        
        if (edgevboSize == 0 ||
            params.viewDimensions != edgeViewDimensions)
        {
            // if the view dimensions have changed, we need to regenerate the buffers
            edgeViewDimensions = params.viewDimensions;
            
            if (params.viewDimensions == 2) {
                edgevboSize = (numSprings + 1) * sizeof(EGfloat) * 6; // 3 * 2
                edgeiboSize = (numSprings + 1) * sizeof(EGVertexIndex) * 2;
            }
            if (params.viewDimensions == 3) {
                edgevboSize = (numSprings + 1) * sizeof(EGfloat) * 8; // 4 * 2
                edgeiboSize = (numSprings + 1) * sizeof(EGVertexIndex) * 2;
            }
            
            // clear OpenCL buffers
            edgeiboBuffer = EGOpenCLBufferPtr();
            edgevboBuffer = EGOpenCLBufferPtr();
            clcmdqueue->finish();
            
            if (edgeibo) {
                glDeleteBuffers(1, &edgeibo);
                EGDebug("EGSpringLayoutCL:%s deleted edge vertex index buffer %d\n", __func__, edgeibo);
                edgeibo = 0;
            }
            if (edgevbo) {
                glDeleteBuffers(1, &edgevbo);
                EGDebug("EGSpringLayoutCL:%s deleted edge vertex buffer %d\n", __func__, edgevbo);
                edgevbo = 0;
            }
            glGenBuffers(1, &edgeibo);
            glGenBuffers(1, &edgevbo);

            // create OpenGL buffers
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, edgeiboSize, NULL, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            
            glBindBuffer(GL_ARRAY_BUFFER, edgevbo);
            glBufferData(GL_ARRAY_BUFFER, edgevboSize, NULL, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        
            glFinish();

            EGDebug("EGSpringLayoutCL:%s created edge vertex index buffer %d: size=%d\n", __func__, edgeibo, (int)edgeiboSize);
            EGDebug("EGSpringLayoutCL:%s created edge vertex buffer %d: size=%d\n", __func__, edgevbo, (int)edgevboSize);
        }
        
        // create OpenCL buffers for any buffers that have changed
        if (nodeibo && !nodeiboBuffer) {
            nodeiboBuffer = clctx->createBufferFromGLBuffer(CL_MEM_READ_WRITE, nodeibo);
        }
        if (nodevbo && !nodevboBuffer) {
            nodevboBuffer = clctx->createBufferFromGLBuffer(CL_MEM_READ_WRITE, nodevbo);
        }
        if (edgeibo && !edgeiboBuffer) {
            edgeiboBuffer = clctx->createBufferFromGLBuffer(CL_MEM_READ_WRITE, edgeibo);
        }
        if (edgevbo && !edgevboBuffer) {
            edgevboBuffer = clctx->createBufferFromGLBuffer(CL_MEM_READ_WRITE, edgevbo);
        }
        
        // populate bounds
        VecTypeCL minBound, maxBound, viewBound, viewOffset;
        populateBounds(params, minBound, maxBound, viewBound, viewOffset);
        
        // update visible dimensions buffer
        updateVisibleDimensions(params.visibleDimensions);
        
        if (params.defaultNodeShape == EGGraphRendererParamNodeShapePoint) {
            createVertexBufferPointsKernel->setArgs(pointPosition, pointSize, pointColor, nodevboBuffer, nodeiboBuffer,
                                                    EGOpenCLParam(sizeof(VecTypeCL), &minBound),
                                                    EGOpenCLParam(sizeof(VecTypeCL), &maxBound),
                                                    EGOpenCLParam(sizeof(VecTypeCL), &viewBound),
                                                    EGOpenCLParam(sizeof(VecTypeCL), &viewOffset),
                                                    (cl_float)params.defaultNodePointSize,
                                                    visibleDimensionsBuffer, (cl_int)params.viewDimensions);
        }
        if (params.defaultNodeShape == EGGraphRendererParamNodeShapeCube) {
            const EGfloat cubeScale = 0.5f * params.defaultNodePointSize / ((viewport->screenWidth + viewport->screenHeight) * 0.25f);
            createVertexBufferCubesKernel->setArgs(pointPosition, pointSize, pointColor, nodevboBuffer, nodeiboBuffer,
                                                   EGOpenCLParam(sizeof(VecTypeCL), &minBound),
                                                   EGOpenCLParam(sizeof(VecTypeCL), &maxBound),
                                                   EGOpenCLParam(sizeof(VecTypeCL), &viewBound),
                                                   EGOpenCLParam(sizeof(VecTypeCL), &viewOffset),
                                                   (cl_float)cubeScale, visibleDimensionsBuffer, (cl_int)params.viewDimensions);
        }
        createVertexBufferEdgesKernel->setArgs(pointPosition, springColor, springPointIndex1, springPointIndex2,
                                               edgevboBuffer, edgeiboBuffer,
                                               EGOpenCLParam(sizeof(VecTypeCL), &minBound),
                                               EGOpenCLParam(sizeof(VecTypeCL), &maxBound),
                                               EGOpenCLParam(sizeof(VecTypeCL), &viewBound),
                                               EGOpenCLParam(sizeof(VecTypeCL), &viewOffset),
                                               visibleDimensionsBuffer, (cl_int)params.viewDimensions);

        if (params.defaultNodeShape == EGGraphRendererParamNodeShapePoint) {
            clcmdqueue->enqueueAcquireGLObjects(EGOpenCLBufferList(nodevboBuffer, edgeiboBuffer, edgevboBuffer));
            clcmdqueue->enqueueNDRangeKernel(createVertexBufferPointsKernel, globalSizePoints);
            clcmdqueue->enqueueNDRangeKernel(createVertexBufferEdgesKernel, globalSizeSprings);
            clcmdqueue->enqueueReleaseGLObjects(EGOpenCLBufferList(nodevboBuffer, edgeiboBuffer, edgevboBuffer));
        } else if (params.defaultNodeShape == EGGraphRendererParamNodeShapeCube) {
            clcmdqueue->enqueueAcquireGLObjects(EGOpenCLBufferList(nodevboBuffer, nodeiboBuffer, edgeiboBuffer, edgevboBuffer));
            clcmdqueue->enqueueNDRangeKernel(createVertexBufferCubesKernel, globalSizePoints);
            clcmdqueue->enqueueNDRangeKernel(createVertexBufferEdgesKernel, globalSizeSprings);
            clcmdqueue->enqueueReleaseGLObjects(EGOpenCLBufferList(nodevboBuffer, nodeiboBuffer, edgeiboBuffer, edgevboBuffer));
        }
        
        clcmdqueue->finish();
    }

    void getVertexBufferPoints(EGGraphRendererParams<VecType> &params, GLuint &nodeibo, GLuint &nodevbo, GLuint &nodeind)
    {        
        nodeind = numPoints;
        
        nodeibo = this->nodeibo;
        nodevbo = this->nodevbo;
    }
    
    void getVertexBufferCubes(EGGraphRendererParams<VecType> &params, GLuint &nodeibo, GLuint &nodevbo, GLuint &nodeind)
    {
        if (params.viewDimensions == 2) nodeind = numPoints * 6;
        if (params.viewDimensions == 3) nodeind = numPoints * 36;
        
        nodeibo = this->nodeibo;
        nodevbo = this->nodevbo;
    }
    
    void getVertexBufferEdges(EGGraphRendererParams<VecType> &params, GLuint &edgeibo, GLuint &edgevbo, GLuint &edgeind)
    {
        edgeind = numSprings * 2;
        edgeibo = this->edgeibo;
        edgevbo = this->edgevbo;
    }
    
    void refreshPositions(EGbool forceRefresh)
    {
        if (pointPosition.getSize() == 0 || (!forceRefresh && clctx->glSharingIsEnabled() && useOpenCLSharing)) return;
        
        // read positions
        pointPosition.copyToHost(clcmdqueue);
        
        // update point positions
        const size_t nelem = sizeof(VecTypeCL) / sizeof(ScalarTypeCL);
        for (EGint idx = 0; idx < numPoints; idx++) {
            for (size_t i = 0; i < points[idx]->position.dim(); i++) {
                points[idx]->position[i] = i < nelem ? pointPosition[idx].s[i] : 0.0f;
            }
        }
    }
    
    void getBounds(VecType &minBounds, VecType &maxBounds)
    {
        createArrays();
        
        // exec task getBoundsGather
        clcmdqueue->enqueueNDRangeKernel(getBoundsGroupKernel, globalSizePointWorkGroupItems);
        
        // exec task getBoundsGather
        clcmdqueue->enqueueTask(getBoundsGatherKernel);
        
        VecTypeCL bounds[2];
        clcmdqueue->enqueueReadBuffer(boundsBuffer, true, 0, sizeof(VecTypeCL) * 2, bounds);
        const size_t nelem = sizeof(VecTypeCL) / sizeof(ScalarTypeCL);
        for (size_t i = 0; i < minBounds.dim(); i++) {
            minBounds[i] = i < nelem ? bounds[0].s[i] : 0.0f;
            maxBounds[i] = i < nelem ? bounds[1].s[i] : 0.0f;
        }
    }

    /* EGGraphLayout methods */
    
    void init()
    {
        EGGraphNodeList *nodes = EGGraphLayout<VecType>::graph->getNodeList();
        EGGraphEdgeList *edges = EGGraphLayout<VecType>::graph->getEdgeList();

        // add points and set initial positions for all nodes
        multiplier = (ScalarType)nodes->size() / (3.1415926f * 2.0f);
        for (auto ni = nodes->begin(); ni != nodes->end(); ni++) {
            addNode(*ni);
        }
        
        // add springs and set initial values for all edges
        for (auto ei = edges->begin(); ei != edges->end(); ei++) {
            addEdge(*ei);
        }
        
        // sort points by weight/centrality
        //EGGraphPointWeightCompare<PointType> weightCompare;
        //std::sort(points, points + numPoints, weightCompare);

        EGDebug("EGSpringLayoutCL:%s numPoints=%d numSprings=%d\n", __func__, (EGint)pointmap.size(), (EGint)springmap.size());

        createArrays();
    }
    
    void clear()
    {
        deleteArrays();
        pointmap.clear();
        springmap.clear();
    }
    
    void deleteArrays()
    {
        if (!arraysCreated) return;

        // upload positions back to GPU
        refreshPositions(true);

        delete [] springs;
        delete [] points;
        
        springs = nullptr;
        points = nullptr;

        // clear point buffers
        pointPosition.clear();
        pointVelocity.clear();
        pointAcceleration.clear();
        pointMass.clear();
        pointSize.clear();
        pointColor.clear();
        pointSpringAccelSlotOffset.clear();
        pointSpringAccelSlotCount.clear();
        
        // clear spring buffers
        springLength.clear();
        springStiffness.clear();
        springColor.clear();
        springPointIndex1.clear();
        springPointIndex2.clear();
        springAccelSlotPoint1.clear();
        springAccelSlotPoint2.clear();
        
        // delete acceleration accumulation buffer
        accelAccumBuffer = EGOpenCLBufferPtr();
        
        // delete energy buffers
        energyGroupBuffer = EGOpenCLBufferPtr();
        energyBuffer = EGOpenCLBufferPtr();
        
        // delete bounds buffers
        boundsBuffer = EGOpenCLBufferPtr();
        minBoundsBuffer = EGOpenCLBufferPtr();
        maxBoundsBuffer = EGOpenCLBufferPtr();

        // delete visible dimensions buffer
        visibleDimensionsBuffer = EGOpenCLBufferPtr();
        lastVisibleDimensions.clear();

        // delete tree buffers
        if (buildTree) {
            tree->deleteArrays();
        }
        
        // delete GL/CL sharing buffers
        edgeiboBuffer = EGOpenCLBufferPtr();
        edgevboBuffer = EGOpenCLBufferPtr();
        nodeiboBuffer = EGOpenCLBufferPtr();
        nodevboBuffer = EGOpenCLBufferPtr();
        
        clcmdqueue->finish();
        
        if (edgevbo) {
            glDeleteBuffers(1, &edgevbo);
            EGDebug("EGSpringLayoutCL:%s deleted edge vertex buffer %d\n", __func__, edgevbo);
            edgevbo = 0;
            edgevboSize = 0;
        }
        if (edgeibo) {
            glDeleteBuffers(1, &edgeibo);
            EGDebug("EGSpringLayoutCL:%s deleted edge vertex index buffer %d\n", __func__, edgeibo);
            edgeibo = 0;
            edgeiboSize = 0;
        }
        if (nodevbo) {
            glDeleteBuffers(1, &nodevbo);
            EGDebug("EGSpringLayoutCL:%s deleted node vertex buffer %d\n", __func__, nodevbo);
            nodevbo = 0;
            nodevboSize = 0;
        }
        if (nodeibo) {
            glDeleteBuffers(1, &nodeibo);
            EGDebug("EGSpringLayoutCL:%s deleted node vertex index buffer %d\n", __func__, nodeibo);
            nodeibo = 0;
            nodeiboSize = 0;
        }
        
        glFinish();

        arraysCreated = false;
    }
    
    EGint nearestPow2(EGint r)
    {
        r--;
        r |= r >> 1;
        r |= r >> 2;
        r |= r >> 4;
        r |= r >> 8;
        r |= r >> 16;
        r++;
        return r;
    }
    
    void createArrays()
    {
        if (arraysCreated) return;
        
        // create points array
        numPoints = (EGint)pointmap.size();
        PointType** pptr = points = new PointType*[numPoints];
        for (auto pi = pointmap.begin(); pi != pointmap.end(); pi++) {
            *(pptr++) = (*pi).second;
        }
        
        // sort points by weight
        EGGraphPointWeightCompare<PointType> weightCompare;
        std::sort(points, points + numPoints, weightCompare);
        
        // create springs array
        numSprings = (EGint)springmap.size();
        SpringCLType** sptr = springs = new SpringCLType*[numSprings];
        for (auto si = springmap.begin(); si != springmap.end(); si++) {
            *(sptr++) = static_cast<SpringCLType*>((*si).second);
        }

        // create point opencl buffers
        numPointsPow2 = nearestPow2(numPoints);
        pointPosition.setSize(clctx, numPoints);
        pointVelocity.setSize(clctx, numPoints);
        pointAcceleration.setSize(clctx, numPoints);
        pointMass.setSize(clctx, numPoints);
        pointSize.setSize(clctx, numPoints);
        pointColor.setSize(clctx, numPoints);
        pointSpringAccelSlotOffset.setSize(clctx, numPoints);
        pointSpringAccelSlotCount.setSize(clctx, numPoints);

        // create spring opencl buffers
        springLength.setSize(clctx, numSprings);
        springStiffness.setSize(clctx, numSprings);
        springColor.setSize(clctx, numSprings);
        springPointIndex1.setSize(clctx, numSprings);
        springPointIndex2.setSize(clctx, numSprings);
        springAccelSlotPoint1.setSize(clctx, numSprings);
        springAccelSlotPoint2.setSize(clctx, numSprings);

        // set point data in opencl buffers
        EGuint accelSlot = 0;
        const size_t nelem = sizeof(VecTypeCL) / sizeof(ScalarTypeCL);
        for (EGint pnum = 0; pnum < numPoints; pnum++) {
            PointType *point = points[pnum];
            point->index = pnum;
            for (size_t i = 0; i < nelem; i++) {
                pointPosition[pnum].s[i] = i < point->position.dim() ? point->position[i] : 0.0f;
                pointVelocity[pnum].s[i] = i < point->velocity.dim() ? point->velocity[i] : 0.0f;
                pointAcceleration[pnum].s[i] = i < point->acceleration.dim() ? point->acceleration[i] : 0.0f;
            }
            pointMass[pnum] = point->mass;
            
            EGGraphDataPtr sizedata = point->node->getData(EGGraphDataSize::ID);
            EGfloat size = sizedata ? static_cast<EGGraphDataSize&>(*sizedata).getSize() : 1.0f;
            pointSize[pnum] = size;

            EGGraphDataPtr colordata = point->node->getData(EGGraphDataColor::ID);
            EGuint rgba32 = colordata ? static_cast<EGGraphDataColor&>(*colordata).getColor().rgba32() : 0xffffffff; // defaultNodeColorRgba32
            pointColor[pnum] = rgba32;
            
            // set acceleration slot on springs (per spring - to scatter point accelerations)
            EGuint startAccelSlot = accelSlot;
            for (auto si = point->springs.begin(); si != point->springs.end(); si++) {
                SpringCLType *spring = static_cast<SpringCLType*>(*si);
                if (spring->point1 == point) spring->accelSlotPoint1 = accelSlot++;
                if (spring->point2 == point) spring->accelSlotPoint2 = accelSlot++;
            }
            // set acceleration slot ranges on points (per point - to gather point accelerations from springs)
            pointSpringAccelSlotOffset[pnum] = startAccelSlot;
            pointSpringAccelSlotCount[pnum] = accelSlot - startAccelSlot;
        }
        
        // set spring data in opencl buffers
        for (EGint snum = 0; snum < numSprings; snum++) {
            SpringCLType *spring = springs[snum];
            springLength[snum] = spring->length;
            springStiffness[snum] = spring->stiffness;
            springPointIndex1[snum] = spring->point1->index;
            springPointIndex2[snum] = spring->point2->index;
            springAccelSlotPoint1[snum] = spring->accelSlotPoint1;
            springAccelSlotPoint2[snum] = spring->accelSlotPoint2;
            
            EGGraphDataPtr colordata = spring->edges.at(0)->getData(EGGraphDataColor::ID);
            EGuint rgba32 = colordata ? static_cast<EGGraphDataColor&>(*colordata).getColor().rgba32() : 0xffffffff; // defaultNodeColorRgba32
            springColor[snum] = rgba32;
        }
        
        // initialize visible dimensions buffer
        visibleDimensions[0] = 0;
        visibleDimensions[1] = 1;
        visibleDimensions[2] = 2;
        visibleDimensionsBuffer = clctx->createBuffer(CL_MEM_READ_ONLY, sizeof(cl_uint) * 3, NULL);
        clcmdqueue->enqueueWriteBuffer(visibleDimensionsBuffer, true, 0, visibleDimensionsBuffer->getSize(), visibleDimensions);
        
        // sum point buffer sizes
        EGsize pointBufferTotal = 0;
        pointBufferTotal += pointPosition.getSize();
        pointBufferTotal += pointVelocity.getSize();
        pointBufferTotal += pointAcceleration.getSize();
        pointBufferTotal += pointSpringAccelSlotOffset.getSize();
        pointBufferTotal += pointSpringAccelSlotCount.getSize();
        pointBufferTotal += pointMass.getSize();
        pointBufferTotal += pointSize.getSize();
        pointBufferTotal += pointColor.getSize();

        // sum spring buffer sizes
        EGsize springBufferTotal = 0;
        springBufferTotal += springLength.getSize();
        springBufferTotal += springStiffness.getSize();
        springBufferTotal += springColor.getSize();
        springBufferTotal += springPointIndex1.getSize();
        springBufferTotal += springPointIndex2.getSize();
        springBufferTotal += springAccelSlotPoint1.getSize();
        springBufferTotal += springAccelSlotPoint2.getSize();
        
        // copy point and spring buffers to device
        clcmdqueue->copyBuffersToDevice(pointPosition, pointVelocity, pointAcceleration,
                                        pointSpringAccelSlotOffset, pointSpringAccelSlotCount,
                                        pointMass, pointSize, pointColor,
                                        springLength, springStiffness, springColor,
                                        springPointIndex1, springPointIndex2,
                                        springAccelSlotPoint1, springAccelSlotPoint2);
        
        // calculate point work group size
        size_t maxWorkGroupSize = clcmdqueue->getDevice()->getMaxWorkGroupSize();
        size_t maxComputeUnits = clcmdqueue->getDevice()->getMaxComputeUnits();
        
        // start off with maximum work group size
        pointWorkGroupSize = (std::min)((size_t)(numPoints + (maxComputeUnits - 1)) / maxComputeUnits, maxWorkGroupSize);
        pointWorkGroupItems = (numPoints + (pointWorkGroupSize - 1)) / pointWorkGroupSize;
        pointWorkGroupPoints = pointWorkGroupItems * pointWorkGroupSize;
        size_t pointsDelta = pointWorkGroupPoints % numPoints;
        
        // reduce workgroup size until point over run is no more than ~3% and workgroup fits into local memory
        EGsize localMemSize = clcmdqueue->getDevice()->getLocalMemSize();
        while (pointWorkGroupSize > 1 && (pointsDelta > (size_t)(numPoints + (numPoints-1) / 32) || sizeof(VecTypeCL) * pointWorkGroupSize > localMemSize)) {
            pointWorkGroupSize -= 1;
            pointWorkGroupItems = (numPoints + (pointWorkGroupSize - 1)) / pointWorkGroupSize;
            pointWorkGroupPoints = pointWorkGroupItems * pointWorkGroupSize;
            pointsDelta = pointWorkGroupPoints % numPoints;
        }
        
        EGDebug("EGSpringLayoutCL:%s maxWorkGroupSize=%d numPoints=%d pointWorkGroupSize=%d pointWorkGroupItems=%d pointWorkGroupPoints=%d pointsDelta=%d\n",
                __func__, (int)maxWorkGroupSize, (int)numPoints, (int)pointWorkGroupSize, (int)pointWorkGroupItems, (int)pointWorkGroupPoints, (int)pointsDelta);
        
        // acceleration accumulation buffer
        EGsize accelAccumBufferSize;
        if (clcmdqueue->getDevice()->getLocalMemType() == CL_LOCAL)
        {
            // we share the acceleration accumulation buffer between the coulombs law and hookes law kernels
            EGDebug("EGSpringLayoutCL:%s using %u bytes of local memory for accelerated coulombs law\n",
                    __func__, (EGuint)(sizeof(VecTypeCL) * pointWorkGroupSize));
            accelAccumBufferSize = (std::max)(sizeof(VecTypeCL) * numSprings * 2,
                                              sizeof(VecTypeCL) * numPoints * pointWorkGroupItems);
        } else {
            accelAccumBufferSize = sizeof(VecTypeCL) * numSprings * 2;
        }
        accelAccumBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, accelAccumBufferSize, NULL);
        
        // create energy sum buffers
        energyGroupBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(ScalarTypeCL) * pointWorkGroupItems, NULL);
        energyBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(ScalarTypeCL), NULL);
        EGsize energyBufferTotal = sizeof(ScalarTypeCL) * pointWorkGroupItems + sizeof(ScalarTypeCL);
        
        // create bounds buffer
        boundsBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(VecTypeCL) * 2, NULL);
        minBoundsBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(VecTypeCL) * pointWorkGroupSize, NULL);
        maxBoundsBuffer = clctx->createBuffer(CL_MEM_READ_WRITE, sizeof(VecTypeCL) * pointWorkGroupSize, NULL);
        EGsize boundsBufferTotal = sizeof(VecTypeCL) * pointWorkGroupSize * 2;
        
        // bind kernel arguments
        bindKernelArguments();
        
        // print buffer sizes
        EGsize bufferTotal = pointBufferTotal + springBufferTotal + accelAccumBufferSize + energyBufferTotal + boundsBufferTotal;
        EGDebug("EGSpringLayoutCL:%s numPoints=%d numSprings=%d pointBufferTotal=%dK springBufferTotal=%uK accelAccumBufferSize=%uK totalBuffers=%uK\n",
                __func__, numPoints, numSprings,
                (EGuint)(pointBufferTotal / 1024),
                (EGuint)(springBufferTotal / 1024),
                (EGuint)(accelAccumBufferSize / 1024),
                (EGuint)(bufferTotal / 1024));
        
        // create tree arrays
        if (buildTree) {
            tree->createArrays(numPoints, pointWorkGroupSize, pointWorkGroupItems, pointWorkGroupPoints);
        }

        if (barnesHut) {
#if TREE_STRUCT
            // set applyCoulombsLawBarnesHut kernel arguments
            applyCoulombsLawBarnesHutKernel->setArgs(pointPosition, pointAcceleration, pointMass, repulsion, tree->getNodesBuffer());
#endif
        }

        arraysCreated = true;
    }
    
    void bindKernelArguments()
    {
        // create work dimensions
        globalSizePoints =                  EGOpenCLDimension(numPoints);
        globalSizeSprings =                 EGOpenCLDimension(numSprings);
        globalSizePointSpringAccelSlots =   EGOpenCLDimension(numSprings * 2);
        globalSizePointWorkGroupItems =     EGOpenCLDimension(pointWorkGroupItems);
        globalSizePointsWorkGroupItems =    EGOpenCLDimension(numPoints * pointWorkGroupItems);
        globalSizeWorkGroupPointsItems =    EGOpenCLDimension(pointWorkGroupPoints, pointWorkGroupItems);
        localSizeWorkGroupSize =            EGOpenCLDimension(pointWorkGroupSize, 1);
        
        // set clearPointAccel kernel arguments
        clearPointAccelKernel->setArgs(accelAccumBuffer);
        
        if (clcmdqueue->getDevice()->getLocalMemType() == CL_LOCAL)
        {
            // set applyCoulombsLawGroup kernel arguments
            applyCoulombsLawGroupKernel->setArgs(pointPosition, pointAcceleration, accelAccumBuffer, pointMass,
                                                 EGOpenCLLocalMemParam(sizeof(VecTypeCL) * pointWorkGroupSize),
                                                 numPoints, (ScalarTypeCL)repulsion);
            
            // set sumPointGroupAccel kernel arguments
            sumPointGroupAccelKernel->setArgs(pointAcceleration, accelAccumBuffer, numPoints, (cl_int)pointWorkGroupItems);
        }
        else
        {
            // set applyCoulombsLaw kernel arguments
            applyCoulombsLawKernel->setArgs(pointPosition, pointAcceleration, pointMass, numPoints, (ScalarTypeCL)repulsion);
        }

        // set applyHookesLaw kernel arguments
        applyHookesLawKernel->setArgs(pointPosition, pointAcceleration, accelAccumBuffer, pointMass,
                                      springLength, springStiffness, springPointIndex1, springPointIndex2,
                                      springAccelSlotPoint1, springAccelSlotPoint2, numSprings);

        // set sumPointSpringAccel kernel arguments
        sumPointSpringAccelKernel->setArgs(pointAcceleration, accelAccumBuffer,
                                           pointSpringAccelSlotOffset, pointSpringAccelSlotCount, numPoints);

        // set attractToCenter kernel arguments
        attractToCenterKernel->setArgs(pointPosition, pointAcceleration, pointMass, numPoints, (ScalarTypeCL)centerAttraction);

        // set updatePosition kernel arguments
        updatePositionKernel->setArgs(pointPosition, pointAcceleration, pointVelocity, numPoints,
                                      (ScalarTypeCL)timeStep, (ScalarTypeCL)damping, (ScalarTypeCL)maxSpeed);

        // set totalEnergyGroup kernel arguments
        totalEnergyGroupKernel->setArgs(pointVelocity, pointMass, energyGroupBuffer, (cl_int)pointWorkGroupSize, numPoints);
        
        // set totalEnergySum kernel arguments
        totalEnergySumKernel->setArgs(energyGroupBuffer, energyBuffer, (cl_int)pointWorkGroupItems);
        
        // set getBoundsGroup kernel arguments
        getBoundsGroupKernel->setArgs(pointPosition, minBoundsBuffer, maxBoundsBuffer, (cl_int)pointWorkGroupSize, numPoints);
        
        // set getBoundsGather kernel arguments
        getBoundsGatherKernel->setArgs(minBoundsBuffer, maxBoundsBuffer, boundsBuffer, (cl_int)pointWorkGroupItems);
    }
    
    EGGraphPoint<VecType>* getPoint(EGGraphNode *node)
    {
        auto pi = pointmap.find(node);
        return pi != pointmap.end() ? static_cast<EGGraphPoint<VecType>*>((*pi).second) : NULL;
    }
    
    void addNode(EGGraphNode *node)
    {
        // we have one point per node
        auto pi = pointmap.find(node);
        if (pi == pointmap.end()) {
            VecType position = 0.0f;
            if (position.dim() == 1) {
                /* purely random coordinates */
                position[0] = multiplier * ((ScalarType)rand() / (ScalarType)RAND_MAX);
            } else if (position.dim() == 2) {
                /* random coordinates on a circle */
                ScalarType phi = 3.1415927f * 2.0f * ((ScalarType)rand() / (ScalarType)RAND_MAX);
                position[0] = cosf(phi);
                position[1] = sinf(phi);
            } else if (position.dim() == 3) {
                /* random coordinates on a sphere */
                ScalarType phi = 3.1415927f * 2.0f * ((ScalarType)rand() / (ScalarType)RAND_MAX);
                ScalarType theta = 3.1415927f * 2.0f * ((ScalarType)rand() / (ScalarType)RAND_MAX);
                position[0] = multiplier * sinf(theta) * cosf(phi);
                position[1] = multiplier * sinf(theta) * sinf(phi);
                position[2] = multiplier * cosf(theta);
            } else if (position.dim() == 4) {
                /* random coordinates on a 3-sphere */
                ScalarType phi = 3.1415927f * 2.0f * ((ScalarType)rand() / (ScalarType)RAND_MAX);
                ScalarType theta = 3.1415927f * 2.0f * ((ScalarType)rand() / (ScalarType)RAND_MAX);
                ScalarType kappa = 3.1415927f * 2.0f * ((ScalarType)rand() / (ScalarType)RAND_MAX);
                position[0] = multiplier * sinf(theta) * cosf(phi);
                position[1] = multiplier * sinf(theta) * sinf(phi) * cosf(kappa);
                position[2] = multiplier * cosf(theta);
                position[3] = multiplier * sinf(theta) * sinf(phi) * sinf(kappa);
            }
            PointType *point = new PointType(node, position, node->getWeight(), 0.0f, 0.0f);
            pointmap.insert(PointMapValue(node, point));
        }
        deleteArrays();
        running = true;
    }
    
    void addEdge(EGGraphEdge *edge)
    {
        NodePairType nodepair(edge);
        auto si = springmap.find(nodepair);
        if (si == springmap.end()) {
            SpringType *spring = new SpringCLType(nodepair, edge->getLength(), stiffness);
            spring->point1 = pointmap[edge->getSource()];
            spring->point2 = pointmap[edge->getTarget()];
            springmap.insert(SpringMapValue(nodepair, spring));
            spring->addEdge(edge);
            spring->point1->addSpring(spring);
            spring->point2->addSpring(spring);
        } else {
            SpringType *spring = (*si).second;
            EGsize numEdges = spring->numEdges();
            ScalarType r = 1.0f / (numEdges + 1);
            spring->length = spring->length * (1.0f - r) + edge->getLength() * r;
            spring->addEdge(edge);
        }
        deleteArrays();
        running = true;
    }
    
    void removeNode(EGGraphNode *node)
    {
        auto pi = pointmap.find(node);
        if (pi != pointmap.end()) {
            pointmap.erase(pi);
        }
        deleteArrays();
        running = true;
    }
    
    void removeEdge(EGGraphEdge *edge)
    {
        // find the spring for the node pair of this edge
        // (as there can be more than one edge between the same two nodes)
        NodePairType nodepair(edge);
        auto si = springmap.find(nodepair);
        if (si != springmap.end()) {
            SpringType *spring = (*si).second;
            spring->removeEdge(edge);
            // if this is the last edge, then remove the spring
            if (spring->numEdges() == 0) {
                spring->point1->removeSpring(spring);
                spring->point2->removeSpring(spring);
                springmap.erase(si);
            }
        }
        deleteArrays();
        running = true;
    }
    
    void updateNode(EGGraphNode *node)
    {
        auto pi = pointmap.find(node);
        if (pi != pointmap.end()) {
            PointType *point = (*pi).second;
            // update weight
            point->mass = node->getWeight();
        }
        deleteArrays();
        running = true;
    }
    
    void updateEdge(EGGraphEdge *edge)
    {
        NodePairType nodepair(edge);
        auto si = springmap.find(nodepair);
        if (si != springmap.end()) {
            SpringType *spring = (*si).second;
            assert(spring->numEdges() > 0);
            // update length (maximum of all edges common to this spring)
            spring->length = 0;
            EGGraphEdgeList *edges = spring->getEdges();
            for (auto ei = edges->begin(); ei != edges->end(); ei++) {
                EGGraphEdge *edge = *ei;
                spring->length = (std::max)(spring->length, (ScalarType)edge->getLength());
            }
        }
        deleteArrays();
        running = true;
    }
    
    EGint getNumIterations() { return iterCount; }

    void step()
    {
        if (!running) return;
        
        // create OpenCL buffers for points and springs
        createArrays();
        
        //
        // Build tree
        //

        if (buildTree) {
            tree->build(pointPosition, pointMass);
        }
        
        //
        // Apply Coulombs Law
        //
        
        if (barnesHut) {
            // exec kernel applyCoulombsLaw<numPoints>();
            clcmdqueue->enqueueNDRangeKernel(applyCoulombsLawBarnesHutKernel, globalSizePoints);
        } else {
            if (clcmdqueue->getDevice()->getLocalMemType() == CL_LOCAL)
            {
                // exec kernel clearPointAccel<numPoints*pointWorkGroupItems>
                clcmdqueue->enqueueNDRangeKernel(clearPointAccelKernel, globalSizePointsWorkGroupItems);
                
                // exec kernel applyCoulombsLawGroup<pointWorkGroupPoints,pointWorkGroupItems><pointWorkGroupSize,1>();
                clcmdqueue->enqueueNDRangeKernel(applyCoulombsLawGroupKernel, globalSizeWorkGroupPointsItems, localSizeWorkGroupSize);
                
                // exec kernel sumPointGroupAccel<numPoints>();
                clcmdqueue->enqueueNDRangeKernel(sumPointGroupAccelKernel, globalSizePoints);
            }
            else
            {
                // exec kernel applyCoulombsLaw<numPoints>();
                clcmdqueue->enqueueNDRangeKernel(applyCoulombsLawKernel, globalSizePoints);
            }
        }
        
        //
        // Apply Hookes Law
        //
        
        // exec kernel clearPointAccel<numSprings*2>();
        clcmdqueue->enqueueNDRangeKernel(clearPointAccelKernel, globalSizePointSpringAccelSlots);
        
        // exec kernel applyHookesLaw<numSprings>();
        clcmdqueue->enqueueNDRangeKernel(applyHookesLawKernel, globalSizeSprings);
        
        // exec kernel sumPointSpringAccel<numPoints>();
        clcmdqueue->enqueueNDRangeKernel(sumPointSpringAccelKernel, globalSizePoints);
        
        //
        // Attract to center
        //
        
        // exec kernel attractToCenter<numPoints>();
        if (centerAttraction > 0) {
            clcmdqueue->enqueueNDRangeKernel(attractToCenterKernel, globalSizePoints);
        }
        
        // Update velocities and positions
        
        // exec kernel updatePosition<numPoints>();
        clcmdqueue->enqueueNDRangeKernel(updatePositionKernel, globalSizePoints);
        
        //
        // Compute total energy
        //
        
        // exec kernel totalEnergyGroup<pointWorkGroupItems>();
        clcmdqueue->enqueueNDRangeKernel(totalEnergyGroupKernel, globalSizePointWorkGroupItems);
        
        // exec kernek task totalEnergySum();
        clcmdqueue->enqueueTask(totalEnergySumKernel);
        
        // read energy sum
        clcmdqueue->enqueueReadBuffer(energyBuffer, true, 0, sizeof(ScalarTypeCL), &energy);

#if TREE_DEBUG
        if (buildTree) {
            // testing dump state
            tree->dumptreeState();

            // we are just testing so stop
            energy = 0;
        }
#endif
        
        // update iteration count
        iterCount++;
        
        // stop simulation when energy of the system goes below a threshold
        if (debug) {
            EGDebug("EGSpringLayoutCL:%s totalEnergy=%f\n", __func__, energy);
        }
        if (energy < maxEnergy) {
            EGDebug("EGSpringLayoutCL:%s stopped: totalEnergy=%f\n", __func__, energy);
            running = false;
        }
    }
    
    EGbool isRunning()
    {
        return running;
    }
    
    EGfloat totalEnergy()
    {
        return (EGfloat)energy;
    }
    
    void stop()
    {
        running = false;
    }
};

#if TREE_ARRAY
#define EGSpringLayoutCLPointTreeType EGPointRegionTreeArrayCL
#endif

#if TREE_STRUCT
#define EGSpringLayoutCLPointTreeType EGPointRegionTreeStructCL
#endif

#if TREE_MONOLITHIC
#define EGSpringLayoutCLPointTreeType EGPointRegionTreeMonolithicCL
#endif

/* EGSpringLayout2DCL */

template <typename VecType,
          typename NodePairType = EGGraphNodePairOrdered,
          typename ScalarType = typename VecType::value_type,
          typename PointTreeType = EGSpringLayoutCLPointTreeType<VecType,ScalarType,4,4>>
class EGSpringLayout2DCL : public EGSpringLayoutCL<VecType,NodePairType,ScalarType,false,false,PointTreeType,cl_float4,cl_float>
{
public:
    EGSpringLayout2DCL(EGSpringLayoutSettings &s) : EGSpringLayoutCL<VecType,NodePairType,ScalarType,false,false,PointTreeType,cl_float4,cl_float>(s) {}
};

/* EGSpringLayout3DCL */

template <typename VecType,
          typename NodePairType = EGGraphNodePairOrdered,
          typename ScalarType = typename VecType::value_type,
          typename PointTreeType = EGSpringLayoutCLPointTreeType<VecType,ScalarType,8,8>>
class EGSpringLayout3DCL : public EGSpringLayoutCL<VecType,NodePairType,ScalarType,false,false,PointTreeType,cl_float4,cl_float>
{
public:
    EGSpringLayout3DCL(EGSpringLayoutSettings &s) : EGSpringLayoutCL<VecType,NodePairType,ScalarType,false,false,PointTreeType,cl_float4,cl_float>(s) {}
};


/* EGSpringLayoutBarnesHut2DCL */

template <typename VecType,
          typename NodePairType = EGGraphNodePairOrdered,
          typename ScalarType = typename VecType::value_type,
          typename PointTreeType = EGSpringLayoutCLPointTreeType<VecType,ScalarType,4,4>>
class EGSpringLayoutBarnesHut2DCL : public EGSpringLayoutCL<VecType,NodePairType,ScalarType,true,true,PointTreeType,cl_float4,cl_float>
{
public:
    EGSpringLayoutBarnesHut2DCL(EGSpringLayoutSettings &s) : EGSpringLayoutCL<VecType,NodePairType,ScalarType,true,true,PointTreeType,cl_float4,cl_float>(s) {}
};

/* EGSpringLayoutBarnesHut3DCL */

template <typename VecType,
          typename NodePairType = EGGraphNodePairOrdered,
          typename ScalarType = typename VecType::value_type,
          typename PointTreeType = EGSpringLayoutCLPointTreeType<VecType,ScalarType,8,8>>
class EGSpringLayoutBarnesHut3DCL : public EGSpringLayoutCL<VecType,NodePairType,ScalarType,true,true,PointTreeType,cl_float4,cl_float>
{
public:
    EGSpringLayoutBarnesHut3DCL(EGSpringLayoutSettings &s) : EGSpringLayoutCL<VecType,NodePairType,ScalarType,true,true,PointTreeType,cl_float4,cl_float>(s) {}
};

#endif
