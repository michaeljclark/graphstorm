// See LICENSE for license details.

#pragma once

template <typename VecType, typename GraphLayoutType>
class EGGraphNodeRendererES2 : public EGGraphNodeRenderer<VecType>
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
    
    EGRenderProgramPtr point_program;
    EGRenderAttributeInfo* point_a_position;
    EGRenderAttributeInfo* point_a_pointsize;
    EGRenderAttributeInfo* point_a_color;
    
    EGRenderProgramPtr cube_program;
    EGRenderAttributeInfo* cube_a_position;
    EGRenderAttributeInfo* cube_a_color;
    
public:
    typedef std::shared_ptr<EGGraphNodeRendererES2> RendererPtr;
    
    EGGraphNodeRendererES2(EGGraphPtr graph, EGGraphLayoutPtr layout)
        : graph(graph), layout(layout), lastParams(), batch("EGGraphNodeRendererES2")
    {
        // link shader attributes and uniforms
        EGRenderApiES2 &gl = EGRenderApiES2::apiImpl();
        link(gl.getRenderProgram(EGResource::getResource("Resources/gles2.bundle/Shaders/NoLightingNoTexture.vsh"),
                                 EGResource::getResource("Resources/gles2.bundle/Shaders/NoLightingNoTexture.fsh")),
             gl.getRenderProgram(EGResource::getResource("Resources/gles2.bundle/Shaders/PointNoLightingNoTexture.vsh"),
                                 EGResource::getResource("Resources/gles2.bundle/Shaders/PointNoLightingNoTexture.fsh")));
        
        EGDebug("%s initialized\n", __func__);
    }
    
    virtual void link(EGRenderProgramPtr cube_program, EGRenderProgramPtr point_program)
    {
        this->cube_program = cube_program;
        cube_a_position = cube_program->getAttribute("a_position");
        cube_a_color = cube_program->getAttribute("a_color");
        
        this->point_program = point_program;
        point_a_position = point_program->getAttribute("a_position");
        point_a_pointsize = point_program->getAttribute("a_pointsize");
        point_a_color = point_program->getAttribute("a_color");
    }
    
    virtual void update(EGViewportPtr &viewport, EGGraphRendererParams<VecType> &params)
    {
        // setup batch state
        if (params != lastParams) {
            if (params.defaultNodeShape == EGGraphRendererParamNodeShapeCube) {
                if (params.viewDimensions == 2) {
                    batch.clearState()
                         .setDrawType(EGRenderBatchDrawElements)
                         .setMode(GL_TRIANGLES)
                         .setVertexSize(12)
                         .addState(new EGRenderBatchAttribArrayStateES2(cube_a_position->index, 2, GL_FLOAT, 0, 12, (void*)0))
                         .addState(new EGRenderBatchAttribArrayStateES2(cube_a_color->index, 4, GL_UNSIGNED_BYTE, 1, 12, (void*)8));
                }
                if (params.viewDimensions == 3) {
                    batch.clearState()
                         .setDrawType(EGRenderBatchDrawElements)
                         .setMode(GL_TRIANGLES)
                         .setVertexSize(28)
                         .addState(new EGRenderBatchAttribArrayStateES2(cube_a_position->index, 3, GL_FLOAT, 0, 28, (void*)0))
                         .addState(new EGRenderBatchAttribArrayStateES2(cube_a_color->index, 4, GL_UNSIGNED_BYTE, 1, 28, (void*)24));
                }
            }
            if (params.defaultNodeShape == EGGraphRendererParamNodeShapePoint) {
                if (params.viewDimensions == 2) {
                    batch.clearState()
                         .setDrawType(EGRenderBatchDrawElements)
                         .setMode(GL_POINTS)
                         .setVertexSize(16)
#ifndef NACL
                         .addState(new EGRenderBatchFlagStateES2(GL_PROGRAM_POINT_SIZE_EXT, true))
#endif
                         .addState(new EGRenderBatchAttribArrayStateES2(point_a_position->index, 2, GL_FLOAT, 0, 16, (void*)0))
                         .addState(new EGRenderBatchAttribArrayStateES2(point_a_pointsize->index, 4, GL_FLOAT, 1, 16, (void*)8))
                         .addState(new EGRenderBatchAttribArrayStateES2(point_a_color->index, 4, GL_UNSIGNED_BYTE, 1, 16, (void*)12));
                }
                if (params.viewDimensions == 3) {
                    batch.clearState()
                         .setDrawType(EGRenderBatchDrawElements)
                         .setMode(GL_POINTS)
                         .setVertexSize(20)
#ifndef NACL
                         .addState(new EGRenderBatchFlagStateES2(GL_PROGRAM_POINT_SIZE_EXT, true))
#endif
                         .addState(new EGRenderBatchAttribArrayStateES2(point_a_position->index, 3, GL_FLOAT, 0, 20, (void*)0))
                         .addState(new EGRenderBatchAttribArrayStateES2(point_a_pointsize->index, 4, GL_FLOAT, 1, 20, (void*)12))
                         .addState(new EGRenderBatchAttribArrayStateES2(point_a_color->index, 4, GL_UNSIGNED_BYTE, 1, 20, (void*)16));
                }
            }
            lastParams = params;
        }
        
        // get graph nodes
        EGGraphNodeList *nodes = graph->getNodeList();
        
        // get graph bounds
        VecType bound = params.maxBound - params.minBound;
        
        // point size
        EGfloat pointSizeMult = 1;
        if (params.defaultNodeShape == EGGraphRendererParamNodeShapeCube) {
            pointSizeMult = 0.5f * params.defaultNodePointSize / ((viewport->screenWidth + viewport->screenHeight) * 0.25f);
        } else if (params.defaultNodeShape == EGGraphRendererParamNodeShapePoint) {
            pointSizeMult = params.defaultNodePointSize;
        }

        // fill dimension array
        EGint d[3];
        for (EGint i = 0; i < params.viewDimensions; i++) {
            d[i] = params.visibleDimensions[i];
        }
        
        // setup vertex and index buffer for node squares
        batch.rewind();
        EGuint defaultNodeColorRgba32 = params.defaultNodeColor.rgba32();
        for (EGGraphNodeList::iterator ni = nodes->begin(); ni != nodes->end(); ni++) {
            EGGraphNode *node = *ni;
            EGGraphPoint<VecType> *point = layout->getPoint(node);
            VecType pos = point->getPosition();
            VecType proj = (pos - params.minBound) / bound * params.viewBound + params.viewOffset;
            EGGraphDataPtr sizedata = point->node->getData(EGGraphDataSize::ID);
            EGfloat pointSize = sizedata ? static_cast<EGGraphDataSize&>(*sizedata).getSize() : 1.0f;
            EGfloat h = pointSize * pointSizeMult;
            EGGraphDataPtr colordata = node->getData(EGGraphDataColor::ID);
            EGuint rgba32 = colordata ? static_cast<EGGraphDataColor&>(*colordata).getColor().rgba32() : defaultNodeColorRgba32;
            if (params.defaultNodeShape == EGGraphRendererParamNodeShapeCube) {
                if (params.viewDimensions == 2) {
                    EGfloat x1 = proj[d[0]] - h, y1 = proj[d[1]] - h;
                    EGfloat x2 = proj[d[0]] + h, y2 = proj[d[1]] + h;
                    EGVertexIndex ind[] = {
                        batch.varr->pushVertex(x1, y1, rgba32),
                        batch.varr->pushVertex(x1, y2, rgba32),
                        batch.varr->pushVertex(x2, y2, rgba32),
                        batch.varr->pushVertex(x2, y1, rgba32),
                    };
                    EGVertexIndex indtri[] = { ind[0], ind[1], ind[3], ind[1], ind[2], ind[3] };
                    batch.iarr->pushIndices(indtri, 6);
                } else if (params.viewDimensions == 3) {
                    EGfloat x1 = proj[d[0]] - h, y1 = proj[d[1]] - h, z1 = proj[d[2]] - h;
                    EGfloat x2 = proj[d[0]] + h, y2 = proj[d[1]] + h, z2 = proj[d[2]] + h;
                    EGVertexIndex ind[] = {
                        // front
                        batch.varr->pushVertex(x1, y1, z1, 0.0f, 0.0f, -1.0f, rgba32), // 0
                        batch.varr->pushVertex(x2, y1, z1, 0.0f, 0.0f, -1.0f, rgba32), // 1
                        batch.varr->pushVertex(x2, y2, z1, 0.0f, 0.0f, -1.0f, rgba32), // 2
                        batch.varr->pushVertex(x1, y2, z1, 0.0f, 0.0f, -1.0f, rgba32), // 3
                        // back
                        batch.varr->pushVertex(x1, y1, z2, 0.0f, 0.0f, 1.0f, rgba32), // 4
                        batch.varr->pushVertex(x2, y1, z2, 0.0f, 0.0f, 1.0f, rgba32), // 5
                        batch.varr->pushVertex(x2, y2, z2, 0.0f, 0.0f, 1.0f, rgba32), // 6
                        batch.varr->pushVertex(x1, y2, z2, 0.0f, 0.0f, 1.0f, rgba32), // 7
                        // top
                        batch.varr->pushVertex(x1, y2, z1, 0.0f, 1.0f, 0.0f, rgba32), // 8
                        batch.varr->pushVertex(x2, y2, z1, 0.0f, 1.0f, 0.0f, rgba32), // 9
                        batch.varr->pushVertex(x2, y2, z2, 0.0f, 1.0f, 0.0f, rgba32), // 10
                        batch.varr->pushVertex(x1, y2, z2, 0.0f, 1.0f, 0.0f, rgba32), // 11
                        // bottom
                        batch.varr->pushVertex(x1, y1, z1, 0.0f, -1.0f, 0.0f, rgba32), // 12
                        batch.varr->pushVertex(x2, y1, z1, 0.0f, -1.0f, 0.0f, rgba32), // 13
                        batch.varr->pushVertex(x2, y1, z2, 0.0f, -1.0f, 0.0f, rgba32), // 14
                        batch.varr->pushVertex(x1, y1, z2, 0.0f, -1.0f, 0.0f, rgba32), // 15
                        // left
                        batch.varr->pushVertex(x1, y1, z2, -1.0f, 0.0f, 0.0f, rgba32), // 16
                        batch.varr->pushVertex(x1, y1, z1, -1.0f, 0.0f, 0.0f, rgba32), // 17
                        batch.varr->pushVertex(x1, y2, z1, -1.0f, 0.0f, 0.0f, rgba32), // 18
                        batch.varr->pushVertex(x1, y2, z2, -1.0f, 0.0f, 0.0f, rgba32), // 19
                        // right
                        batch.varr->pushVertex(x2, y1, z1, 1.0f, 0.0f, 0.0f, rgba32), // 20
                        batch.varr->pushVertex(x2, y1, z2, 1.0f, 0.0f, 0.0f, rgba32), // 21
                        batch.varr->pushVertex(x2, y2, z2, 1.0f, 0.0f, 0.0f, rgba32), // 22
                        batch.varr->pushVertex(x2, y2, z1, 1.0f, 0.0f, 0.0f, rgba32), // 23
                    };
                    EGVertexIndex indtri[] =
                    {
                        // front
                        ind[0], ind[2], ind[1],
                        ind[0], ind[3], ind[2],
                        // back
                        ind[4], ind[5], ind[6],
                        ind[6], ind[7], ind[4],
                        // top
                        ind[8], ind[10], ind[9],
                        ind[8], ind[11], ind[10],
                        // bottom
                        ind[12], ind[13], ind[14],
                        ind[14], ind[15], ind[12],
                        // left
                        ind[16], ind[18], ind[17],
                        ind[16], ind[19], ind[18],
                        // right
                        ind[20], ind[21], ind[22],
                        ind[22], ind[23], ind[20],
                     };
                    batch.iarr->pushIndices(indtri, 36);
                }
            } else if (params.defaultNodeShape == EGGraphRendererParamNodeShapePoint) {
                if (params.viewDimensions == 2) {
                    EGVertexIndex ind = batch.varr->pushVertex(proj[d[0]], proj[d[1]], h, rgba32);
                    batch.iarr->pushIndices(&ind, 1);
                } else if (params.viewDimensions == 3) {
                    EGVertexIndex ind = batch.varr->pushVertex(proj[d[0]], proj[d[1]], proj[d[2]], h, rgba32);
                    batch.iarr->pushIndices(&ind, 1);
                }
            }
        }
    }

    virtual void draw(EGViewportPtr &, EGGraphRendererParams<VecType> &params)
    {
        EGRenderApiES2 &gl = EGRenderApiES2::apiImpl();
        if (params.defaultNodeShape == EGGraphRendererParamNodeShapeCube) {
            gl.useProgram(cube_program);
        }
        if (params.defaultNodeShape == EGGraphRendererParamNodeShapePoint) {
            gl.useProgram(point_program);
        }
        batch.draw();
    }
};
