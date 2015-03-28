/*
 *  EWWidgetRenderer.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGGL.h"
#include "EGColor.h"
#include "EGText.h"
#include "EGImage.h"
#include "EGIndexArray.h"
#include "EGVertexArray.h"
#include "EGRenderBatch.h"

#include "EWWidget.h"
#include "EWWidgetRenderer.h"


/* EWWidgetRenderer */

EWWidgetRenderer::EWWidgetRenderer(EWWidget *widget) : widget(widget) {}

EWWidgetRenderer::~EWWidgetRenderer() {}
