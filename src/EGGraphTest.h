/*
 *  EGGraphTest.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGGraphTest_H
#define EGGraphTest_H


/* EGGraphTest */

struct EGGraphTest
{
    std::string name;
    
    virtual ~EGGraphTest() {}
    
    virtual void populate(EGGraphPtr graph) = 0;
};


/* EGGraphTestAllPairs */

struct EGGraphTestAllPairs : public EGGraphTest
{
    EGint numNodes;
    
    EGGraphTestAllPairs(EGint numNodes);
    
    void populate(EGGraphPtr graph);
};


/* EGGraphTestRandomPairs */

struct EGGraphTestRandomPairs : public EGGraphTest
{
    EGint numNodes;
    EGfloat p;
    
    EGGraphTestRandomPairs(EGint numNodes, EGfloat p);
    
    void populate(EGGraphPtr graph);
};


/* EGGraphTestRandomScaleFree */

struct EGGraphTestRandomScaleFree : public EGGraphTest
{
    EGint numNodes;
    
    EGGraphTestRandomScaleFree(EGint numNodes);
    
    void populate(EGGraphPtr graph);
};


/* EGGraphTestLoopStar */

struct EGGraphTestLoopStar : public EGGraphTest
{
    EGint numSuperNodes;
    EGint numSubNodes;
    EGint numSubSubNodes;
    
    EGGraphTestLoopStar(EGint numSuperNodes, EGint numSubNodes, EGint numSubSubNodes);
    
    void populate(EGGraphPtr graph);
};


/* EGGraphTestGrid */

struct EGGraphTestGrid : public EGGraphTest
{
    EGint X;
    EGint Y;
    
    EGGraphTestGrid(EGint X, EGint Y);
    
    void populate(EGGraphPtr graph);
};


/* EGGraphTestTube */

struct EGGraphTestTube : public EGGraphTest
{
    EGint X;
    EGint Y;
    
    EGGraphTestTube(EGint X, EGint Y);
    
    void populate(EGGraphPtr graph);
};


/* EGGraphTestTorus */

struct EGGraphTestTorus : public EGGraphTest
{
    EGint X;
    EGint Y;
    
    EGGraphTestTorus(EGint X, EGint Y);
    
    void populate(EGGraphPtr graph);
};


/* EGGraphTestResource */

struct EGGraphTestResource : public EGGraphTest
{
    EGResourcePtr rsrc;
    
    EGGraphTestResource(EGResourcePtr rsrc);
    
    void populate(EGGraphPtr graph);
};


/* EGGraphTestImpl */

struct EGGraphTestImpl
{
    static EGGraphTest* testgraphs[];
    
    static EGGraphTestAllPairs        allPairs16;
    static EGGraphTestAllPairs        allPairs64;
    static EGGraphTestAllPairs        allPairs256;
    static EGGraphTestRandomPairs     randomPairs64p0125;
    static EGGraphTestRandomPairs     randomPairs256p0125;
    static EGGraphTestRandomPairs     randomPairs1024p0125;
    static EGGraphTestRandomScaleFree randomScaleFree64;
    static EGGraphTestRandomScaleFree randomScaleFree256;
    static EGGraphTestRandomScaleFree randomScaleFree1024;
    static EGGraphTestLoopStar        loopStar1x16x0;
    static EGGraphTestLoopStar        loopStar1x64x0;
    static EGGraphTestLoopStar        loopStar1x256x16;
    static EGGraphTestLoopStar        loopStar2x2x2;
    static EGGraphTestLoopStar        loopStar4x4x4;
    static EGGraphTestLoopStar        loopStar8x8x8;
    static EGGraphTestLoopStar        loopStar16x16x16;
    static EGGraphTestGrid            grid16x16;
    static EGGraphTestGrid            grid32x32;
    static EGGraphTestGrid            grid64x64;
    static EGGraphTestGrid            grid128x128;
    static EGGraphTestTube            tube16x32;
    static EGGraphTestTube            tube16x64;
    static EGGraphTestTube            tube16x128;
    static EGGraphTestTube            tube32x64;
    static EGGraphTestTube            tube32x128;
    static EGGraphTestTube            tube32x256;
    static EGGraphTestTube            tube64x128;
    static EGGraphTestTube            tube64x256;
    static EGGraphTestTube            tube64x512;
    static EGGraphTestTorus           torus16x32;
    static EGGraphTestTorus           torus16x64;
    static EGGraphTestTorus           torus16x128;
    static EGGraphTestTorus           torus32x64;
    static EGGraphTestTorus           torus32x128;
    static EGGraphTestTorus           torus32x256;
    static EGGraphTestTorus           torus64x128;
    static EGGraphTestTorus           torus64x256;
    static EGGraphTestTorus           torus64x512;
    static EGGraphTestResource        graphResource;
};

#endif
