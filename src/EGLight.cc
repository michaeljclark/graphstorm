// See LICENSE for license details.

#include "EG.h"
#include "EGGL.h"
#include "EGLight.h"
#include "EGMath.h"
#include "EGRenderApi.h"
#include "EGRenderApiES2.h"


/* EGLight */

const EGfloat EGLight::POSITION_DEFAULT[4] = {0.0f, 0.0f, 1.0f, 0.0f};
const EGfloat EGLight::SPOTDIRECTION_DEFAULT[3] = {0.0f, 0.0f, -1.0f};
const EGfloat EGLight::AMBIENT_DEFAULT[4] = {0.0f, 0.0f, 0.0f, 1.0f};
const EGfloat EGLight::SPECULAR_DEFAULT[4] = {1.0f, 1.0f, 1.0f, 1.0f};
const EGfloat EGLight::DIFFUSE_DEFAULT[4] = {1.0f, 1.0f, 1.0f, 1.0f};

void EGLight::setupGLState()
{
    
    EGRenderApiES2 &gl = static_cast<EGRenderApiES2&>(EGRenderApi::currentApiImpl());
    EGRenderProgramPtr &activeprogram = EGRenderApiES2::activeprogram;
    if (!activeprogram) return;

    // multiply light position by current modelView matrix
    EGfloat modelViewMatrix[16];
    gl.getMatrix(EGRenderMatrixModelView, modelViewMatrix);
    EGfloat positionEye[4];
    EGMath::multMat4fVec4f(positionEye, position, modelViewMatrix);

    char u_light_ambient[20];
    char u_light_diffuse[20];
    char u_light_specular[20];
    char u_light_position[20];
    snprintf(u_light_ambient, sizeof(u_light_ambient), "u_light%d.ambient", lightN);
    snprintf(u_light_diffuse, sizeof(u_light_ambient), "u_light%d.diffuse", lightN);
    snprintf(u_light_specular, sizeof(u_light_ambient), "u_light%d.specular", lightN);
    snprintf(u_light_position, sizeof(u_light_ambient), "u_light%d.position", lightN);
    EGRenderUniformInfo *u_light0_ambient = activeprogram->getUniform(u_light_ambient);
    EGRenderUniformInfo *u_light0_diffuse = activeprogram->getUniform(u_light_diffuse);
    EGRenderUniformInfo *u_light0_specular = activeprogram->getUniform(u_light_specular);
    EGRenderUniformInfo *u_light0_position = activeprogram->getUniform(u_light_position);
    if(u_light0_ambient) glUniform4fv(u_light0_ambient->location, 1, ambient);
    if(u_light0_diffuse) glUniform4fv(u_light0_diffuse->location, 1, diffuse);
    if(u_light0_specular) glUniform4fv(u_light0_specular->location, 1, specular);
    if(u_light0_position) glUniform4fv(u_light0_position->location, 1, position);
}
