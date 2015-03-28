/*
 *  EGVertexArray.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGVertexArray_H
#define EGVertexArray_H

class EGVertexArray;
typedef std::shared_ptr<EGVertexArray> EGVertexArrayPtr;


/* EGVertexArray */

class EGVertexArray
{
public:
    int nvert;
    size_t vbufsize;
    char *vbuf;
    
    EGVertexArray() : nvert(0), vbufsize(1024), vbuf(new char[vbufsize]) {}
    
    ~EGVertexArray() { if (vbuf) delete [] vbuf; }
    
    inline void rewind() { nvert = 0; }
    
    inline void resizeCheck(size_t vsize)
    {
        if ((nvert + 1) * vsize > vbufsize) {
            char *tmp = vbuf;
            vbufsize *= 2;
            vbuf = new char[vbufsize];
            memcpy(vbuf, tmp, nvert * vsize);
            delete[] tmp;
        }
    }
    
    EGVertexIndex pushVertex(EGfloat x, EGfloat y, EGfloat u, EGfloat v)
    {
        const size_t vsize = sizeof(EGfloat) * 4;
        resizeCheck(vsize);
        EGuint ind = nvert++;
        char *vloc = vbuf + ind * vsize;
        memcpy(vloc, (char*)&x, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 1, (char*)&y, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 2, (char*)&u, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 3, (char*)&v, sizeof(EGfloat));
        return ind;
    }

    EGVertexIndex pushVertex(EGfloat x, EGfloat y, EGfloat z, EGfloat u, EGfloat v)
    {
        const size_t vsize = sizeof(EGfloat) * 4;
        resizeCheck(vsize);
        EGuint ind = nvert++;
        char *vloc = vbuf + ind * vsize;
        memcpy(vloc, (char*)&x, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 1, (char*)&y, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 2, (char*)&z, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 3, (char*)&u, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 4, (char*)&v, sizeof(EGfloat));
        return ind;
    }

    EGVertexIndex pushVertex(EGfloat x, EGfloat y, unsigned int rgba32)
    {
        const size_t vsize = sizeof(EGfloat) * 2 + sizeof(unsigned int);
        resizeCheck(vsize);
        EGuint ind = nvert++;
        char *vloc = vbuf + ind * vsize;
        memcpy(vloc, (char*)&x, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 1, (char*)&y, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 2, (char*)&rgba32, sizeof(unsigned int));
        return ind;
    }
        
    EGVertexIndex pushVertex(EGfloat x, EGfloat y, EGfloat z, unsigned int rgba32)
    {
        const size_t vsize = sizeof(EGfloat) * 3 + sizeof(unsigned int);
        resizeCheck(vsize);
        EGuint ind = nvert++;
        char *vloc = vbuf + ind * vsize;
        memcpy(vloc, (char*)&x, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 1, (char*)&y, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 2, (char*)&z, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 3, (char*)&rgba32, sizeof(unsigned int));
        return ind;
    }
    
    EGVertexIndex pushVertex(EGfloat x, EGfloat y, EGfloat u, EGfloat v, unsigned int rgba32)
    {
        const size_t vsize = sizeof(EGfloat) * 4 + sizeof(unsigned int);
        resizeCheck(vsize);
        EGuint ind = nvert++;
        char *vloc = vbuf + ind * vsize;
        memcpy(vloc, (char*)&x, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 1, (char*)&y, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 2, (char*)&u, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 3, (char*)&v, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 4, (char*)&rgba32, sizeof(unsigned int));
        return ind;
    }

    EGVertexIndex pushVertex(EGfloat x, EGfloat y, EGfloat z, EGfloat u, EGfloat v, unsigned int rgba32)
    {
        const size_t vsize = sizeof(EGfloat) * 5 + sizeof(unsigned int);
        resizeCheck(vsize);
        EGuint ind = nvert++;
        char *vloc = vbuf + ind * vsize;
        memcpy(vloc, (char*)&x, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 1, (char*)&y, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 2, (char*)&z, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 3, (char*)&u, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 4, (char*)&v, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 5, (char*)&rgba32, sizeof(unsigned int));
        return ind;
    }

    EGVertexIndex pushVertex(EGfloat x, EGfloat y, EGfloat z, EGfloat u, EGfloat v, EGfloat w, unsigned int rgba32)
    {
        const size_t vsize = sizeof(EGfloat) * 6 + sizeof(unsigned int);
        resizeCheck(vsize);
        EGuint ind = nvert++;
        char *vloc = vbuf + ind * vsize;
        memcpy(vloc, (char*)&x, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 1, (char*)&y, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 2, (char*)&z, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 3, (char*)&u, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 4, (char*)&v, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 5, (char*)&w, sizeof(EGfloat));
        memcpy(vloc + sizeof(EGfloat) * 6, (char*)&rgba32, sizeof(unsigned int));
        return ind;
    }
};

#endif
