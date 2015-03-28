/*
 *  EGGraphNodeRendererES2CL.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGGraphNodeRendererES2CL_H
#define EGGraphNodeRendererES2CL_H


/* EGGraphNodeRendererES2CL */

template <typename VecType, typename GraphLayoutType>
class EGGraphNodeRendererES2CL : public EGGraphNodeRenderer<VecType>
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
    typedef std::shared_ptr<EGGraphNodeRendererES2CL> RendererPtr;
    
    EGGraphNodeRendererES2CL(EGGraphPtr graph, EGGraphLayoutPtr layout)
        : graph(graph), layout(layout), lastParams(), batch("EGGraphNodeRendererES2CL")
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
    
    virtual void update(EGViewportPtr &, EGGraphRendererParams<VecType> &params)
    {
        // setup batch state
        if (params != lastParams) {
            if (params.defaultNodeShape == EGGraphRendererParamNodeShapeCube) {
                if (params.viewDimensions == 2) {
                    batch.clearState()
                    .setMode(GL_TRIANGLES)
                    .addState(new EGRenderBatchAttribArrayStateES2(cube_a_position->index, 2, GL_FLOAT, 0, 12, (void*)0))
                    .addState(new EGRenderBatchAttribArrayStateES2(cube_a_color->index, 4, GL_UNSIGNED_BYTE, 1, 12, (void*)8));
                }
                if (params.viewDimensions == 3) {
                    batch.clearState()
                    .setMode(GL_TRIANGLES)
                    .addState(new EGRenderBatchAttribArrayStateES2(cube_a_position->index, 3, GL_FLOAT, 0, 28, (void*)0))
                    .addState(new EGRenderBatchAttribArrayStateES2(cube_a_color->index, 4, GL_UNSIGNED_BYTE, 1, 28, (void*)24));
                }
            }
            if (params.defaultNodeShape == EGGraphRendererParamNodeShapePoint) {
                if (params.viewDimensions == 2) {
                    batch.clearState()
                    .setMode(GL_POINTS)
                    .addState(new EGRenderBatchFlagStateES2(GL_PROGRAM_POINT_SIZE_EXT, true))
                    .addState(new EGRenderBatchAttribArrayStateES2(point_a_position->index, 2, GL_FLOAT, 0, 16, (void*)0))
                    .addState(new EGRenderBatchAttribArrayStateES2(point_a_pointsize->index, 4, GL_FLOAT, 1, 16, (void*)8))
                    .addState(new EGRenderBatchAttribArrayStateES2(point_a_color->index, 4, GL_UNSIGNED_BYTE, 1, 16, (void*)12));
                }
                if (params.viewDimensions == 3) {
                    batch.clearState()
                    .setMode(GL_POINTS)
                    .addState(new EGRenderBatchFlagStateES2(GL_PROGRAM_POINT_SIZE_EXT, true))
                    .addState(new EGRenderBatchAttribArrayStateES2(point_a_position->index, 3, GL_FLOAT, 0, 20, (void*)0))
                    .addState(new EGRenderBatchAttribArrayStateES2(point_a_pointsize->index, 4, GL_FLOAT, 1, 20, (void*)12))
                    .addState(new EGRenderBatchAttribArrayStateES2(point_a_color->index, 4, GL_UNSIGNED_BYTE, 1, 20, (void*)16));
                }
            }
            lastParams = params;
        }
        
        // generate vertex buffers on the GPU
        GLuint nodeibo = 0, nodevbo = 0, nodeind = 0;
        switch (params.defaultNodeShape) {
            case EGGraphRendererParamNodeShapeCube:
                static_cast<GraphLayoutType*>(layout.get())->getVertexBufferCubes(params, nodeibo, nodevbo, nodeind);
                break;
            case EGGraphRendererParamNodeShapePoint:
                static_cast<GraphLayoutType*>(layout.get())->getVertexBufferPoints(params, nodeibo, nodevbo, nodeind);
                break;
        }
        batch.setVertexBuffers(EGRenderBatchDrawElements, nodeind, nodevbo, nodeibo);
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

#endif
