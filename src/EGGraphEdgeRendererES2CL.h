// See LICENSE for license details.

#pragma once

/* EGGraphEdgeRendererES2CL */

template <typename VecType, typename GraphLayoutType>
class EGGraphEdgeRendererES2CL : public EGGraphEdgeRenderer<VecType>
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
    typedef std::shared_ptr<EGGraphEdgeRendererES2CL> RendererPtr;
    
    EGGraphEdgeRendererES2CL(EGGraphPtr graph, EGGraphLayoutPtr layout)
        : graph(graph), layout(layout), lastParams(), batch("EGGraphEdgeRendererES2CL")
    {
        // link shader attributes and uniforms
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
                     .setMode(GL_LINES)
                     .addState(new EGRenderBatchLineWidthStateES2(params.defaultEdgeLineWidth))
                     .addState(new EGRenderBatchAttribArrayStateES2(a_position->index, 2, GL_FLOAT, 0, 12, (void*)0))
                     .addState(new EGRenderBatchAttribArrayStateES2(a_color->index, 4, GL_UNSIGNED_BYTE, 1, 12, (void*)8));
            }
            if (params.viewDimensions == 3) {
                batch.clearState()
                     .setMode(GL_LINES)
                     .addState(new EGRenderBatchLineWidthStateES2(params.defaultEdgeLineWidth))
                     .addState(new EGRenderBatchAttribArrayStateES2(a_position->index, 3, GL_FLOAT, 0, 16, (void*)0))
                     .addState(new EGRenderBatchAttribArrayStateES2(a_color->index, 4, GL_UNSIGNED_BYTE, 1, 16, (void*)12));
            }
            lastParams = params;
        }
        
        // generate vertex buffers on the GPU
        GLuint edgeibo = 0, edgevbo = 0, edgeind = 0;
        static_cast<GraphLayoutType*>(layout.get())->getVertexBufferEdges(params, edgeibo, edgevbo, edgeind);
        batch.setVertexBuffers(EGRenderBatchDrawElements, edgeind, edgevbo, edgeibo);
    }
    
    virtual void draw(EGViewportPtr &, EGGraphRendererParams<VecType> &)
    {
        EGRenderApiES2 &gl = EGRenderApiES2::apiImpl();
        gl.useProgram(program);
        batch.draw();
    }
};
