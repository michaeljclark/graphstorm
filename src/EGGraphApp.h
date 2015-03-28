/*
 *  EGGraphApp.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGGraphApp_H
#define EGGraphApp_H

#define USE_OPENCL_DEVICE "gpu"
#define USE_OPENCL_SHARING 1
#define USE_OPENCL_PROFILING 0

#include "EG.h"
#include "EGGL.h"
#include "EGMath.h"
#include "EGColor.h"
#include "EGText.h"
#include "EGImage.h"
#include "EGCamera.h"
#include "EGViewport.h"
#include "EGTrackBall.h"
#include "EGLight.h"
#include "EGTime.h"
#include "EGResource.h"
#include "EGRenderApi.h"
#include "EGRenderApiES2.h"
#include "EGIndexArray.h"
#include "EGVertexArray.h"
#include "EGRenderBatch.h"
#include "EGRenderBatchES2.h"
#include "EGCombination.h"
#include "EGPointRegionOctree.h"
#include "EGPointRegionQuadtree.h"
#include "EGGraph.h"
#include "EGGraphData.h"
#include "EGGraphTest.h"
#include "EGGraphModifier.h"
#include "EGGraphLayout.h"
#include "EGGraphRendererParams.h"
#include "EGGraphEdgeRenderer.h"
#include "EGGraphNodeRenderer.h"
#include "EGGraphEdgeLabelRenderer.h"
#include "EGGraphNodeLabelRenderer.h"
#include "EGBoundsRenderer.h"
#include "EGTreeRenderer.h"
#include "EGSpringLayout.h"

#if USE_OPENCL
#include "EGOpenCL.h"
#include "EGSpringLayoutCL.h"
#include "EGGraphEdgeRendererES2CL.h"
#include "EGGraphNodeRendererES2CL.h"
#endif

#include "EGGraphLayoutFactory.h"
#include "EGGraphAppUI.h"


class EGGraphApp;
typedef std::shared_ptr<EGGraphApp> EGGraphAppPtr;


/* EGGraphAppInteractMode */

enum EGGraphAppInteractMode {
    EGGraphAppInteractNone,
    EGGraphAppInteractDolly,
    EGGraphAppInteractTrackball
};

enum EGGraphAppMouseButton {
    EGGraphAppMouseButtonNone = 0x0,
    EGGraphAppMouseButtonLeft = 0x1,
    EGGraphAppMouseButtonRight = 0x2,
};

/* EGGraphApp */

class EGGraphApp
{
public:
    static const EGfloat MAX_ZOOM_LEVEL;
    static const EGfloat MIN_ZOOM_LEVEL;
    
    EGRenderApi *gl;
    EGCameraPtr camera;
    EGViewportPtr viewport;
    EGTrackBallPtr trackball;
    EGfloat shapeSize;
    EGint dollyStartPoint[2];
    EGint worldSize[2];
    EGfloat worldRotation[4];
    EGfloat worldRotationMatrix[16];
    EGbool recalcProjection;
    EGGraphAppInteractMode interactMode;
    EGTimer fpsTimer;
    EGfloat lastfps;
    EGfloat lastips;
    EGint fpsNumFrames;
    EGint lastIters;
    EGbool showInfo;
    EGbool showTree;
    EGTextPtr fpsText;
    EGTextPtr iterText;
    EGTextPtr energyText;
    EGGraphPtr graph;
    EGGraphLayoutImplPtr layoutImpl;
    
#if USE_OPENCL
    EGOpenCLPtr cl;
    EGOpenCLDeviceList cldevices;
    EGOpenCLDevicePtr cldevice;
    EGOpenCLContextPtr clctx;
    EGOpenCLCommandQueuePtr clcmdqueue;
#endif
    
    EGGraphAppUIPtr graphappui;
    
    EGGraphApp();
    virtual ~EGGraphApp();
    
    void init();
    void initCL();
    void switchDeviceCL(int deviceIndex);
    
#if USE_OPENCL
    void openCLTest();
#endif
    
    EGGraphLayoutImplPtr getLayoutImpl() { return layoutImpl; }
    void setLayoutImpl(EGGraphLayoutImplPtr layoutImpl);
    
    void clearGraph();
    void setupGraph(EGSpringLayoutSettings &settings, EGGraphTest &graphtest, EGGraphLayoutFactory &factory, EGGraphModifierList modifierList = EGGraphModifierList());

    void cameraReset();
    void setupLighting();
    void calcWorldProjection();
    void applyTrackballRotation();
    void draw3D();
    void draw2D();
    void draw();
    void reshape(EGint width, EGint height);
    void idle();
    void mousePress(EGint x, EGint y, EGenum buttons);
    void mouseRelease(EGint x, EGint y, EGenum buttons);
    void mouseMove(EGint x, EGint y, EGenum buttons);
};

#endif
