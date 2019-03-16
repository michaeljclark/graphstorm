// See LICENSE for license details.

#include "EG.h"
#include "EGGL.h"
#include "EGMath.h"
#include "EGResource.h"
#include "EGRenderApi.h"
#include "EGRenderApiES2.h"
#include "EGText.h"
#include "EGTextRenderer.h"
#include "EGTextRendererES2.h"

/* EGTextRendererES2 */

EGRenderProgramPtr EGTextRendererES2::program;
EGRenderAttributeInfo* EGTextRendererES2::a_position;
EGRenderAttributeInfo* EGTextRendererES2::a_normal;
EGRenderAttributeInfo* EGTextRendererES2::a_color;
EGRenderAttributeInfo* EGTextRendererES2::a_texcoord0;
EGRenderUniformInfo* EGTextRendererES2::u_texture0;

EGTextRendererES2::EGTextRendererES2(EGText *obj) : obj(obj), texId(0), vbo(0), vao(0)
{
    EGRenderApiES2 &gl = EGRenderApiES2::apiImpl();
    if (!program) {
        link(gl.getRenderProgram(EGResource::getResource("Resources/gles2.bundle/Shaders/Unified110.vsh"),
                                 EGResource::getResource("Resources/gles2.bundle/Shaders/Unified110.fsh")));
    }
}

EGTextRendererES2::~EGTextRendererES2()
{
    clear();
}

void EGTextRendererES2::link(EGRenderProgramPtr program)
{
    EGTextRendererES2::program = program;
    a_position = program->getAttribute("a_position");
    a_normal = program->getAttribute("a_normal");
    a_color = program->getAttribute("a_color");
    a_texcoord0 = program->getAttribute("a_texcoord0");
    u_texture0 = program->getUniform("u_texture0");
}

void EGTextRendererES2::clear()
{
    if (vbo) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (texId) {
        glDeleteTextures(1, &texId);
        texId = 0;
    }
}

void EGTextRendererES2::update()
{
    static const GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_RED};

    if (obj->updated) return;
    
    // regenerate texture
    if (obj->prepare() || !texId) {
        if (!texId) {
            glGenTextures(1, &texId);
            glBindTexture(GL_TEXTURE_2D, texId);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#if defined(__APPLE__)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, obj->texWidth, obj->texHeight, 0, GL_ALPHA, GL_UNSIGNED_BYTE, (char*)obj->texData);
#else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, obj->texWidth, obj->texHeight, 0, GL_RED, GL_UNSIGNED_BYTE, (char*)obj->texData);
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
#endif
        } else {
            glBindTexture(GL_TEXTURE_2D, texId);
#if defined(__APPLE__)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, obj->texWidth, obj->texHeight, 0, GL_ALPHA, GL_UNSIGNED_BYTE, (char*)obj->texData);
#else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, obj->texWidth, obj->texHeight, 0, GL_RED, GL_UNSIGNED_BYTE, (char*)obj->texData);
#endif
        }
    }
    
    // create vertices array
    EGVertexBufferElement *vertices;
    if (obj->flags & EGTextEmbossed) {
        vertices = new EGVertexBufferElement[40];
    } else {
        vertices = new EGVertexBufferElement[20];
    }

    const EGint tw = obj->texWidth;
    const EGint th = obj->texHeight;
    const EGfloat rscale = 1.0f / obj->scale;
    const EGint xl = (EGint)((obj->flags & EGTextHAlignCenter ? tw / 2 : (obj->flags & EGTextHAlignRight ? tw : 0)) * rscale);
    const EGint xh = (EGint)((obj->flags & EGTextHAlignCenter ? tw / 2 : (obj->flags & EGTextHAlignLeft ? tw : 0)) * rscale);
    const EGint yl = (EGint)((obj->flags & EGTextVAlignCenter ? th / 2 : (obj->flags & EGTextVAlignBase ? th : 0)) * rscale);
    const EGint yh = (EGint)((obj->flags & EGTextVAlignCenter ? th / 2 : (obj->flags & EGTextVAlignTop ? th : 0)) * rscale);
    const EGfloat x = obj->x;
    const EGfloat y = obj->y;
    const EGfloat angle = obj->angle;
    const EGuint color_rgba32 = EGColor(obj->color[0], obj->color[1], obj->color[2], obj->color[3]).rgba32();
    const EGuint emboss_rgba32 = EGColor(obj->embosscolor[0], obj->embosscolor[1], obj->embosscolor[2], obj->embosscolor[3]).rgba32();
    
    EGVertexBufferElement *v = vertices;
    if (angle == 0) {
        (*v++).f = x - xl; (*v++).f = y - yl; (*v++).f = 0; (*v++).f = 1; (*v++).u = color_rgba32;
        (*v++).f = x - xl; (*v++).f = y + yh; (*v++).f = 0; (*v++).f = 0; (*v++).u = color_rgba32;
        (*v++).f = x + xh; (*v++).f = y - yl; (*v++).f = 1; (*v++).f = 1; (*v++).u = color_rgba32;
        (*v++).f = x + xh; (*v++).f = y + yh; (*v++).f = 1; (*v++).f = 0; (*v++).u = color_rgba32;
    } else if (angle == 90) {
        (*v++).f = x - yl; (*v++).f = y - xl; (*v++).f = 0; (*v++).f = 0; (*v++).u = color_rgba32;
        (*v++).f = x - yl; (*v++).f = y + xh; (*v++).f = 1; (*v++).f = 0; (*v++).u = color_rgba32;
        (*v++).f = x + yh; (*v++).f = y - xl; (*v++).f = 0; (*v++).f = 1; (*v++).u = color_rgba32;
        (*v++).f = x + yh; (*v++).f = y + xh; (*v++).f = 1; (*v++).f = 1; (*v++).u = color_rgba32;
    } else if (angle == 180) {
        (*v++).f = x - xl; (*v++).f = y - yl; (*v++).f = 1; (*v++).f = 0; (*v++).u = color_rgba32;
        (*v++).f = x - xl; (*v++).f = y + yh; (*v++).f = 1; (*v++).f = 1; (*v++).u = color_rgba32;
        (*v++).f = x + xh; (*v++).f = y - yl; (*v++).f = 0; (*v++).f = 0; (*v++).u = color_rgba32;
        (*v++).f = x + xh; (*v++).f = y + yh; (*v++).f = 0; (*v++).f = 1; (*v++).u = color_rgba32;
    } else if (angle == 270) {
        (*v++).f = x - yl; (*v++).f = y - xl; (*v++).f = 1; (*v++).f = 1; (*v++).u = color_rgba32;
        (*v++).f = x - yl; (*v++).f = y + xh; (*v++).f = 0; (*v++).f = 1; (*v++).u = color_rgba32;
        (*v++).f = x + yh; (*v++).f = y - xl; (*v++).f = 1; (*v++).f = 0; (*v++).u = color_rgba32;
        (*v++).f = x + yh; (*v++).f = y + xh; (*v++).f = 0; (*v++).f = 0; (*v++).u = color_rgba32;
    } else {
        EGfloat sa = sinf(angle * EGMath::Deg2Rad);
        EGfloat ca = cosf(angle * EGMath::Deg2Rad);
        (*v++).f = x - ca * xl + sa * yl; (*v++).f = y - ca * yl - sa * xl; (*v++).f = 0; (*v++).f = 1; (*v++).u = color_rgba32;
        (*v++).f = x - ca * xl - sa * yh; (*v++).f = y + ca * yh - sa * xl; (*v++).f = 0; (*v++).f = 0; (*v++).u = color_rgba32;
        (*v++).f = x + ca * xh + sa * yl; (*v++).f = y - ca * yl + sa * xh; (*v++).f = 1; (*v++).f = 1; (*v++).u = color_rgba32;
        (*v++).f = x + ca * xh - sa * yh; (*v++).f = y + ca * yh + sa * xh; (*v++).f = 1; (*v++).f = 0; (*v++).u = color_rgba32;
    }
    
    if (obj->flags & EGTextEmbossed) {
        EGVertexBufferElement *vsrc = vertices;
        EGVertexBufferElement *vdest = vertices + 20;
        for (int i = 0; i < 4; i++) {
            (*vdest++).f = (*vsrc++).f + 1;
            (*vdest++).f = (*vsrc++).f + 1;
            (*vdest++).f = (*vsrc++).f;
            (*vdest++).f = (*vsrc++).f;
            (*vdest++).u = emboss_rgba32; vsrc++;
        }
    }

#if USE_VAO
    // create vertex array
    if (!vao) {
        glGenVertexArrays(1, &vao);
    }
    glBindVertexArray(vao);
#endif

    // create and update vertex buffer
    if (!vbo) {
        glGenBuffers(1, &vbo);
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, obj->flags & EGTextEmbossed ? 160 : 80, vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // set up vertex array
#if USE_VAO
    glEnableVertexAttribArray(a_position->index);
    glEnableVertexAttribArray(a_texcoord0->index);
    glEnableVertexAttribArray(a_color->index);
    glVertexAttribPointer(a_position->index, 2, GL_FLOAT, 0, 20, 0);
    glVertexAttribPointer(a_texcoord0->index, 2, GL_FLOAT, 0, 20, (void*)8);
    glVertexAttribPointer(a_color->index, 4, GL_UNSIGNED_BYTE, 1, 20, (void*)16);
    glBindVertexArray(0);
#endif
    
    delete [] vertices;

    obj->updated = true;
}

void EGTextRendererES2::draw()
{
    // use no lighting texture alpha program
    EGRenderApiES2 &gl = EGRenderApiES2::apiImpl();
    gl.useProgram(program);
    
    // bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);
    glUniform1i(u_texture0->location, 0);

#if USE_VAO
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
#else
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(a_position->index);
    glEnableVertexAttribArray(a_texcoord0->index);
    glEnableVertexAttribArray(a_color->index);
    glVertexAttribPointer(a_position->index, 2, GL_FLOAT, 0, 20, 0);
    glVertexAttribPointer(a_texcoord0->index, 2, GL_FLOAT, 0, 20, (void*)8);
    glVertexAttribPointer(a_color->index, 4, GL_UNSIGNED_BYTE, 1, 20, (void*)16);
#endif

    if (obj->flags & EGTextEmbossed) {
        glVertexAttrib4f(a_color->index, obj->embosscolor[0], obj->embosscolor[1], obj->embosscolor[2], obj->embosscolor[3]);
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
    }
    
    glVertexAttrib4f(a_color->index, obj->color[0], obj->color[1], obj->color[2], obj->color[3]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

#if USE_VAO
    glBindVertexArray(0);
#else
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}
