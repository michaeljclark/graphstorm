/*
 *  EGRenderApi.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGRenderApi_H
#define EGRenderApi_H


/* EGRenderApiVersion */

typedef enum {
    EGRenderApiVersionES2,
} EGRenderApiVersion;


/* EGRenderMatrix */

typedef enum {
    EGRenderMatrixModelView,
    EGRenderMatrixProjection,
    EGRenderMatrixModelViewProjection,
} EGRenderMatrix;


/* EGVertexBufferElement */

union EGVertexBufferElement {
    EGfloat f;
    EGuint u;
};


/* EGRenderApi */

class EGRenderApi {
protected:
    static EGint nextApiId;
    static EGRenderApi* currentApi;

    EGint apiId;
    const char *apiName;
    EGRenderApiVersion apiVersion;

    EGRenderApi(const char *apiName, EGRenderApiVersion apiVersion);

public:
    static EGRenderApi& currentApiImpl();
    static EGbool init(EGRenderApi &api);
    static EGbool initRenderApi(int glesVersion);
    
    virtual ~EGRenderApi();
    
    const char* getName() const;
    EGint getId() const;
    EGRenderApiVersion getVersion() const;
    EGbool isVersion(EGRenderApiVersion version) const;

    virtual EGbool init() = 0;
    virtual void destroy() = 0;
    
    // EGRenderApi methods
    virtual void loadMatrix(EGRenderMatrix which, EGfloat m[16]) = 0;
    virtual void multMatrix(EGRenderMatrix which, EGfloat m[16]) = 0;
    virtual void getMatrix(EGRenderMatrix which, EGfloat m[16]) = 0;
    virtual void updateUniforms() = 0;
};

#endif
