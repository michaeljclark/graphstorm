// See LICENSE for license details.

#ifndef EGRenderBatchES2_H
#define EGRenderBatchES2_H

/* EGRenderBatchFlagStateES2 */

struct EGRenderBatchProgramES2 : EGRenderBatchState
{
    EGRenderProgramPtr program;
    
    EGRenderBatchProgramES2(EGRenderProgramPtr program)
        : program(program) {}
    
    void setupState()
    {
        EGRenderApiES2 &gl = EGRenderApiES2::apiImpl();
        gl.useProgram(program);
    }
    
    void clearState()
    {
    }
};

/* EGRenderBatchFlagStateES2 */

struct EGRenderBatchFlagStateES2 : EGRenderBatchState
{
    GLenum cap;
    EGbool enable;
    
    EGRenderBatchFlagStateES2(GLenum cap, EGbool enable)
        : cap(cap), enable(enable) {}
    
    void setupState()
    {
        if (enable) {
            glEnable(cap);
        } else {
            glDisable(cap);
        }
    }
    
    void clearState()
    {
        if (enable) {
            glDisable(cap);
        } else {
            glEnable(cap);
        }
    }
};


/* EGRenderBatchScissorStateES2 */

struct EGRenderBatchScissorStateES2 : EGRenderBatchState
{
    GLint x;
    GLint y;
    GLsizei width;
    GLsizei height;
    
    EGRenderBatchScissorStateES2(GLint x, GLint y, GLsizei width, GLsizei height)
        : x(x), y(y), width(width), height(height) {}
    
    void setupState()
    {
        glScissor(x, y, width, height);
    }
    
    void clearState()
    {
    }
};


/* EGRenderBatchUniform1iES2 */

struct EGRenderBatchUniform1iES2 : EGRenderBatchState
{
    GLint location;
    GLint v0;
    
    EGRenderBatchUniform1iES2(GLint location, GLint v0)
        : location(location), v0(v0) {}
    
    void setupState()
    {
        glUniform1i(location, v0);
    }
    
    void clearState()
    {
    }
};


/* EGRenderBatchAttribArrayStateES2 */

struct EGRenderBatchAttribArrayStateES2 : EGRenderBatchState
{
    GLuint index;
    GLint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    GLvoid *pointer;
    
    EGRenderBatchAttribArrayStateES2(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid *pointer)
        : index(index), size(size), type(type), normalized(normalized), stride(stride), pointer(pointer) {}
    
    void setupState()
    {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    }
    
    void clearState()
    {
        glDisableVertexAttribArray(index);
    }
};


/* EGRenderBatchBlendES2 */

struct EGRenderBatchBlendES2 : EGRenderBatchState
{
    GLenum sfactor;
    GLenum dfactor;
    
    EGRenderBatchBlendES2(GLenum sfactor, GLenum dfactor)
        : sfactor(sfactor), dfactor(dfactor) {}
    
    void setupState()
    {
        glEnable(GL_BLEND);
        glBlendFunc(sfactor, dfactor);
    }
    
    void clearState()
    {
        glDisable(GL_BLEND);
    }
};


/* EGRenderBatchTextureStateES2 */

struct EGRenderBatchTextureStateES2 : EGRenderBatchState
{
    GLenum target;
    GLuint texid;
    GLuint texnum;
    
    EGRenderBatchTextureStateES2(GLenum target, GLuint texid, GLuint texnum)
        : target(target), texid(texid), texnum(texnum) {}
    
    void setupState()
    {
        glActiveTexture(GL_TEXTURE0 + texnum);
        glBindTexture(target, texid);
    }
    
    void clearState()
    {
    }
};


/* EGRenderBatchLineWidthStateES2 */

struct EGRenderBatchLineWidthStateES2 : EGRenderBatchState
{
    GLfloat width;
    
    EGRenderBatchLineWidthStateES2(GLfloat width)
        : width(width) {}
    
    void setupState()
    {
        glLineWidth(width);
    }
    
    void clearState()
    {
    }
};


#ifndef __native_client__

/* EGRenderBatchPointSizeStateES2 */

struct EGRenderBatchPointSizeStateES2 : EGRenderBatchState
{
    GLfloat size;
    
    EGRenderBatchPointSizeStateES2(GLfloat size)
        : size(size) {}
    
    void setupState()
    {
        glPointSize(size);
    }
    
    void clearState()
    {
    }
};

#endif

#endif
