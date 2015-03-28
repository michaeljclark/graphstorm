/*
 *  EGTreeRendererES2.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGTreeRendererES2_H
#define EGTreeRendererES2_H


/* EGTreeRendererES2 */

template <typename DataType, typename NodeType, typename VecType>
class EGTreeRendererES2 : public EGTreeRenderer<VecType>
{
protected:
    EGRenderProgramPtr program;
    EGRenderAttributeInfo* a_position;
    EGRenderAttributeInfo* a_normal;
    EGRenderAttributeInfo* a_color;
    
    EGTree<DataType> *tree;
    EGGraphRendererParams<VecType> lastParams;
    EGRenderBatch batch;

public:
    EGTreeRendererES2(EGTree<DataType> *tree) : tree(tree), lastParams(), batch("EGTreeRendererES2")
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
                     .addState(new EGRenderBatchLineWidthStateES2(1.0f))
                     .addState(new EGRenderBatchAttribArrayStateES2(a_position->index, 2, GL_FLOAT, 0, 12, (void*)0))
                     .addState(new EGRenderBatchAttribArrayStateES2(a_color->index, 4, GL_UNSIGNED_BYTE, 1, 12, (void*)8));
            }
            if (params.viewDimensions == 3) {
                batch.clearState()
                     .setDrawType(EGRenderBatchDrawElements)
                     .setMode(GL_LINES)
                     .setVertexSize(16)
                     .addState(new EGRenderBatchLineWidthStateES2(1.0f))
                     .addState(new EGRenderBatchAttribArrayStateES2(a_position->index, 3, GL_FLOAT, 0, 16, (void*)0))
                     .addState(new EGRenderBatchAttribArrayStateES2(a_color->index, 4, GL_UNSIGNED_BYTE, 1, 16, (void*)12));
            }
            lastParams = params;
        }
        
        // create visitor function object
        EGColor color(0.7f, 0.7f, 1.0f, 1.0f);
        EGTreeRendererVisitor rendererVisitor(batch, color, params);
        
        // visit all nodes and create gl batch
        tree->visit(rendererVisitor);
    }
    
    virtual void draw(EGViewportPtr &, EGGraphRendererParams<VecType> &)
    {
        EGRenderApiES2 &gl = EGRenderApiES2::apiImpl();
        gl.useProgram(program);
        batch.draw();
    }
 
protected:
    struct EGTreeRendererVisitor : public EGTreeNodeVisitor
    {
        EGRenderBatch &batch;
        EGColor color;
        VecType bound;
        EGGraphRendererParams<VecType> params;
        EGint d[3];
        
        EGTreeRendererVisitor(EGRenderBatch &batch, EGColor color, EGGraphRendererParams<VecType> &params)
            : batch(batch), color(color), bound(params.maxBound - params.minBound), params(params)
        {
            batch.rewind();
            
            // fill dimension array
            for (EGint i = 0; i < params.viewDimensions; i++) {
                d[i] = params.visibleDimensions[i];
            }
        }
        
        virtual void preDescend(EGTreeNode *node)
        {
            VecType center = static_cast<NodeType*>(node)->container.center;
            VecType half = static_cast<NodeType*>(node)->container.half;
            VecType pos1 = center - half;
            VecType pos2 = center + half;
            VecType proj1 = (pos1 - params.minBound) / bound * params.viewBound + params.viewOffset;
            VecType proj2 = (pos2 - params.minBound) / bound * params.viewBound + params.viewOffset;
            EGuint rgba32 = color.rgba32();
            if (params.viewDimensions == 2) {
                EGfloat x1 = floorf(proj1[d[0]]), y1 = floorf(proj1[d[1]]);
                EGfloat x2 = floorf(proj2[d[0]]), y2 = floorf(proj2[d[1]]);
                EGVertexIndex ind[4] = {
                    batch.varr->pushVertex(x1, y1, rgba32),
                    batch.varr->pushVertex(x1, y2, rgba32),
                    batch.varr->pushVertex(x2, y2, rgba32),
                    batch.varr->pushVertex(x2, y1, rgba32),
                };
                EGVertexIndex indline[] = { ind[0], ind[1], ind[1], ind[2], ind[2], ind[3], ind[3], ind[0] };
                batch.iarr->pushIndices(indline, 8);
            } else if (params.viewDimensions == 3) {
                EGfloat x1 = proj1[d[0]], y1 = proj1[d[1]], z1 = proj1[d[2]];
                EGfloat x2 = proj2[d[0]], y2 = proj2[d[1]], z2 = proj2[d[2]];
                EGVertexIndex ind[] = {
                    batch.varr->pushVertex(x1, y1, z1, rgba32), // 0
                    batch.varr->pushVertex(x2, y1, z1, rgba32), // 1
                    batch.varr->pushVertex(x2, y2, z1, rgba32), // 2
                    batch.varr->pushVertex(x1, y2, z1, rgba32), // 3
                    batch.varr->pushVertex(x1, y1, z2, rgba32), // 4
                    batch.varr->pushVertex(x2, y1, z2, rgba32), // 5
                    batch.varr->pushVertex(x2, y2, z2, rgba32), // 6
                    batch.varr->pushVertex(x1, y2, z2, rgba32), // 7
                };
                EGVertexIndex indline[] = {
                    ind[0], ind[1],
                    ind[1], ind[2],
                    ind[2], ind[3],
                    ind[3], ind[0],
                    ind[0], ind[4],
                    ind[1], ind[5],
                    ind[2], ind[6],
                    ind[3], ind[7],
                    ind[4], ind[5],
                    ind[5], ind[6],
                    ind[6], ind[7],
                    ind[7], ind[4],
                };
                batch.iarr->pushIndices(indline, 24);
            }
        }
        virtual EGbool shouldDescend(EGTreeNode *) { return true; }
        virtual void postDescend(EGTreeNode *) {}
    };
};

#endif
