// See LICENSE for license details.

#include "EG.h"
#include "EGGL.h"
#include "EGMath.h"
#include "EGRenderApi.h"
#include "EGRenderApiES2.h"
#include "EGViewport.h"
#include "EGCamera.h"

void EGCamera::setupRenderState(EGViewportPtr &viewport)
{
    EGfloat m[16];
    EGfloat xmin, xmax, ymin, ymax;
    EGfloat zFar = focalLength * 100.0f; // far frustum plane
    EGfloat zNear = (std::min)((EGfloat)focalLength, (std::max)(focalLength/100.0f, 0.1f)); // near frustum plane clamped at 0.1
    EGfloat aspect = (EGfloat)viewport->viewportWidth / (EGfloat)viewport->viewportHeight;  // window aspect ratio
    
    if (aspect > 1.0) {
        ymax = zNear * tan(aperture * 0.5f * EGMath::Deg2Rad);
        ymin = -ymax;
        xmin = ymin * aspect;
        xmax = ymax * aspect;
    } else {
        xmax = zNear * tan(aperture * 0.5f * EGMath::Deg2Rad);
        xmin = -xmax;
        ymin = xmin / aspect;
        ymax = xmax / aspect;
    }
    
    EGRenderApi *gl = &EGRenderApi::currentApiImpl();
    
    glViewport(viewport->viewportX, viewport->viewportY, viewport->viewportWidth, viewport->viewportHeight);
    
    EGMath::makeFrustumMat4f(m, xmin, xmax, ymin, ymax, zNear, zFar);
    gl->loadMatrix(EGRenderMatrixProjection, m);
    
    EGMath::makeLookAtMat4f(m, viewPos[0], viewPos[1], viewPos[2],
                            viewPos[0] + viewDir[0], viewPos[1] + viewDir[1], viewPos[2] + viewDir[2],
                            viewUp[0], viewUp[1] ,viewUp[2]);
    
    gl->loadMatrix(EGRenderMatrixModelView, m);
}
