// See LICENSE for license details.

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
