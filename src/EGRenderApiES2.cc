// See LICENSE for license details.

#include "EG.h"
#include "EGGL.h"
#include "EGMath.h"
#include "EGText.h"
#include "EGImage.h"
#include "EGResource.h"
#include "EGRenderApi.h"
#include "EGRenderApiES2.h"


/* EGRenderUniformInfo */

const char* EGRenderUniformInfo::typeName(EGenum type)
{
    switch (type) {
        case GL_FLOAT:        return "GL_FLOAT";        break;
        case GL_FLOAT_VEC2:   return "GL_FLOAT_VEC2";   break;
        case GL_FLOAT_VEC3:   return "GL_FLOAT_VEC3";   break;
        case GL_FLOAT_VEC4:   return "GL_FLOAT_VEC4";   break;
        case GL_INT:          return "GL_INT";          break;
        case GL_INT_VEC2:     return "GL_INT_VEC2";     break;
        case GL_INT_VEC3:     return "GL_INT_VEC3";     break;
        case GL_INT_VEC4:     return "GL_INT_VEC4";     break;
        case GL_BOOL:         return "GL_BOOL";         break;
        case GL_BOOL_VEC2:    return "GL_BOOL_VEC2";    break;
        case GL_BOOL_VEC3:    return "GL_BOOL_VEC3";    break;
        case GL_BOOL_VEC4:    return "GL_BOOL_VEC4";    break;
        case GL_FLOAT_MAT2:   return "GL_FLOAT_MAT2";   break;
        case GL_FLOAT_MAT3:   return "GL_FLOAT_MAT3";   break;
        case GL_FLOAT_MAT4:   return "GL_FLOAT_MAT4";   break;
        case GL_SAMPLER_2D:   return "GL_SAMPLER_2D";   break;
        case GL_SAMPLER_CUBE: return "GL_SAMPLER_CUBE"; break;
        default: return "UNKNOWN";
    }
}

/* EGRenderApiES2 */

static const char class_name[] = "EGRenderApiES2";

EGRenderProgramMap EGRenderApiES2::programcache;
EGRenderProgramPtr EGRenderApiES2::activeprogram;

EGRenderApiES2& EGRenderApiES2::apiImpl()
{
    static EGRenderApiES2 apiImplSingleton;
    return apiImplSingleton;
}

EGRenderApiES2::EGRenderApiES2() : EGRenderApi(class_name, EGRenderApiVersionES2), matricesNeedUpdate(true) {}

EGbool EGRenderApiES2::init()
{
    /* set initial values for modelView and projection matrices */
    EGMath::identityMat4f(modelViewMatrix);
    EGMath::identityMat4f(projectionMatrix);
    
    return true;
}

void EGRenderApiES2::destroy()
{
}


// EGRenderApi methods

void EGRenderApiES2::loadMatrix(EGRenderMatrix which, EGfloat m[16])
{
    switch (which) {
        case EGRenderMatrixModelView:
            memcpy(modelViewMatrix, m, sizeof(modelViewMatrix));
            break;
        case EGRenderMatrixProjection:
            memcpy(projectionMatrix, m, sizeof(projectionMatrix));
            break;
        case EGRenderMatrixModelViewProjection:
            EGMath::identityMat4f(projectionMatrix);
            memcpy(modelViewMatrix, m, sizeof(modelViewMatrix));
            break;
    }
    matricesNeedUpdate = true;
}

void EGRenderApiES2::multMatrix(EGRenderMatrix which, EGfloat m[16])
{
    switch (which) {
        case EGRenderMatrixModelView:
            EGMath::multMat4f(modelViewMatrix, m);
            break;
        case EGRenderMatrixProjection:
            EGMath::multMat4f(projectionMatrix, m);
            break;
        case EGRenderMatrixModelViewProjection:
            EGMath::identityMat4f(projectionMatrix);
            EGMath::multMat4f(modelViewMatrix, m);
            break;
    }
    matricesNeedUpdate = true;
}

void EGRenderApiES2::getMatrix(EGRenderMatrix which, EGfloat m[16])
{
    switch (which) {
        case EGRenderMatrixModelView:
            memcpy(m, modelViewMatrix, sizeof(modelViewMatrix));
            break;
        case EGRenderMatrixProjection:
            memcpy(m, projectionMatrix, sizeof(modelViewMatrix));
            break;
        case EGRenderMatrixModelViewProjection:
            memcpy(m, modelViewProjectionMatrix, sizeof(modelViewMatrix));
            break;
    }
}

void EGRenderApiES2::updateUniforms()
{
    if (!activeprogram) return;
    
    if (matricesNeedUpdate) {
        /* calculate modelViewProjectionMatrix */
        EGMath::multMat4fMat4f(modelViewProjectionMatrix, modelViewMatrix, projectionMatrix);
        
        /* calculate normal matrix for lighting */
        EGMath::invertMat4f(inverseTransposeModelViewMatrix, modelViewMatrix);
        EGMath::transposeMat4f(inverseTransposeModelViewMatrix);
        EGMath::mat4toMat3f(normalMatrix, inverseTransposeModelViewMatrix);
        matricesNeedUpdate = false;
    }
    
    EGRenderUniformInfo *u_modelViewMatrix = activeprogram->getUniform("u_modelViewMatrix");
    EGRenderUniformInfo *u_modelViewProjectionMatrix = activeprogram->getUniform("u_modelViewProjectionMatrix");
    EGRenderUniformInfo *u_normalMatrix = activeprogram->getUniform("u_normalMatrix");
    
    if (u_modelViewMatrix) glUniformMatrix4fv(u_modelViewMatrix->location, 1, GL_FALSE, modelViewMatrix);
    if (u_modelViewProjectionMatrix) glUniformMatrix4fv(u_modelViewProjectionMatrix->location, 1, GL_FALSE, modelViewProjectionMatrix);
    if (u_normalMatrix) glUniformMatrix3fv(u_normalMatrix->location, 1, GL_FALSE, normalMatrix);
}

EGstring EGRenderApiES2::getRenderProgramKey(EGResourceList vertShaderRsrcs, EGResourceList fragShaderRsrcs)
{
    std::stringstream ss;
    for(EGResourceList::iterator ri = vertShaderRsrcs.begin(); ri != vertShaderRsrcs.end(); ri++) {
        if (ri != vertShaderRsrcs.begin()) ss << ",";
        ss << (*ri)->getPath();
    }
    ss << ":";
    for(EGResourceList::iterator ri = fragShaderRsrcs.begin(); ri != fragShaderRsrcs.end(); ri++) {
        if (ri != fragShaderRsrcs.begin()) ss << ",";
        ss << (*ri)->getPath();
    }
    return ss.str();
}

EGRenderProgramPtr EGRenderApiES2::getRenderProgram(EGResourceList vertShaderRsrcs, EGResourceList fragShaderRsrcs)
{
    EGstring programkey = getRenderProgramKey(vertShaderRsrcs, fragShaderRsrcs);
    EGRenderProgramMap::iterator pi = programcache.find(programkey);
    if (pi == programcache.end()) {
        EGRenderProgramPtr program(new EGRenderProgram(vertShaderRsrcs, fragShaderRsrcs));
        programcache.insert(std::pair<EGstring,EGRenderProgramPtr>(programkey, program));
        return program;
    } else {
        return (*pi).second;
    }
}

EGRenderProgramPtr EGRenderApiES2::getRenderProgram(EGResourcePtr vertShaderRsrc, EGResourcePtr fragShaderRsrc)
{
    EGResourceList vertShaderRsrcs;
    EGResourceList fragShaderRsrcs;
    vertShaderRsrcs.push_back(vertShaderRsrc);
    fragShaderRsrcs.push_back(fragShaderRsrc);
    return getRenderProgram(vertShaderRsrcs, fragShaderRsrcs);
}

void EGRenderApiES2::useProgram(EGRenderProgramPtr &program)
{
    if (activeprogram != program) {
        activeprogram = program;
        glUseProgram(program->program);
        updateUniforms();
    }
}


/* EGRenderProgram */
 
EGRenderProgram::EGRenderProgram(EGResourceList vertShaderRsrcs, EGResourceList fragShaderRsrcs)
    : vertShaderRsrcs(vertShaderRsrcs), fragShaderRsrcs(fragShaderRsrcs), numuniforms(0)
{
    init();
}

EGRenderProgram::EGRenderProgram(EGResourcePtr vertShaderRsrc, EGResourcePtr fragShaderRsrc)
    : vertShaderRsrcs(), fragShaderRsrcs(), numuniforms(0)
{
    vertShaderRsrcs.push_back(vertShaderRsrc);
    fragShaderRsrcs.push_back(fragShaderRsrc);
    init();
}

EGRenderProgram::~EGRenderProgram()
{
    glDeleteProgram(program);
    program = 0;
}

void EGRenderProgram::init()
{
    EGint maxVertexUniformVectors = 0;
#if defined (GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB)
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB, &maxVertexUniformVectors);
#elif defined (GL_MAX_VERTEX_UNIFORM_VECTORS)
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertexUniformVectors);
#endif
    
    if (!loadShaders()) {
        EGError("%s:%s failed to load shaders\n", class_name, __func__);
    }
}

/* initialize and load shaders */
EGbool EGRenderProgram::loadShaders()
{
    GLuint vertShader = 0, fragShader = 0;
	
	// create shader program
	program = glCreateProgram();
	
	// create and compile vertex shader
	if (!compileShader(&vertShader, GL_VERTEX_SHADER, vertShaderRsrcs)) {
		destroyShaders(vertShader, fragShader, program);
		return false;
	}
	
	// create and compile fragment shader
	if (!compileShader(&fragShader, GL_FRAGMENT_SHADER, fragShaderRsrcs)) {
		destroyShaders(vertShader, fragShader, program);
		return false;
	}
	
	// attach vertex shader to program
	glAttachShader(program, vertShader);
	
	// attach fragment shader to program
	glAttachShader(program, fragShader);
	
#if 0
	// bind VertexAttrib locations
	// this needs to be done prior to linking
    for (int attrNum = 0; attrNum < EGRenderVertexAttribCount; attrNum++) {
        glBindAttribLocation(program, attrNum, EGRenderApiES2::vertexAttribNames[attrNum]);
    }
    
    // bind custom attrs
    for (EGRenderVertexAttribList::iterator ai = customAttrs.begin(); ai != customAttrs.end(); ai++) {
        EGRenderVertexAttrib &attrib = *ai;
        glBindAttribLocation(program, attrib.position, attrib.name.c_str());
    }
#endif
	
	// link program
	if (!linkProgram(program)) {
		destroyShaders(vertShader, fragShader, program);
		return false;
	}
    
    // get active attribute names
    EGint numattrs = 0;
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numattrs);
    for (int i = 0; i < numattrs; i++)  {
        //glGetActiveAttrib(program, i,  GLsizei  bufSize,  GLsizei * length,  GLint * size,  GLenum * type,  GLchar * name);
        EGint namelen=-1, size=-1;
        GLenum type = GL_ZERO;
        char namebuf[128];
        glGetActiveAttrib(program, GLuint(i), sizeof(namebuf)-1, &namelen, &size, &type, namebuf);
        EGRenderAttributeInfoPtr attributeinfo(new EGRenderAttributeInfo(namebuf, i, type));
        attributeNameMap.insert(std::pair<std::string,EGRenderAttributeInfoPtr>(namebuf, attributeinfo));
    }
	
	// get active uniform names
	// this needs to be done after linking
    numuniforms = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numuniforms);
    uniformNameMap.clear();
    for (int i = 0; i < numuniforms; i++) {
        EGint namelen=-1, size=-1;
        GLenum type = GL_ZERO;
        char namebuf[128];
        glGetActiveUniform(program, GLuint(i), sizeof(namebuf)-1, &namelen, &size, &type, namebuf);
        namebuf[namelen] = 0;
        EGuint location = glGetUniformLocation(program, namebuf);
        EGRenderUniformInfoPtr uniforminfo(new EGRenderUniformInfo(namebuf, location, type, size));
        uniformNameMap.insert(std::pair<std::string,EGRenderUniformInfoPtr>(namebuf, uniforminfo));
    }
        
    // print attribute info
    std::map<std::string,EGRenderAttributeInfoPtr>::iterator ai;
    for (ai = attributeNameMap.begin(); ai != attributeNameMap.end(); ai++) {
        EGRenderAttributeInfoPtr &attributeinfo = (*ai).second;
        EGDebug("%s:%s attribute %-30s location=%-3d type=%s\n",
            class_name, __func__, attributeinfo->name.c_str(), attributeinfo->index, EGRenderUniformInfo::typeName(attributeinfo->type));
    }

    // print uniform info
    std::map<std::string,EGRenderUniformInfoPtr>::iterator ui;
    for (ui = uniformNameMap.begin(); ui != uniformNameMap.end(); ui++) {
        EGRenderUniformInfoPtr &uniforminfo = (*ui).second;
        EGDebug("%s:%s uniform   %-30s location=%-3d type=%s\n",
                class_name, __func__, uniforminfo->name.c_str(), uniforminfo->location, EGRenderUniformInfo::typeName(uniforminfo->type));
    }
	
	// release vertex and fragment shaders
	if (vertShader) {
		glDeleteShader(vertShader);
		vertShader = 0;
	}
	if (fragShader) {
		glDeleteShader(fragShader);
		fragShader = 0;
	}
    
    glUseProgram(program);
    
    EGDebug("%s:%s loaded shaders to program %d\n", class_name, __func__, program);

	return true;
}

/* compile shader */
EGint EGRenderProgram::compileShader(GLuint *shader, GLenum type, EGResourceList shaderRsrcs)
{
	
    EGsize numSources = shaderRsrcs.size();
    const EGbyte **sources = new const EGbyte*[numSources];
    EGint *lengths = new EGint[numSources];

	// read source code for shader resources
    for (EGsize i = 0; i < numSources ; i++) {
        EGResourcePtr &shaderRsrc = shaderRsrcs.at(i);
        EGoffset rsrcLen = shaderRsrc->getLength();
        if (rsrcLen <= 0) {
            EGError("%s:%s invalid resource: %s\n", class_name, __func__, shaderRsrc->getPath().c_str());
            for (EGsize j = 0; j < i ; j++) {
                delete [] sources[j];
            }
            delete [] sources;
            delete [] lengths;
            return 0;
        }
        EGbyte *source = new EGbyte[rsrcLen];
        sources[i] = source;
        lengths[i] = (EGint)rsrcLen;
        EGoffset readlen = shaderRsrc->read(source, rsrcLen);
        if (rsrcLen != readlen) {
            EGError("%s:%s short read: %s\n", class_name, __func__, shaderRsrc->getPath().c_str());
            for (EGsize j = 0; j <= i ; j++) {
                delete [] sources[j];
            }
            delete [] sources;
            delete [] lengths;
            return 0;
        }
        EGDebug("%s:%s read: %s\n", class_name, __func__, shaderRsrc->getPath().c_str());
    }
	
    // compile shader
    *shader = glCreateShader(type);				// create shader
    glShaderSource(*shader, (GLsizei)numSources, sources, lengths);	// set source code in the shader
    glCompileShader(*shader);					// compile shader
    
    for (EGsize i = 0; i < numSources ; i++) {
        delete [] sources[i];
    }
    delete [] sources;
    delete [] lengths;
	
    // print debug log
    if (debug) {
        GLint logLength;
        glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = new GLchar[logLength];
            glGetShaderInfoLog(*shader, logLength, &logLength, log);
            EGDebug("%s:%s shader sources: %s\n", class_name, __func__, sources);
            EGDebug("%s:%s shader compile log: %s\n", class_name, __func__, log);
            delete [] log;
        }
    }
    
	EGint status;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        EGError("%s:%s failed to compile shader\n", class_name, __func__);
	} else {
        EGDebug("%s:%s compiled shader\n", class_name, __func__);
    }
	
	return status;
}


/* Link a program with all currently attached shaders */
EGint EGRenderProgram::linkProgram(GLuint prog)
{
	EGint status;
	
	glLinkProgram(prog);
	
    // print debug log
    if (debug) {
        GLint logLength;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = new GLchar[logLength];
            glGetProgramInfoLog(prog, logLength, &logLength, log);
            EGDebug("%s:%s program link log: %s\n", class_name, __func__, log);
            delete [] log;
        }
    }
    
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        EGError("%s:%s failed to link program %d\n", class_name, __func__, prog);
	}
    
	return status;
}


/* Validate a program (for i.e. inconsistent samplers) */
EGint EGRenderProgram::validateProgram(GLuint prog)
{
	EGint logLength, status;
	
	glValidateProgram(prog);
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = new GLchar[logLength];
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        EGDebug("%s:%s program validate log: %s\n", class_name, __func__, log);
        delete [] log;
    }
    
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE) {
        EGError("%s:%s failed to validate program %d\n", class_name, __func__, prog);
    }
	
	return status;
}

/* delete shader resources */
void EGRenderProgram::destroyShaders(GLuint vertShader, GLuint fragShader, GLuint prog)
{
	if (vertShader) {
		glDeleteShader(vertShader);
		vertShader = 0;
	}
	if (fragShader) {
		glDeleteShader(fragShader);
		fragShader = 0;
	}
	if (prog) {
		glDeleteProgram(prog);
		prog = 0;
	}
}
