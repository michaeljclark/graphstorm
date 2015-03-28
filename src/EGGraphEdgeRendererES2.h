/*
 *  EGGraphEdgeRendererES2.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGGraphEdgeRendererES2_H
#define EGGraphEdgeRendererES2_H


/* EGGraphEdgeRendererES2 */

template <typename VecType, typename GraphLayoutType>
class EGGraphEdgeRendererES2 : public EGGraphEdgeRenderer<VecType>
{
public:
    typedef EGGraphPoint<VecType> EGGraphPointType;
    typedef EGGraphLayout<VecType> EGGraphLayoutType;
    typedef typename EGGraphLayoutType::LayoutPtr EGGraphLayoutPtr;

protected:
    EGGraphPtr graph;
    EGGraphLayoutPtr layout;
    EGGraphRendererParams<VecType> lastParams;
    EGRenderBatch batch;
    
    EGRenderProgramPtr program;
    EGRenderAttributeInfo* a_position;
    EGRenderAttributeInfo* a_color;
        
public:
    typedef std::shared_ptr<EGGraphEdgeRendererES2> RendererPtr;
    
    EGGraphEdgeRendererES2(EGGraphPtr graph, EGGraphLayoutPtr layout)
        : graph(graph), layout(layout), lastParams(), batch("EGGraphEdgeRendererES2")
    {        
        // link shader attributes
        EGRenderApiES2 &gl = EGRenderApiES2::apiImpl();
        link(gl.getRenderProgram(EGResource::getResource("Resources/gles2.bundle/Shaders/NoLightingNoTexture.vsh"),
                                 EGResource::getResource("Resources/gles2.bundle/Shaders/NoLightingNoTexture.fsh")));

        EGDebug("%s initialized\n", __func__);
    }

    virtual void link(EGRenderProgramPtr program)
    {
        this->program = program;
        a_position = program->getAttribute("a_position");
        a_color = program->getAttribute("a_color");
    }
        
    virtual void update(EGViewportPtr &, EGGraphRendererParams<VecType> &params)
    {
        // setup batch state
        if (params != lastParams) {
            if (params.viewDimensions == 2) {
                batch.clearState()
                     .setDrawType(EGRenderBatchDrawElements)
                     .setMode(GL_LINES)
                     .setVertexSize(12)
                     .addState(new EGRenderBatchLineWidthStateES2(params.defaultEdgeLineWidth))
                     .addState(new EGRenderBatchAttribArrayStateES2(a_position->index, 2, GL_FLOAT, 0, 12, (void*)0))
                     .addState(new EGRenderBatchAttribArrayStateES2(a_color->index, 4, GL_UNSIGNED_BYTE, 1, 12, (void*)8));
            }
            if (params.viewDimensions == 3) {
                batch.clearState()
                     .setDrawType(EGRenderBatchDrawElements)
                     .setMode(GL_LINES)
                     .setVertexSize(16)
                     .addState(new EGRenderBatchLineWidthStateES2(params.defaultEdgeLineWidth))
                     .addState(new EGRenderBatchAttribArrayStateES2(a_position->index, 3, GL_FLOAT, 0, 16, (void*)0))
                     .addState(new EGRenderBatchAttribArrayStateES2(a_color->index, 4, GL_UNSIGNED_BYTE, 1, 16, (void*)12));
            }
            lastParams = params;
        }
        
        // get graph edges
        EGGraphEdgeList *edges = graph->getEdgeList();
        
        // get graph bounds
        VecType bound = params.maxBound - params.minBound;
        
        // fill dimension array
        EGint d[3];
        for (EGint i = 0; i < params.viewDimensions; i++) {
            d[i] = params.visibleDimensions[i];
        }
        
        // setup vertex and index buffer for edge lines
        batch.rewind();
        EGuint defaultEdgeColorRgba32 = params.defaultEdgeColor.rgba32();
        for (EGGraphEdgeList::iterator ei = edges->begin(); ei != edges->end(); ei++) {
            EGGraphEdge *edge = *ei;
            EGGraphNode *node1 = edge->getSource();
            EGGraphNode *node2 = edge->getTarget();
            EGGraphPoint<VecType> *point1 = layout->getPoint(node1);
            EGGraphPoint<VecType> *point2 = layout->getPoint(node2);
            EGGraphDataPtr colordata = edge->getData(EGGraphDataColor::ID);
            EGuint rgba32 = colordata ? static_cast<EGGraphDataColor&>(*colordata).getColor().rgba32() : defaultEdgeColorRgba32;
            VecType pos1 = point1->getPosition();
            VecType pos2 = point2->getPosition();
            VecType proj1 = (pos1 - params.minBound) / bound * params.viewBound + params.viewOffset;
            VecType proj2 = (pos2 - params.minBound) / bound * params.viewBound + params.viewOffset;
            if (params.viewDimensions == 2) {
                EGfloat x1 = proj1[d[0]], y1 = proj1[d[1]];
                EGfloat x2 = proj2[d[0]], y2 = proj2[d[1]];
                EGVertexIndex ind[4] = {
                    batch.varr->pushVertex(x1, y1, rgba32),
                    batch.varr->pushVertex(x2, y2, rgba32),
                };
                EGVertexIndex indline[] = { ind[0], ind[1] };
                batch.iarr->pushIndices(indline, 2);
            } else if (params.viewDimensions == 3) {
                EGfloat x1 = proj1[d[0]], y1 = proj1[d[1]], z1 = proj1[d[2]];
                EGfloat x2 = proj2[d[0]], y2 = proj2[d[1]], z2 = proj2[d[2]];
                EGVertexIndex ind[4] = {
                    batch.varr->pushVertex(x1, y1, z1, rgba32),
                    batch.varr->pushVertex(x2, y2, z2, rgba32),
                };
                EGVertexIndex indline[] = { ind[0], ind[1] };
                batch.iarr->pushIndices(indline, 2);
            }
        }
    }
    
    virtual void draw(EGViewportPtr &, EGGraphRendererParams<VecType> &)
    {
        EGRenderApiES2 &gl = EGRenderApiES2::apiImpl();
        gl.useProgram(program);
        batch.draw();
    }
};

#endif
