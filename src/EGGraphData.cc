/*
 *  EGGraphData.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGColor.h"

#include "EGGraph.h"
#include "EGGraphData.h"


/* EGGraphDataLabel */

const EGuint EGGraphDataLabel::ID = 1;

EGGraphDataPair EGGraphDataLabel::create(EGstring label) { return EGGraphDataPair(ID, EGGraphDataPtr(new EGGraphDataLabel(label))); }


/* EGGraphDataColor */

const EGuint EGGraphDataColor::ID = 2;

EGGraphDataPair EGGraphDataColor::create(EGColor color) { return EGGraphDataPair(ID, EGGraphDataPtr(new EGGraphDataColor(color))); }
EGGraphDataPair EGGraphDataColor::create(EGstring colorstr) { return EGGraphDataPair(ID, EGGraphDataPtr(new EGGraphDataColor(EGColor::fromHex(colorstr)))); }


/* EGGraphDataSize */

const EGuint EGGraphDataSize::ID = 3;

EGGraphDataPair EGGraphDataSize::create(EGfloat size) { return EGGraphDataPair(ID, EGGraphDataPtr(new EGGraphDataSize(size))); }
