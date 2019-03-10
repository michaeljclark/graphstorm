// See LICENSE for license details.

#pragma once

#if USE_VERTEX_32
typedef EGuint EGVertexIndex;
#define EGINDEXARRAY_GLTYPE GL_UNSIGNED_INT
#else
typedef EGushort EGVertexIndex;
#define EGINDEXARRAY_GLTYPE GL_UNSIGNED_SHORT
#endif

#define EGINDEXARRAY_INITIAL_INDICE_SIZE 64

class EGIndexArray;
typedef std::shared_ptr<EGIndexArray> EGIndexArrayPtr;


/* EGIndexArray */

class EGIndexArray
{
public:
    int nind, sind;
    EGVertexIndex *ind;
    
    EGIndexArray() : nind(0), sind(EGINDEXARRAY_INITIAL_INDICE_SIZE), ind(new EGVertexIndex[sind]) {}
    
    ~EGIndexArray() { if (ind) delete [] ind; }
    
    size_t usedSize() { return sizeof(EGIndexArray) + sizeof(EGVertexIndex) * nind; }
    size_t totalSize() { return sizeof(EGIndexArray) + sizeof(EGVertexIndex) * sind; }
    EGuint count() { return nind; }
    
    void clear()
    {
        delete [] ind;
        ind = NULL;
        sind = nind = 0;
    }
    
    void rewind()
    {
        nind = 0;
    }
    
    void trim()
    {
        /* trim if the utilization is less than ~88% */
        if (sind > (nind + (nind >> 3))) {
            EGVertexIndex *tmp = ind;
            sind = nind;
            ind = new EGVertexIndex[sind];
            memcpy(ind, tmp, sizeof(EGVertexIndex) * sind);
            delete[] tmp;
        }
    }
    
    void pushIndices(EGVertexIndex *newind, int count)
    {
        /* resize index array if needed */
        if (nind + count >= sind) {
            EGVertexIndex *tmp = ind;
            int tmpsize = sind;
            sind *= 4;
            ind = new EGVertexIndex[sind];
            memcpy(ind, tmp, sizeof(EGVertexIndex) * tmpsize);
            delete[] tmp;
        }
        memcpy(&ind[nind], newind, sizeof(EGVertexIndex) * count);
        nind += count;
    }
};
