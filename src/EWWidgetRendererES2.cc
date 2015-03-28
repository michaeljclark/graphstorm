/*
 *  EWWidgetRendererES2.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGGL.h"
#include "EGColor.h"
#include "EGText.h"
#include "EGImage.h"
#include "EGViewport.h"
#include "EGResource.h"
#include "EGIndexArray.h"
#include "EGVertexArray.h"
#include "EGRenderApi.h"
#include "EGRenderApiES2.h"
#include "EGRenderBatch.h"
#include "EGRenderBatchES2.h"
#include "EGRenderBatchText.h"

#include "EWWidget.h"
#include "EWContainer.h"
#include "EWContext.h"
#include "EWWidgetRenderer.h"
#include "EWWidgetRendererES2.h"


/* EWWidgetRendererES2 */

EWWidgetRendererES2::EWWidgetRendererES2(EWWidget *widget) : EWWidgetRenderer(widget), scissorEnabled(false)
{
    // link shader attributes
    EGRenderApiES2 &gl = EGRenderApiES2::apiImpl();
    
    tex_program = gl.getRenderProgram(EGResource::getResource("Resources/gles2.bundle/Shaders/NoLighting.vsh"),
                                        EGResource::getResource("Resources/gles2.bundle/Shaders/NoLighting.fsh"));
    tex_u_texture0 = tex_program->getUniform("u_texture0");
    tex_a_position = tex_program->getAttribute("a_position");
    tex_a_color = tex_program->getAttribute("a_color");
    tex_a_texcoord0 = tex_program->getAttribute("a_texcoord0");
    
    notex_program = gl.getRenderProgram(EGResource::getResource("Resources/gles2.bundle/Shaders/NoLightingNoTexture.vsh"),
                                        EGResource::getResource("Resources/gles2.bundle/Shaders/NoLightingNoTexture.fsh"));
    notex_a_position = notex_program->getAttribute("a_position");
    notex_a_color = notex_program->getAttribute("a_color");
}

void EWWidgetRendererES2::draw()
{
    for (EGRenderBatchList::iterator rbi = batchList.begin(); rbi != batchList.end(); rbi++) {
        EGRenderBatchPtr &batch = *rbi;
        batch->draw();
    }
}

void EWWidgetRendererES2::begin()
{
    batchList.clear();
    EGRenderBatch *batch = newBatch<EGRenderBatch>();
    batch->setDrawType(EGRenderBatchStateOnly);
    batch->addState(new EGRenderBatchFlagStateES2(GL_DEPTH_TEST, false));
}

void EWWidgetRendererES2::end()
{
    EGRenderBatch *batch = newBatch<EGRenderBatch>();
    batch->setDrawType(EGRenderBatchStateOnly);
    if (scissorEnabled) {
        batch->addState(new EGRenderBatchFlagStateES2(GL_SCISSOR_TEST, false));
    }
    batch->addState(new EGRenderBatchFlagStateES2(GL_DEPTH_TEST, true));
}

void EWWidgetRendererES2::scissor(EGRect rect)
{
    EGint viewportHeight = widget->getContext()->getViewport()->screenHeight;
    EGRenderBatch *batch = newBatch<EGRenderBatch>();
    batch->setDrawType(EGRenderBatchStateOnly);
    batch->addState(new EGRenderBatchFlagStateES2(GL_SCISSOR_TEST, true));
    batch->addState(new EGRenderBatchScissorStateES2(rect.x, viewportHeight - rect.y - rect.height, rect.width, rect.height));
    scissorEnabled = true;
}

void EWWidgetRendererES2::fill(EGRect rect, EGColor color)
{
    EGRenderBatch *batch = lastBatch<EGRenderBatch>();
    if (!(batch && batch->mode == GL_TRIANGLES && batch->vertexSize == 12)){
        batch = newBatch<EGRenderBatch>();
        batch->setDrawType(EGRenderBatchDrawElements);
        batch->setMode(GL_TRIANGLES);
        batch->setVertexSize(12);
    }
    if (color.alpha < 1.0f) {
        batch->addState(new EGRenderBatchBlendES2(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    }
    batch->addState(new EGRenderBatchProgramES2(notex_program))
          .addState(new EGRenderBatchAttribArrayStateES2(notex_a_position->index, 2, GL_FLOAT, 0, 12, (void*)0))
          .addState(new EGRenderBatchAttribArrayStateES2(notex_a_color->index, 4, GL_UNSIGNED_BYTE, 1, 12, (void*)8));
    EGuint rgba32 = color.rgba32();
    EGfloat x1 = rect.x, y1 = rect.y;
    EGfloat x2 = rect.x + rect.width, y2 = rect.y + rect.height;
    EGVertexIndex ind[] = {
        batch->varr->pushVertex(x1, y1, rgba32),
        batch->varr->pushVertex(x1, y2, rgba32),
        batch->varr->pushVertex(x2, y2, rgba32),
        batch->varr->pushVertex(x2, y1, rgba32),
    };
    EGVertexIndex indtri[] = { ind[0], ind[1], ind[3], ind[1], ind[2], ind[3] };
    batch->iarr->pushIndices(indtri, 6);
}

void EWWidgetRendererES2::fill(EGRect outerRect, EGRect innerRect, EGColor color)
{
    EGRenderBatch *batch = lastBatch<EGRenderBatch>();
    if (!(batch && batch->mode == GL_TRIANGLES && batch->vertexSize == 12)) {
        batch = newBatch<EGRenderBatch>();
        batch->setDrawType(EGRenderBatchDrawElements);
        batch->setMode(GL_TRIANGLES);
        batch->setVertexSize(12);
    }
    batch->addState(new EGRenderBatchProgramES2(notex_program))
          .addState(new EGRenderBatchAttribArrayStateES2(notex_a_position->index, 2, GL_FLOAT, 0, 12, (void*)0))
          .addState(new EGRenderBatchAttribArrayStateES2(notex_a_color->index, 4, GL_UNSIGNED_BYTE, 1, 12, (void*)8));
    EGuint rgba32 = color.rgba32();
    EGfloat x1o = outerRect.x,                      y1o = outerRect.y;
    EGfloat x1i = innerRect.x,                      y1i = innerRect.y;
    EGfloat x2o = outerRect.x + outerRect.width,    y2o = outerRect.y + outerRect.height;
    EGfloat x2i = innerRect.x + innerRect.width,    y2i = innerRect.y + innerRect.height;
    EGVertexIndex ind[] = {
        batch->varr->pushVertex(x1i, y1i, rgba32),
        batch->varr->pushVertex(x1o, y1o, rgba32),
        batch->varr->pushVertex(x1i, y2i, rgba32),
        batch->varr->pushVertex(x1o, y2o, rgba32),
        batch->varr->pushVertex(x2i, y2i, rgba32),
        batch->varr->pushVertex(x2o, y2o, rgba32),
        batch->varr->pushVertex(x2i, y1i, rgba32),
        batch->varr->pushVertex(x2o, y1o, rgba32),
    };
    EGVertexIndex indtristrip[] = { ind[0], ind[1], ind[2],
                                    ind[1], ind[3], ind[2],
                                    ind[2], ind[3], ind[4],
                                    ind[3], ind[5], ind[4],
                                    ind[4], ind[5], ind[6],
                                    ind[5], ind[7], ind[6],
                                    ind[6], ind[7], ind[0],
                                    ind[7], ind[1], ind[0] };
    batch->iarr->pushIndices(indtristrip, 24);
}


static void EG_CALLBACK tessMeshPolyBegin(GLenum which, GLvoid *userData)
{
}

static void EG_CALLBACK tessMeshPolyEnd(GLvoid *userData)
{
}

static void EG_CALLBACK tessMeshPolyVertex(GLvoid *data, GLvoid *userData)
{
    EGVertexIndex ind = (EGVertexIndex) (unsigned long) data;
    EGRenderBatch *batch = (EGRenderBatch*) userData;
    batch->iarr->pushIndices(&ind, 1);
}

static void EG_CALLBACK tessMeshPolyEdgeFlag(GLboolean flag, GLvoid *userData)
{
}

void EWWidgetRendererES2::fill(EGPointList poly, EGColor color)
{
    EGRenderBatch *batch = lastBatch<EGRenderBatch>();
    if (!(batch && batch->mode == GL_TRIANGLES && batch->vertexSize == 12)) {
        batch = newBatch<EGRenderBatch>();
        batch->setDrawType(EGRenderBatchDrawElements);
        batch->setMode(GL_TRIANGLES);
        batch->setVertexSize(12);
    }
    if (color.alpha < 1.0f) {
        batch->addState(new EGRenderBatchBlendES2(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    }
    batch->addState(new EGRenderBatchProgramES2(notex_program))
          .addState(new EGRenderBatchAttribArrayStateES2(notex_a_position->index, 2, GL_FLOAT, 0, 12, (void*)0))
          .addState(new EGRenderBatchAttribArrayStateES2(notex_a_color->index, 4, GL_UNSIGNED_BYTE, 1, 12, (void*)8));
    EGuint rgba32 = color.rgba32();
    if (poly.size() == 3) {
        EGVertexIndex ind[] = {
            batch->varr->pushVertex(poly[0].x, poly[0].y, rgba32),
            batch->varr->pushVertex(poly[1].x, poly[1].y, rgba32),
            batch->varr->pushVertex(poly[2].x, poly[2].y, rgba32),
        };
        EGVertexIndex indtri[] = { ind[0], ind[1], ind[2] };
        batch->iarr->pushIndices(indtri, 3);
    } else if (poly.size() == 4) {
        EGVertexIndex ind[] = {
            batch->varr->pushVertex(poly[0].x, poly[0].y, rgba32),
            batch->varr->pushVertex(poly[1].x, poly[1].y, rgba32),
            batch->varr->pushVertex(poly[2].x, poly[2].y, rgba32),
            batch->varr->pushVertex(poly[3].x, poly[3].y, rgba32),
        };
        EGVertexIndex indtri[] = { ind[0], ind[1], ind[3], ind[1], ind[2], ind[3] };
        batch->iarr->pushIndices(indtri, 6);
    } else if (poly.size() > 4) {
        GLUtesselator *tobj = gluNewTess();
        gluTessCallback(tobj, GLU_TESS_VERTEX_DATA, GLU_CALLBACK tessMeshPolyVertex);
        gluTessCallback(tobj, GLU_TESS_BEGIN_DATA, GLU_CALLBACK tessMeshPolyBegin);
        gluTessCallback(tobj, GLU_TESS_END_DATA, GLU_CALLBACK tessMeshPolyEnd);
        gluTessCallback(tobj, GLU_TESS_EDGE_FLAG_DATA, GLU_CALLBACK tessMeshPolyEdgeFlag);
        gluTessProperty(tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE);
        gluTessNormal(tobj, 0, 0, 1);
        gluTessBeginPolygon(tobj, batch);
        gluTessBeginContour(tobj);
        GLdouble *vec, *vecp;
        vecp = vec = new GLdouble[3 * poly.size()];
        EGVertexIndex *ind = new EGVertexIndex[poly.size()];
        for (EGsize i = 0; i < poly.size(); i++) {
            ind[i] = batch->varr->pushVertex(poly[i].x, poly[i].y, rgba32);
            *vecp++ = (GLdouble)poly[i].x;
            *vecp++ = (GLdouble)poly[i].y;
            *vecp++ = (GLdouble)0;
        }
        vecp = vec;
        for (EGsize i = 0; i < poly.size(); i++) {
            gluTessVertex(tobj, vecp, (void*) (unsigned long) ind[i]);
            vecp += 3;
        }
        gluTessEndContour(tobj);
        gluTessEndPolygon(tobj);
        delete[] vec;
        delete[] ind;
        gluDeleteTess(tobj);
    }
}

void EWWidgetRendererES2::stroke(EGRect rect, EGColor color, EGfloat width, EWStrokeRectFlags flags)
{
    EGfloat iw = 0, ow = 0;
    switch (flags) {
        case EWStrokeRectInner:  ow = 0;             iw = width;        break;
        case EWStrokeRectCenter: ow = width * 0.5f;  iw = width * 0.5f; break;
        case EWStrokeRectOuter:  ow = width;         iw = 0;            break;
    }
    EGfloat x1o = rect.x - ow,                y1o = rect.y - ow;
    EGfloat x1i = rect.x + iw,                y1i = rect.y + iw;
    EGfloat x2o = rect.x + rect.width + ow,   y2o = rect.y + rect.height + ow;
    EGfloat x2i = rect.x + rect.width - iw,   y2i = rect.y + rect.height - iw;
    fill(EGRect(x1o, y1o, x2o - x1o, y2o - y1o), EGRect(x1i, y1i, x2i - x1i, y2i - y1i), color);
}

void EWWidgetRendererES2::stroke(EGLine line, EGColor color, EGfloat width)
{
    EGRenderBatch *batch = lastBatch<EGRenderBatch>();
    if (!(batch && batch->mode == GL_LINES && batch->vertexSize == 12)) {
        batch = newBatch<EGRenderBatch>();
        batch->setDrawType(EGRenderBatchDrawElements);
        batch->setMode(GL_LINES);
        batch->setVertexSize(12);
    }
    if (color.alpha < 1.0f) {
        batch->addState(new EGRenderBatchBlendES2(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    }
    batch->addState(new EGRenderBatchProgramES2(notex_program))
          .addState(new EGRenderBatchLineWidthStateES2(width))
          .addState(new EGRenderBatchAttribArrayStateES2(notex_a_position->index, 2, GL_FLOAT, 0, 12, (void*)0))
          .addState(new EGRenderBatchAttribArrayStateES2(notex_a_color->index, 4, GL_UNSIGNED_BYTE, 1, 12, (void*)8));
    EGuint rgba32 = color.rgba32();
    EGfloat x1 = line.p1.x, y1 = line.p1.y;
    EGfloat x2 = line.p2.x, y2 = line.p2.y;
    EGVertexIndex ind[] = {
        batch->varr->pushVertex(x1, y1, rgba32),
        batch->varr->pushVertex(x2, y2, rgba32),
    };
    EGVertexIndex indline[] = { ind[0], ind[1] };
    batch->iarr->pushIndices(indline, 2);
}

void EWWidgetRendererES2::stroke(EGPointList poly, EGColor color, EGfloat width, EGbool closed)
{
    EGRenderBatch *batch = lastBatch<EGRenderBatch>();
    if (!(batch && batch->mode == GL_LINE_STRIP && batch->vertexSize == 12)) {
        batch = newBatch<EGRenderBatch>();
        batch->setDrawType(EGRenderBatchDrawElements);
        batch->setMode(GL_LINE_STRIP);
        batch->setVertexSize(12);
    }
    if (color.alpha < 1.0f) {
        batch->addState(new EGRenderBatchBlendES2(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    }
    batch->addState(new EGRenderBatchProgramES2(notex_program))
          .addState(new EGRenderBatchLineWidthStateES2(width))
          .addState(new EGRenderBatchAttribArrayStateES2(notex_a_position->index, 2, GL_FLOAT, 0, 12, (void*)0))
          .addState(new EGRenderBatchAttribArrayStateES2(notex_a_color->index, 4, GL_UNSIGNED_BYTE, 1, 12, (void*)8));
    EGuint rgba32 = color.rgba32();
    EGVertexIndex *ind = new EGVertexIndex[poly.size() + (closed ? 1 : 0)];
    for (EGsize i = 0; i < poly.size(); i++) {
        ind[i] = batch->varr->pushVertex(poly[i].x, poly[i].y, rgba32);
    }
    if (closed) {
        ind[poly.size()] = ind[0];
    }
    batch->iarr->pushIndices(ind, (EGVertexIndex)poly.size() + (closed ? 1 : 0));
    delete [] ind;
}

void EWWidgetRendererES2::paint(EGRect rect, EGImagePtr image, EGColor color, EGfloat *uvs)
{
    EGRenderBatch *batch = newBatch<EGRenderBatch>();
    batch->setDrawType(EGRenderBatchDrawElements);
    batch->setMode(GL_TRIANGLES);
    batch->setVertexSize(20);
    batch->addState(new EGRenderBatchProgramES2(tex_program))
          .addState(new EGRenderBatchBlendES2(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA))
          .addState(new EGRenderBatchTextureStateES2(GL_TEXTURE_2D, image->getGLTexId(), 0))
          .addState(new EGRenderBatchUniform1iES2(tex_u_texture0->location, 0))
          .addState(new EGRenderBatchAttribArrayStateES2(tex_a_position->index, 2, GL_FLOAT, 0, 20, (void*)0))
          .addState(new EGRenderBatchAttribArrayStateES2(tex_a_texcoord0->index, 2, GL_FLOAT, 0, 20, (void*)8))
          .addState(new EGRenderBatchAttribArrayStateES2(tex_a_color->index, 4, GL_UNSIGNED_BYTE, 1, 20, (void*)16));
    EGuint rgba32 = color.rgba32();
    EGfloat x1 = rect.x, y1 = rect.y;
    EGfloat x2 = rect.x + rect.width, y2 = rect.y + rect.height;
    EGfloat *uv = uvs ? uvs : image->getUV();
    EGVertexIndex ind[] = {
        batch->varr->pushVertex(x1, y1, uv[0], uv[1], rgba32),
        batch->varr->pushVertex(x1, y2, uv[2], uv[3], rgba32),
        batch->varr->pushVertex(x2, y2, uv[6], uv[7], rgba32),
        batch->varr->pushVertex(x2, y1, uv[4], uv[5], rgba32),
    };
    EGVertexIndex indtri[] = { ind[0], ind[1], ind[3], ind[1], ind[2], ind[3] };
    batch->iarr->pushIndices(indtri, 6);
}

void EWWidgetRendererES2::paint(EGPoint point, EGTextPtr text, EGColor color)
{
    EGRenderBatchText *batch = newBatch<EGRenderBatchText>();
    batch->text = text;
    text->setColor(color);
    text->setPosition(point.x, point.y);
}
