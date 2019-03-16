// See LICENSE for license details.

#pragma once

#define USE_VAO 1

struct EGRenderBatchBase;
typedef std::shared_ptr<EGRenderBatchBase> EGRenderBatchPtr;
typedef std::vector<EGRenderBatchPtr> EGRenderBatchList;

struct EGRenderBatchState;
typedef std::vector<EGRenderBatchState*> EGRenderBatchStateList;


/* EGRenderBatchState */

struct EGRenderBatchState
{
    bool is_attrib;

    EGRenderBatchState(bool is_attrib) : is_attrib(is_attrib) {}
    virtual ~EGRenderBatchState() {}
    
    virtual void setupState() = 0;
    virtual void clearState() = 0;
};


/* EGRenderBatchType */

enum EGRenderBatchDrawType
{
    EGRenderBatchDrawArrays,
    EGRenderBatchDrawElements,
    EGRenderBatchStateOnly,
};


/* EGRenderBatchMode */

enum EGRenderBatchBufferType
{
    EGRenderBatchCreateBuffer,
    EGRenderBatchExternalBuffer,
};


/* EGRenderBatchFlags */

enum EGRenderBatchFlags
{
    EGRenderBatchFlagsDrawArrays     = 0x01,
    EGRenderBatchFlagsDrawElements   = 0x02,
    EGRenderBatchFlagsStateOnly      = 0x04,
    EGRenderBatchFlagsCreateBuffer   = 0x08,
    EGRenderBatchFlagsExternalBuffer = 0x10,
    EGRenderBatchFlagsBuffersDirty   = 0x20,
    EGRenderBatchFlagsPreserveArrays = 0x40
};

/* EGRenderBatchBase */

struct EGRenderBatchBase
{
    virtual void draw() = 0;
};

/* EGRenderBatch */

struct EGRenderBatch : EGRenderBatchBase
{
    static const EGbool debug = false;
    
    std::string batchName;
    GLenum mode;
    EGenum flags;
    size_t count;
    size_t vertexSize;
    size_t vboSize;
    size_t iboSize;
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    EGVertexArrayPtr varr;
    EGIndexArrayPtr iarr;
    EGRenderBatchStateList statelist;
    
    EGRenderBatch(std::string batchName)
        : batchName(batchName), mode(0), flags(EGRenderBatchFlagsCreateBuffer), count(0), vertexSize(0), vboSize(0), iboSize(0), vao(0), vbo(0), ibo(0), varr(), iarr()
    {
    }
    
    virtual ~EGRenderBatch()
    {
        clearState();
        clearBuffers();
    }
    
    void rewind()
    {
        if (flags & EGRenderBatchFlagsCreateBuffer) {
            if (flags & EGRenderBatchFlagsDrawArrays) {
                if (!varr) {
                    varr = EGVertexArrayPtr(new EGVertexArray());
                } else {
                    varr->rewind();
                }
                vboSize = iboSize = 0;
                count = 0;
            }
            if (flags & EGRenderBatchFlagsDrawElements) {
                if (!varr) {
                    varr = EGVertexArrayPtr(new EGVertexArray());
                } else {
                    varr->rewind();
                }
                if (!iarr) {
                    iarr = EGIndexArrayPtr(new EGIndexArray());
                } else {
                    iarr->rewind();
                }
                vboSize = iboSize = 0;
                count = 0;
            }
        }
        flags |= EGRenderBatchFlagsBuffersDirty;
    }
    
    void clearBuffers()
    {
        if (flags & EGRenderBatchFlagsCreateBuffer) {
            if (vbo) {
                glDeleteBuffers(1, &vbo);
                if (debug) {
                    EGDebug("%s deleted vertex buffer %d\n", batchName.c_str(), vbo);
                }
                vbo = 0;
            }
            if (ibo) {
                glDeleteBuffers(1, &ibo);
                if (debug) {
                    EGDebug("%s deleted vertex index buffer %d\n", batchName.c_str(), ibo);
                }
                ibo = 0;
            }
        }
    }
    
    EGRenderBatch& clearState()
    {
        for (EGRenderBatchStateList::iterator si = statelist.begin(); si != statelist.end(); si++) {
            delete *si;
        }
        statelist.clear();
        return *this;
    }
    
    EGRenderBatch& setMode(GLenum mode)
    {
        this->mode = mode;
        return *this;
    }
    
    EGRenderBatch& setDrawType(EGRenderBatchDrawType drawtype)
    {
        switch (drawtype) {
            case EGRenderBatchDrawArrays:
                flags &= ~EGRenderBatchFlagsStateOnly;
                flags &= ~EGRenderBatchFlagsDrawElements;
                flags |= EGRenderBatchFlagsDrawArrays;
                break;
            case EGRenderBatchDrawElements:
                flags &= ~EGRenderBatchFlagsStateOnly;
                flags &= ~EGRenderBatchFlagsDrawArrays;
                flags |= EGRenderBatchFlagsDrawElements;
                break;
            case EGRenderBatchStateOnly:
                flags &= ~EGRenderBatchFlagsDrawArrays;
                flags &= ~EGRenderBatchFlagsDrawElements;
                flags |= EGRenderBatchFlagsStateOnly;
                break;
        }
        if (flags & EGRenderBatchFlagsCreateBuffer) {
            switch (drawtype) {
                case EGRenderBatchDrawArrays:
                    varr = EGVertexArrayPtr(new EGVertexArray());
                    iarr = EGIndexArrayPtr();
                    count = 0;
                    break;
                case EGRenderBatchDrawElements:
                    varr = EGVertexArrayPtr(new EGVertexArray());
                    iarr = EGIndexArrayPtr(new EGIndexArray());
                    count = 0;
                default:
                    break;
            }
        } else {
            varr = EGVertexArrayPtr();
            iarr = EGIndexArrayPtr();
        }
        return *this;
    }

    EGRenderBatch& setVertexSize(size_t vertexSize)
    {
        this->vertexSize = vertexSize;
        return *this;
    }

    EGRenderBatch& setVertexBuffers(EGRenderBatchDrawType drawtype, size_t count, GLuint vbo)
    {
        clearBuffers();
        assert(drawtype == EGRenderBatchDrawArrays);
        flags &= ~EGRenderBatchFlagsCreateBuffer;
        flags |= EGRenderBatchFlagsExternalBuffer;
        setDrawType(drawtype);
        this->count = count;
        this->vbo = vbo;
        this->ibo = 0;
        this->vertexSize = 0;
        this->vboSize = 0;
        this->iboSize = 0;
        return *this;
    }
    
    EGRenderBatch& setVertexBuffers(EGRenderBatchDrawType drawtype, size_t count, GLuint vbo, GLuint ibo)
    {
        clearBuffers();
        assert(drawtype == EGRenderBatchDrawElements);
        flags &= ~EGRenderBatchFlagsCreateBuffer;
        flags |= EGRenderBatchFlagsExternalBuffer;
        setDrawType(drawtype);
        this->count = count;
        this->vbo = vbo;
        this->ibo = ibo;
        this->vertexSize = 0;
        this->vboSize = 0;
        this->iboSize = 0;
        return *this;
    }

    EGRenderBatch& addState(EGRenderBatchState *state)
    {
        statelist.push_back(state);
        return *this;
    }
    
    void createBuffers()
    {
#if USE_VAO
        bool bound = false;
        if (!vao) {
            glGenVertexArrays(1, &vao);
        }
#endif

        if (varr && (!vbo || flags & EGRenderBatchFlagsBuffersDirty) && flags & EGRenderBatchFlagsCreateBuffer) {
            vboSize = vertexSize * varr->nvert;
#if USE_VAO
            if (!bound) {
                glBindVertexArray(vao);
                bound = true;
            }
#endif
            if (!vbo) {
                glGenBuffers(1, &vbo);
                if (debug) {
                    EGDebug("%s created vertex buffer %d\n", batchName.c_str(), vbo);
                }
            }
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, vboSize, varr->vbuf, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            if (flags & EGRenderBatchFlagsDrawArrays) {
                count = varr->nvert;
            }
            if (!(flags & EGRenderBatchFlagsPreserveArrays)) {
                varr = EGVertexArrayPtr();
            }
        }
        if (iarr && (!ibo || flags & EGRenderBatchFlagsBuffersDirty) && flags & EGRenderBatchFlagsCreateBuffer) {
            iboSize = sizeof(EGVertexIndex) * iarr->nind;
#if USE_VAO
            if (!bound) {
                glBindVertexArray(vao);
                bound = true;
            }
#endif
            if (!ibo) {
                glGenBuffers(1, &ibo);
                if (debug) {
                    EGDebug("%s created vertex index buffer %d\n", batchName.c_str(), ibo);
                }
            }
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, iboSize, iarr->ind, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            if (flags & EGRenderBatchFlagsDrawElements) {
                count = iarr->nind;
            }
            if (!(flags & EGRenderBatchFlagsPreserveArrays)) {
                iarr = EGIndexArrayPtr();
            }
        }
#if USE_VAO
        if (bound) {
            for (EGRenderBatchStateList::iterator si = statelist.begin(); si != statelist.end(); si++) {
                if ((*si)->is_attrib) {
                    (*si)->setupState();
                }
            }
        }
#endif
        flags &= ~EGRenderBatchFlagsBuffersDirty;

#if USE_VAO
        if (bound) {
            glBindVertexArray(0);
        }
#endif
    }

    void setupState()
    {
        for (EGRenderBatchStateList::iterator si = statelist.begin(); si != statelist.end(); si++) {
            if (!USE_VAO || !(*si)->is_attrib) {
                (*si)->setupState();
            }
        }
    }
    
    void teardownState()
    {
        for (EGRenderBatchStateList::iterator si = statelist.begin(); si != statelist.end(); si++) {
            if (!USE_VAO || !(*si)->is_attrib) {
                (*si)->clearState();
            }
        }
    }
    
    void prepare()
    {
        createBuffers();
    }
    
    void draw()
    {
        if (flags & EGRenderBatchFlagsStateOnly) {
            setupState();
            return;
        }
        createBuffers();
        if (flags & EGRenderBatchFlagsDrawArrays) {
            setupState();
#if USE_VAO
            glBindVertexArray(vao);
#endif
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glDrawArrays(mode, 0, (GLsizei)count);
#if USE_VAO
            glBindVertexArray(0);
#endif
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            teardownState();
        }
        if (flags & EGRenderBatchFlagsDrawElements) {
            setupState();
#if USE_VAO
            glBindVertexArray(vao);
#endif
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glDrawElements(mode, (GLsizei)count, EGINDEXARRAY_GLTYPE, (void*)0);
#if USE_VAO
            glBindVertexArray(0);
#endif
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            teardownState();
        }
    }
};
