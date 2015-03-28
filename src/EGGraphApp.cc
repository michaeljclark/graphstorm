/*
 *  EGGraphApp.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EGGraphApp.h"


/* EGGraphApp */

static const char class_name[] = "EGGraphApp";

const EGfloat EGGraphApp::MAX_ZOOM_LEVEL = 0.001f;
const EGfloat EGGraphApp::MIN_ZOOM_LEVEL = 1000.0f;

EGGraphApp::EGGraphApp()
  : gl(NULL),
    camera(new EGCamera()),
    viewport(new EGViewport()),
    trackball(new EGTrackBall()),
    shapeSize(3.0f),
    recalcProjection(true),
    interactMode(EGGraphAppInteractNone),
    fpsTimer(),
    lastfps(0),
    lastips(0),
    fpsNumFrames(0),
    lastIters(0),
    showInfo(true),
    showTree(true)
{}

EGGraphApp::~EGGraphApp()
{
}

void EGGraphApp::init()
{
    EGDebug("%s:%s\n", class_name, __func__);
    
    // initialize render api
    const EGint glesVersion = 2;
    EGRenderApi::initRenderApi(glesVersion);
    gl = &EGRenderApi::currentApiImpl();
        
    // setup info text
    fpsText = EGText::create(EGText::defaultFontFace, 14, EGFontStyleNormal, EGTextHAlignLeft | EGTextVAlignBase);
    iterText = EGText::create(EGText::defaultFontFace, 14, EGFontStyleNormal, EGTextHAlignLeft | EGTextVAlignBase);
    energyText = EGText::create(EGText::defaultFontFace, 14, EGFontStyleNormal, EGTextHAlignLeft | EGTextVAlignBase);
        
    cameraReset();
}

void EGGraphApp::initCL()
{
#if USE_OPENCL
    cl = EGOpenCLPtr(new EGOpenCL());
    EGOpenCLDeviceList all_cldevices = cl->getDevices(USE_OPENCL_DEVICE);
    clctx = cl->createContext(all_cldevices, USE_OPENCL_SHARING);
    cldevices = clctx->getDevices();
    cldevice = cldevices.at(0);
    clcmdqueue = clctx->createCommandQueue(cldevice, USE_OPENCL_PROFILING ? CL_QUEUE_PROFILING_ENABLE : 0);
    cldevice->print();
#if USE_OPENCL_PROFILING
    clcmdqueue->setPrintProfilingInfo(true);
#endif
    openCLTest();
#endif
}

void EGGraphApp::switchDeviceCL(int deviceIndex)
{
#if USE_OPENCL
    cldevices = EGOpenCLDeviceList();
    cldevices.push_back(cl->getDevices()[deviceIndex]);
    clctx = cl->createContext(cldevices, USE_OPENCL_SHARING);
    cldevice = clctx->getDevices().at(deviceIndex);
    clcmdqueue = clctx->createCommandQueue(cldevice, USE_OPENCL_PROFILING ? CL_QUEUE_PROFILING_ENABLE : 0);
    cldevice->print();
#if USE_OPENCL_PROFILING
    clcmdqueue->setPrintProfilingInfo(true);
#endif
#endif
}


#if USE_OPENCL
void EGGraphApp::openCLTest()
{
    int a[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    int b[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    int c[8];
    
    EGOpenCLProgramPtr cltestprog = clctx->createProgram(EGResource::getResource("Resources/cl.bundle/test.cl"));
    
    EGOpenCLBufferPtr abuffer = clctx->createBuffer(CL_MEM_READ_ONLY, sizeof(a), NULL);
    EGOpenCLBufferPtr bbuffer = clctx->createBuffer(CL_MEM_READ_ONLY, sizeof(b), NULL);
    EGOpenCLBufferPtr cbuffer = clctx->createBuffer(CL_MEM_WRITE_ONLY, sizeof(c), NULL);
    
    EGOpenCLKernelPtr addkernel = cltestprog->getKernel("add");
    addkernel->setArg(0, abuffer);
    addkernel->setArg(1, bbuffer);
    addkernel->setArg(2, cbuffer);
    
    clcmdqueue->enqueueWriteBuffer(abuffer, true, 0, sizeof(a), a);
    clcmdqueue->enqueueWriteBuffer(bbuffer, true, 0, sizeof(b), b);
    clcmdqueue->enqueueNDRangeKernel(addkernel, EGOpenCLDimension(8));
    clcmdqueue->enqueueReadBuffer(cbuffer, true, 0, sizeof(c), c)->wait();
    
    EGbool error = false;
    for (int i = 0; i < 8; i++) {
        error |= (a[i] + b[i] != c[i]);
    }
    EGDebug("%s:%s Add Test: %s\n", class_name, __func__, error ? "Failed" : "Success");
}
#endif

void EGGraphApp::setLayoutImpl(EGGraphLayoutImplPtr layoutImpl)
{
    this->layoutImpl = layoutImpl;
    if (graph) {
        graph->setLayout(layoutImpl->getLayout());
    }
}

void EGGraphApp::clearGraph()
{
    graph = EGGraphPtr(new EGGraph());
    layoutImpl = EGGraphLayoutImplPtr();
}

void EGGraphApp::setupGraph(EGSpringLayoutSettings &settings, EGGraphTest &graphtest, EGGraphLayoutFactory &factory, EGGraphModifierList modifierList)
{
    // create graph
    graph = EGGraphPtr(new EGGraph());
    
    // fill graph with test data
    graphtest.populate(graph);
    
    // apply graph modifiers
    EGGraphModifier::modify(graph, modifierList);
    
    EGDebug("%s:%s nodes=%d edges=%d\n", class_name, __func__, (int)graph->getNodeList()->size(), (int)graph->getEdgeList()->size());
    
    // non-deterministic
    srand((unsigned int)time(NULL));
    
    // deterministic
    //srand(45364);

    // clear current layout
    layoutImpl = EGGraphLayoutImplPtr();

#if USE_OPENCL
    // create spring layout
    layoutImpl = factory.create(graph, settings, clctx->glSharingIsEnabled());
    
    // initialize OpenCL
    layoutImpl->initOpenCL(clctx, clcmdqueue, clctx->glSharingIsEnabled());
#else
    // create spring layout
    layoutImpl = factory.create(graph, settings, false);
#endif
    
    // add spring layout to the graph
    graph->setLayout(layoutImpl->getLayout());
}

void EGGraphApp::cameraReset()
{
    camera->aperture = 40;
    camera->focalLength = shapeSize;
    camera->rotPoint[0] = 0.0;
    camera->rotPoint[1] = 0.0;
    camera->rotPoint[2] = 0.0;
    
    camera->viewPos[0] = 0.0;
    camera->viewPos[1] = 0.0;
    camera->viewPos[2] = (shapeSize*1.5f);
    camera->viewDir[0] = 0;
    camera->viewDir[1] = 0;
    camera->viewDir[2] = -1;
    
    camera->viewUp[0] = 0;
    camera->viewUp[1] = 1;
    camera->viewUp[2] = 0;
    
    worldSize[0] = 1024;
    worldSize[1] = 1024;
    
    worldRotation[0] = 0;
    worldRotation[1] = 0;
    worldRotation[2] = 0;
    worldRotation[3] = 0;
}

void EGGraphApp::setupLighting()
{
    // get the model view matrix
    //EGfloat modelView[16];
    //gl->getMatrix(EGRenderMatrixModelView, modelView);
    
    EGLight light0(0);
    light0.setPosition(7.0f, -7.0f, 12.0f ,0.0f);
    light0.setAmbient(0.2f, 0.2f, 0.2f, 1.0f);
    light0.setupGLState();
    
    // restore the model view matrix
    //gl->loadMatrix(EGRenderMatrixModelView, modelView);
}

void EGGraphApp::calcWorldProjection()
{
    // Build rotation matrix
    EGfloat rot1[16], rot2[16];
    EGMath::makeRotateMat4f(rot1,
                            worldRotation[0], worldRotation[1],
                            worldRotation[2], worldRotation[3]);
    EGMath::makeRotateMat4f(rot2,
                            trackball->gTrackBallRotation[0], trackball->gTrackBallRotation[1],
                            trackball->gTrackBallRotation[2], trackball->gTrackBallRotation[3]);
    EGMath::multMat4fMat4f(worldRotationMatrix, rot1, rot2);
    
    recalcProjection = false;
}

void EGGraphApp::applyTrackballRotation()
{
    if (trackball->gTrackBallRotation[0] != 0.0) {
        trackball->addToRotationTrackball(trackball->gTrackBallRotation, worldRotation);
    }
    trackball->reset();
}

void EGGraphApp::draw3D()
{
    if (!layoutImpl) return;
    
    // calculate projection
    calcWorldProjection();
    
    if (graphappui) {
        // transform to the left
        EGfloat transform[16] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            -0.4, 0, 0, 1
        };
        gl->multMatrix(EGRenderMatrixModelView, transform);
    }
    
    // Apply rotation matrix
    gl->multMatrix(EGRenderMatrixModelView, worldRotationMatrix);
    gl->updateUniforms();
    
    // Get model view project matrix
    gl->getMatrix(EGRenderMatrixModelViewProjection, viewport->modelViewProjectionMatrix);

    // Update layout
    layoutImpl->update(viewport);

    // layout 3D draw pass
    layoutImpl->draw3D(viewport);
}

void EGGraphApp::draw2D()
{    
    EGfloat m[16];
    
    EGMath::identityMat4f(m);
    gl->loadMatrix(EGRenderMatrixProjection, m);
    
    EGMath::scaleMat4f(m, viewport->scaleFactor * 2.0f / viewport->actualWidth, viewport->scaleFactor * -2.0f / viewport->actualHeight, viewport->scaleFactor);
    EGMath::rotateMat4f(m, viewport->screenRotation, 0, 0, 1);
    EGMath::translateMat4f(m, -viewport->screenWidth * 0.5f, -viewport->screenHeight * 0.5f, 0.0f);
    gl->loadMatrix(EGRenderMatrixModelView, m);
    gl->updateUniforms();
    
    // layout 2D draw pass
    layoutImpl->draw2D(viewport);

    // draw text
    if (showInfo) {
        fpsText->setPosition(10.0f, viewport->screenHeight - 50.0f);
        fpsText->draw();
        iterText->setPosition(10.0f, viewport->screenHeight - 30.0f);
        iterText->draw();
        energyText->setPosition(10.0f, viewport->screenHeight - 10.0f);
        energyText->draw();
    }
    
    if (graphappui) {
        graphappui->draw();
    }
}

void EGGraphApp::draw()
{
    glViewport(0, 0, viewport->actualWidth, viewport->actualHeight);
    
    // setup GL state
    camera->setupRenderState(viewport);
        
    // clear the surface
    glClearColor(0.2f, 0.2f, 0.2f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Calculate world projection matrix if view has changed
    if (recalcProjection) {
        calcWorldProjection();
    }
    
    // setup lighting
    setupLighting();
    
    // draw
    draw3D();
    
    // update info text
    fpsTimer.stop();
    fpsNumFrames++;
    long long usec = fpsTimer.getUSec();
    if (usec > 100000) {
        EGfloat energy = layoutImpl ? layoutImpl->totalEnergy() : 0.0f;
        EGint iters = layoutImpl ? layoutImpl->getNumIterations() : 0;
        lastfps = 1000000.0f / usec * fpsNumFrames;
        lastips = 1000000.0f / usec * (iters - lastIters);
        fpsTimer.start();
        fpsNumFrames = 0;
        lastIters = iters;
        
        // update info text
        std::stringstream fps_ss, ips_ss, energy_ss;
        fps_ss << "FPS: " << std::setprecision(3) << lastfps;
        ips_ss << "IPS: " << std::setprecision(5) << lastips;
        energy_ss << "Energy: " << std::setprecision(5) << energy;
        fpsText->setText(fps_ss.str());
        iterText->setText(ips_ss.str());
        energyText->setText(energy_ss.str());
    }
    
    // draw 2d
    draw2D();
}

void EGGraphApp::reshape(EGint width, EGint height)
{
    viewport->screenWidth = width;
    viewport->screenHeight = height;
    viewport->scaleFactor = 1.0f;
    viewport->actualWidth = width;
    viewport->actualHeight = height;
    viewport->viewportX = 0;
    viewport->viewportY = 0;
    viewport->viewportWidth = viewport->actualWidth;
    viewport->viewportHeight = viewport->actualHeight;
    viewport->screenViewportX = 0;
    viewport->screenViewportY = 0;
    viewport->screenViewportWidth = viewport->screenWidth;
    viewport->screenViewportHeight = viewport->screenHeight;
    
    if (graphappui) {
        graphappui->context->reshape();
        graphappui->window->setPosition(EGPoint(width - graphappui->window->calcPreferredSize().width - 8, 8));
        graphappui->setNeedsLayout();
    }

    EGDebug("%s:%s actualWidth=%d actualHeight=%d screenRotation=%f screenWidth=%d screenHeight=%d scaleFactor=%f\n", class_name, __func__,
            viewport->actualWidth, viewport->actualHeight, viewport->screenRotation, viewport->screenWidth, viewport->screenHeight, viewport->scaleFactor);
}

void EGGraphApp::mousePress(EGint x, EGint y, EGenum buttons)
{
    if (buttons & EGGraphAppMouseButtonLeft) {
        if (layoutImpl) {
            EGGraphNode *node = layoutImpl->findNodeFromScreenPosition(viewport, x, y);
            if (node) {
                EGDebug("%s:%s clicked node: %s\n", class_name, __func__, node->getName().c_str());
            }
        }
        y = viewport->actualHeight - y;
        x = (EGint)((x - viewport->viewportX - viewport->viewportWidth * 0.5f) + worldSize[0] * 0.5f);
        y = (EGint)((y - viewport->viewportY - viewport->viewportHeight * 0.5f) + worldSize[1] * 0.5f);
        applyTrackballRotation();
        trackball->startTrackball(x, y, 0, 0, worldSize[0], worldSize[1]);
        interactMode = EGGraphAppInteractTrackball;
    } else if (buttons & EGGraphAppMouseButtonRight) {
        dollyStartPoint[0] = x;
        dollyStartPoint[1] = y;
        applyTrackballRotation();
        interactMode = EGGraphAppInteractDolly;
    }
}

void EGGraphApp::mouseRelease(EGint x, EGint y, EGenum buttons)
{
    switch (interactMode) {
        case EGGraphAppInteractTrackball:
        {
            y = viewport->actualHeight - y;
            x = (EGint)((x - viewport->viewportX - viewport->viewportWidth * 0.5f) + worldSize[0] * 0.5f);
            y = (EGint)((y - viewport->viewportY - viewport->viewportHeight * 0.5f) + worldSize[1] * 0.5f);
            trackball->rollToTrackball(x, y, trackball->gTrackBallRotation);
            applyTrackballRotation();
            break;
        }
        default: break;
    }
    interactMode = EGGraphAppInteractNone;
}

void EGGraphApp::mouseMove(EGint x, EGint y, EGenum buttons)
{
    switch (interactMode) {
        case EGGraphAppInteractDolly:
        {
            EGfloat dolly = (dollyStartPoint[1] - y) * -camera->viewPos[2] / 200.0f;
            EGfloat eyeRelative = camera->eyeSep / camera->focalLength;
            camera->focalLength += camera->focalLength / camera->viewPos[2] * dolly;
            if (camera->focalLength < 1.0) {
                camera->focalLength = 1.0;
            }
            camera->eyeSep = camera->focalLength * eyeRelative;
            camera->viewPos[2] += dolly;
            if (camera->viewPos[2] < MAX_ZOOM_LEVEL) {
                camera->viewPos[2] = MAX_ZOOM_LEVEL;
            }
            if (camera->viewPos[2] > MIN_ZOOM_LEVEL) {
                camera->viewPos[2] = MIN_ZOOM_LEVEL;
            }
            dollyStartPoint[0] = x;
            dollyStartPoint[1] = y;
            recalcProjection = true;
            break;
        }
        case EGGraphAppInteractTrackball:
        {
            y = viewport->actualHeight - y;
            x = (EGint)((x - viewport->viewportX - viewport->viewportWidth * 0.5f) + worldSize[0] * 0.5f);
            y = (EGint)((y - viewport->viewportY - viewport->viewportHeight * 0.5f) + worldSize[1] * 0.5f);
            trackball->rollToTrackball(x, y, trackball->gTrackBallRotation);
            recalcProjection = true;
            break;
        }
        default: break;
    }
}
