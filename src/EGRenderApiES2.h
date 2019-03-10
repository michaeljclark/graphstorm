// See LICENSE for license details.

#pragma once

class EGResource;
typedef std::shared_ptr<EGResource> EGResourcePtr;
typedef std::vector<EGResourcePtr> EGResourceList;

struct EGRenderUniformInfo;
typedef std::shared_ptr<EGRenderUniformInfo> EGRenderUniformInfoPtr;
struct EGRenderAttributeInfo;
typedef std::shared_ptr<EGRenderAttributeInfo> EGRenderAttributeInfoPtr;
class EGRenderProgram;
typedef std::shared_ptr<EGRenderProgram> EGRenderProgramPtr;
typedef std::map<EGstring,EGRenderProgramPtr> EGRenderProgramMap;

/* EGRenderUniformInfo */

struct EGRenderUniformInfo
{
    std::string name;
    EGuint location;
    EGenum type;
    EGint size;
    
    EGRenderUniformInfo() {}
    EGRenderUniformInfo(const std::string &name, EGuint location, EGenum type, EGint size)
        : name(name), location(location), type(type), size(size) {}
    
    static const char* typeName(EGenum type);
};


/* EGRenderAttributeInfo */

struct EGRenderAttributeInfo
{
    std::string name;
    EGint index;
    EGenum type;
    
    EGRenderAttributeInfo(std::string name, EGint index, EGenum type) : name(name), index(index), type(type) {}
};

typedef std::shared_ptr<EGRenderAttributeInfo> EGRenderAttributeInfoPtr;


/* EGRenderProgram */

class EGRenderProgram
{
protected:
    static const EGbool debug = false;

    EGResourceList vertShaderRsrcs;
    EGResourceList fragShaderRsrcs;

    void init();
    EGbool loadShaders();
    EGint compileShader(GLuint *shader, GLenum type, EGResourceList shaderRsrcs);
    EGint linkProgram(GLuint prog);
    EGint validateProgram(GLuint prog);
    void destroyShaders(GLuint vertShader, GLuint fragShader, GLuint prog);
    
public:
    EGRenderProgram(EGResourceList vertShaderRsrcs, EGResourceList fragShaderRsrcs);
    EGRenderProgram(EGResourcePtr vertShaderRsrc, EGResourcePtr fragShaderRsrc);
    ~EGRenderProgram();
    
    EGRenderAttributeInfo* getAttribute(std::string name)
    {
        std::map<std::string,EGRenderAttributeInfoPtr>::iterator ai = attributeNameMap.find(name);
        if (ai != attributeNameMap.end()) {
            return (*ai).second.get();
        } else {
            return NULL;
        }
    }

    EGRenderUniformInfo* getUniform(std::string name)
    {
        std::map<std::string,EGRenderUniformInfoPtr>::iterator ui = uniformNameMap.find(name);
        if (ui != uniformNameMap.end()) {
            return (*ui).second.get();
        } else {
            return NULL;
        }
    }

    EGint program;
    EGint numuniforms;
    std::map<std::string,EGRenderAttributeInfoPtr> attributeNameMap;
    std::map<std::string,EGRenderUniformInfoPtr> uniformNameMap;
};


/* EGRenderApiES2 */

class EGRenderApiES2 : public EGRenderApi
{
public:
    static EGRenderApiES2& apiImpl();

    static EGRenderProgramMap programcache;
    static EGRenderProgramPtr activeprogram;
    
    EGRenderApiES2();

    virtual EGbool init();
    virtual void destroy();

    // EGRenderApi methods
    virtual void loadMatrix(EGRenderMatrix which, EGfloat m[16]);
    virtual void multMatrix(EGRenderMatrix which, EGfloat m[16]);
    virtual void getMatrix(EGRenderMatrix which, EGfloat m[16]);
    void updateUniforms();
    
    // EGRenderApiES2 specific methods
    EGstring getRenderProgramKey(EGResourceList vertShaderRsrcs, EGResourceList fragShaderRsrcs);
    EGRenderProgramPtr getRenderProgram(EGResourceList vertShaderRsrcs, EGResourceList fragShaderRsrcs);
    EGRenderProgramPtr getRenderProgram(EGResourcePtr vertShaderRsrc, EGResourcePtr fragShaderRsrc);
    void useProgram(EGRenderProgramPtr &program);
            
protected:
    EGfloat modelViewMatrix[16];
    EGfloat projectionMatrix[16];
    EGfloat modelViewProjectionMatrix[16];
    EGfloat inverseTransposeModelViewMatrix[16];
    EGfloat normalMatrix[9];
    EGbool matricesNeedUpdate;
};
