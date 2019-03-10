// See LICENSE for license details.

#pragma once

class EGText;


/* EGTextRendererES2 */

class EGTextRendererES2 : public EGTextRenderer
{
protected:
    EGText *obj;
    GLuint texId;
    GLuint vbo;
    
    static EGRenderProgramPtr program;
    static EGRenderAttributeInfo* a_position;
    static EGRenderAttributeInfo* a_normal;
    static EGRenderAttributeInfo* a_color;
    static EGRenderAttributeInfo* a_texcoord0;
    static EGRenderAttributeInfo* a_texcoord1;
    static EGRenderUniformInfo* u_texture0;

public:
    EGTextRendererES2(EGText *obj);
    virtual ~EGTextRendererES2();
    
    virtual void link(EGRenderProgramPtr program);
    virtual void clear();
    virtual void update();
    virtual void draw();
};
