/*
 *  EGMath.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGMath.h"


/* EGMath */

const EGfloat EGMath::Pi = 3.1415927f;
const EGfloat EGMath::Rad2Deg = 180.0f / Pi;
const EGfloat EGMath::Deg2Rad = Pi / 180.0f;
const EGfloat EGMath::Epsilon = 0.001f;


EGfloat EGMath::dot2f(EGfloat a[2], EGfloat b[2])
{
    return a[0] * b[0] + a[1] * b[1];
}

EGfloat EGMath::dot3f(EGfloat a[3], EGfloat b[3])
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

EGfloat EGMath::dot4f(EGfloat a[4], EGfloat b[4])
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}

void EGMath::normalize2f(EGfloat v[])
{
    EGfloat r = (EGfloat)sqrt(dot2f(v, v));
    if (r > 0) {
        v[ 0 ] /= r;
        v[ 1 ] /= r;
    }
}

void EGMath::normalize3f(EGfloat v[])
{
    EGfloat r = (EGfloat)sqrt(dot3f(v, v));
    if (r > 0) {
        v[ 0 ] /= r;
        v[ 1 ] /= r;
        v[ 2 ] /= r;
    }
}

void EGMath::multMat2fMat2f(EGfloat r[4], const EGfloat a[4], const EGfloat b[4])
{
    r[0]  = a[0]  * b[0] + a[1]  * b[2];
    r[1]  = a[0]  * b[1] + a[1]  * b[3];
    r[2]  = a[2]  * b[0] + a[3]  * b[2];
    r[3]  = a[2]  * b[1] + a[3]  * b[3];
}

void EGMath::multMat2fVec2f(EGfloat r[2], const EGfloat v[2], const EGfloat m[4])
{
    r[0] = v[0] * m[0] + v[1] * m[2];
    r[1] = v[0] * m[1] + v[1] * m[3];
}

void EGMath::multMat3fMat3f(EGfloat r[9], const EGfloat a[9], const EGfloat b[9])
{
    r[0]  = a[0]  * b[0] + a[1]  * b[3] + a[2] * b[6];
    r[1]  = a[0]  * b[1] + a[1]  * b[4] + a[2] * b[7];
    r[2]  = a[0]  * b[2] + a[1]  * b[5] + a[2] * b[8];
    r[3]  = a[3]  * b[0] + a[4]  * b[3] + a[5] * b[6];
    r[4]  = a[3]  * b[1] + a[4]  * b[4] + a[5] * b[7];
    r[5]  = a[3]  * b[2] + a[4]  * b[5] + a[5] * b[8];
    r[6]  = a[6]  * b[0] + a[7]  * b[3] + a[8] * b[6];
    r[7]  = a[6]  * b[1] + a[7]  * b[4] + a[8] * b[7];
    r[8]  = a[6]  * b[2] + a[7]  * b[5] + a[8] * b[8];
}

void EGMath::multMat3fVec3f(EGfloat r[3], const EGfloat v[3], const EGfloat m[9])
{
    r[0] = v[0] * m[0] + v[1] * m[3] + v[2] * m[6];
    r[1] = v[0] * m[1] + v[1] * m[4] + v[2] * m[7];
    r[2] = v[0] * m[2] + v[1] * m[5] + v[2] * m[8];
}

void EGMath::multMat4fMat4f(EGfloat r[16], const EGfloat a[16], const EGfloat b[16])
{
    r[0]  = a[0]  * b[0] + a[1]  * b[4] + a[2]  * b[8]  + a[3]  * b[12];
    r[1]  = a[0]  * b[1] + a[1]  * b[5] + a[2]  * b[9]  + a[3]  * b[13];
    r[2]  = a[0]  * b[2] + a[1]  * b[6] + a[2]  * b[10] + a[3]  * b[14];
    r[3]  = a[0]  * b[3] + a[1]  * b[7] + a[2]  * b[11] + a[3]  * b[15];
    r[4]  = a[4]  * b[0] + a[5]  * b[4] + a[6]  * b[8]  + a[7]  * b[12];
    r[5]  = a[4]  * b[1] + a[5]  * b[5] + a[6]  * b[9]  + a[7]  * b[13];
    r[6]  = a[4]  * b[2] + a[5]  * b[6] + a[6]  * b[10] + a[7]  * b[14];
    r[7]  = a[4]  * b[3] + a[5]  * b[7] + a[6]  * b[11] + a[7]  * b[15];
    r[8]  = a[8]  * b[0] + a[9]  * b[4] + a[10] * b[8]  + a[11] * b[12];
    r[9]  = a[8]  * b[1] + a[9]  * b[5] + a[10] * b[9]  + a[11] * b[13];
    r[10] = a[8]  * b[2] + a[9]  * b[6] + a[10] * b[10] + a[11] * b[14];
    r[11] = a[8]  * b[3] + a[9]  * b[7] + a[10] * b[11] + a[11] * b[15];
    r[12] = a[12] * b[0] + a[13] * b[4] + a[14] * b[8]  + a[15] * b[12];
    r[13] = a[12] * b[1] + a[13] * b[5] + a[14] * b[9]  + a[15] * b[13];
    r[14] = a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14];
    r[15] = a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15];
}

void EGMath::multMat4fVec4f(EGfloat r[4], const EGfloat v[4], const EGfloat m[16])
{
    r[0] = v[0] * m[0] + v[1] * m[4] + v[2] * m[8]  + v[3] * m[12];
    r[1] = v[0] * m[1] + v[1] * m[5] + v[2] * m[9]  + v[3] * m[13];
    r[2] = v[0] * m[2] + v[1] * m[6] + v[2] * m[10] + v[3] * m[14];
    r[3] = v[0] * m[3] + v[1] * m[7] + v[2] * m[11] + v[3] * m[15];
}

void EGMath::multMat4fVec4fHomogenousDivide(EGfloat r[4], const EGfloat v[4], const EGfloat m[16])
{
    multMat4fVec4f(r, v, m);
    EGfloat rdiv = 1.0f / r[3];
    r[0] *= rdiv;
    r[1] *= rdiv;
    r[2] *= rdiv;    
    r[3] = 1;
}

void EGMath::multMat4f(EGfloat m[16], const EGfloat lm[16])
{
    EGfloat r[16];
    multMat4fMat4f(r, lm, m);
    memcpy(m, r, sizeof(r));
}

void EGMath::identityMat3f(EGfloat m[9])
{
    m[0] = 1; m[1] = 0; m[2] = 0;
    m[3] = 0; m[4] = 1; m[5] = 0;
    m[6] = 0; m[7] = 0; m[8] = 1;
}

void EGMath::identityMat4f(EGfloat m[16])
{
    m[0] = 1;  m[1] = 0;  m[2] = 0;  m[3] = 0;
    m[4] = 0;  m[5] = 1;  m[6] = 0;  m[7] = 0;
    m[8] = 0;  m[9] = 0;  m[10] = 1; m[11] = 0;
    m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}

void EGMath::scaleMat4f(EGfloat m[16], EGfloat x, EGfloat y, EGfloat z)
{
    m[0] *= x;   m[1] *= x;   m[2]  *= x;   m[3]  *= x;
    m[4] *= y;   m[5] *= y;   m[6]  *= y;   m[7]  *= y;
    m[8] *= z;   m[9] *= z;   m[10] *= z;   m[11] *= z;
}

void EGMath::translateMat4f(EGfloat m[16], EGfloat x, EGfloat y, EGfloat z)
{
    m[12] = x * m[0] + y * m[4] + z * m[8] +  m[12];
    m[13] = x * m[1] + y * m[5] + z * m[9] +  m[13];
    m[14] = x * m[2] + y * m[6] + z * m[10] + m[14];
    m[15] = x * m[3] + y * m[7] + z * m[11] + m[15];
}

void EGMath::rotateMat4f(EGfloat m[16], EGfloat angle, EGfloat ax, EGfloat ay, EGfloat az)
{
    EGfloat lm[16];
    makeRotateMat4f(lm, angle, ax, ay, az);
    multMat4f(m, lm);
}

void EGMath::makeFrustumMat4f(EGfloat m[16], EGfloat left, EGfloat right, EGfloat bottom, EGfloat top, EGfloat zNear, EGfloat zFar)
{
    EGfloat deltaX = right - left;
    EGfloat deltaY = top - bottom;
    EGfloat deltaZ = zFar - zNear;
    
    if ((zNear <= 0) || (zFar <= 0) || (deltaX == 0) ||
        (deltaY == 0) || (deltaZ == 0)) {
        return;
    }
    
    m[0] = zNear * 2.0f / deltaX;
    m[1] = 0;
    m[2] = 0;
    m[3] = 0;
    
    m[4] = 0;
    m[5] = zNear * 2.0f / deltaY;
    m[6] = 0;
    m[7] = 0;
    
    m[8] = (right + left) / deltaX;
    m[9] = (top + bottom) / deltaY;
    m[10] = -(zFar + zNear) / deltaZ;
    m[11] = -1;
    
    m[12] = 0;
    m[13] = 0;
    m[14] = -2.0f * zNear * zFar / deltaZ;
    m[15] = 0;
}

void EGMath::frustumMat4f(EGfloat m[16], EGfloat left, EGfloat right, EGfloat bottom, EGfloat top, EGfloat zNear, EGfloat zFar)
{
    EGfloat lm[16] = {};
    makeFrustumMat4f(lm, left, right, bottom, top, zNear, zFar);
    multMat4f(m, lm);
}

void EGMath::makeLookAtMat4f(EGfloat m[16], EGfloat eyex, EGfloat eyey, EGfloat eyez, EGfloat centerx, EGfloat centery, EGfloat centerz, EGfloat upx, EGfloat upy, EGfloat upz)
{
    EGfloat forward[3] = { centerx - eyex, centery - eyey, centerz - eyez };
    EGfloat up[3] = { upx, upy, upz };
    EGfloat side[3];
    
    normalize3f(forward);
    cross3f(side, forward, up);
    normalize3f(side);
    cross3f(up, side, forward);
    
    m[0] = side[0];  m[1] = up[0];  m[2] = -forward[0];  m[3] = 0;
    m[4] = side[1];  m[5] = up[1];  m[6] = -forward[1];  m[7] = 0;
    m[8] = side[2];  m[9] = up[2];  m[10] = -forward[2]; m[11] = 0;
    m[12] = side[0] * -eyex + up[0] * -eyey + -forward[0] * -eyez;
    m[13] = side[1] * -eyex + up[1] * -eyey + -forward[1] * -eyez;
    m[14] = side[2] * -eyex + up[2] * -eyey + -forward[2] * -eyez;
    m[15] = 1;
}

void EGMath::lookAtMat4f(EGfloat m[16], EGfloat eyex, EGfloat eyey, EGfloat eyez, EGfloat centerx, EGfloat centery, EGfloat centerz, EGfloat upx, EGfloat upy, EGfloat upz)
{
    EGfloat forward[3] = { centerx - eyex, centery - eyey, centerz - eyez };
    EGfloat up[3] = { upx, upy, upz };
    EGfloat side[3];
    EGfloat lm[16];

    normalize3f(forward);
    cross3f(side, forward, up);
    normalize3f(side);
    cross3f(up, side, forward);
    
    lm[0] = side[0];  lm[1] = up[0];  lm[2] = -forward[0];  lm[3] = 0;
    lm[4] = side[1];  lm[5] = up[1];  lm[6] = -forward[1];  lm[7] = 0;
    lm[8] = side[2];  lm[9] = up[2];  lm[10] = -forward[2]; lm[11] = 0;
    lm[12] = 0;       lm[13] = 0;     lm[14] = 0;           lm[15] = 1;

    multMat4f(m, lm);
    translateMat4f(m, -eyex, -eyey, -eyez);
}

void EGMath::transposeMat3f(EGfloat r[9], EGfloat m[9])
{
    r[0] = m[0];
    r[1] = m[3];
    r[2] = m[6];
    r[3] = m[1];
    r[4] = m[4];
    r[5] = m[7];
    r[6] = m[2];
    r[7] = m[5];
    r[8] = m[8];
}

void EGMath::transposeMat3f(EGfloat m[9])
{
    EGfloat r[9];
    transposeMat3f(r, m);
    memcpy(m, r, sizeof(r));
}

void EGMath::transposeMat4f(EGfloat r[16], EGfloat m[16])
{
    r[0] = m[0];
    r[1] = m[4];
    r[2] = m[8];
    r[3] = m[12];
    r[4] = m[1];
    r[5] = m[5];
    r[6] = m[9];
    r[7] = m[13];
    r[8] = m[2];
    r[9] = m[6];
    r[10] = m[10];
    r[11] = m[14];
    r[12] = m[3];
    r[13] = m[7];
    r[14] = m[11];
    r[15] = m[15];
}

void EGMath::transposeMat4f(EGfloat m[16])
{
    EGfloat r[16];
    transposeMat4f(r, m);
    memcpy(m, r, sizeof(r));
}

/* source glpuzzle.c invertMatrix - SGI BSD license */
int EGMath::invertMat3f(EGfloat inverse[9], const EGfloat src[9])
{
    int i, j, k, swap;
    EGfloat t;
    EGfloat temp[3][3];
    
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            temp[i][j] = src[i * 3 + j];
        }
    }
    identityMat3f(inverse);
    
    for (i = 0; i < 3; i++) {
        /* 
         ** Look for largest element in column */
        swap = i;
        for (j = i + 1; j < 3; j++) {
            if (fabs(temp[j][i]) > fabs(temp[i][i])) {
                swap = j;
            }
        }
        
        if (swap != i) {
            /* 
             ** Swap rows. */
            for (k = 0; k < 3; k++) {
                t = temp[i][k];
                temp[i][k] = temp[swap][k];
                temp[swap][k] = t;
                
                t = inverse[i * 3 + k];
                inverse[i * 3 + k] = inverse[swap * 3 + k];
                inverse[swap * 3 + k] = t;
            }
        }
        if (temp[i][i] == 0) {
            /* 
             ** No non-zero pivot.  The matrix is singular, which
             shouldn't ** happen.  This means the user gave us a
             bad matrix. */
            return 0;
        }
        t = temp[i][i];
        for (k = 0; k < 3; k++) {
            temp[i][k] /= t;
            inverse[i * 3 + k] /= t;
        }
        for (j = 0; j < 3; j++) {
            if (j != i) {
                t = temp[j][i];
                for (k = 0; k < 3; k++) {
                    temp[j][k] -= temp[i][k] * t;
                    inverse[j * 3 + k] -= inverse[i * 3 + k] * t;
                }
            }
        }
    }
    return 1;
}

/* source glpuzzle.c invertMatrix - SGI BSD license */
int EGMath::invertMat4f(EGfloat inverse[16], const EGfloat src[16])
{
    int i, j, k, swap;
    EGfloat t;
    EGfloat temp[4][4];
    
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            temp[i][j] = src[i * 4 + j];
        }
    }
    identityMat4f(inverse);
    
    for (i = 0; i < 4; i++) {
        /* 
         ** Look for largest element in column */
        swap = i;
        for (j = i + 1; j < 4; j++) {
            if (fabs(temp[j][i]) > fabs(temp[i][i])) {
                swap = j;
            }
        }
        
        if (swap != i) {
            /* 
             ** Swap rows. */
            for (k = 0; k < 4; k++) {
                t = temp[i][k];
                temp[i][k] = temp[swap][k];
                temp[swap][k] = t;
                
                t = inverse[i * 4 + k];
                inverse[i * 4 + k] = inverse[swap * 4 + k];
                inverse[swap * 4 + k] = t;
            }
        }
        if (temp[i][i] == 0) {
            /* 
             ** No non-zero pivot.  The matrix is singular, which
             shouldn't ** happen.  This means the user gave us a
             bad matrix. */
            return 0;
        }
        t = temp[i][i];
        for (k = 0; k < 4; k++) {
            temp[i][k] /= t;
            inverse[i * 4 + k] /= t;
        }
        for (j = 0; j < 4; j++) {
            if (j != i) {
                t = temp[j][i];
                for (k = 0; k < 4; k++) {
                    temp[j][k] -= temp[i][k] * t;
                    inverse[j * 4 + k] -= inverse[i * 4 + k] * t;
                }
            }
        }
    }
    return 1;
}

void EGMath::mat4toMat3f(EGfloat r[9], EGfloat m[16])
{
    r[0] = m[0];
    r[1] = m[1];
    r[2] = m[2];
    r[3] = m[4];
    r[4] = m[5];
    r[5] = m[6];
    r[6] = m[8];
    r[7] = m[9];
    r[8] = m[10];
}

void EGMath::makeRotateMat3f(EGfloat rot[9], EGfloat angle, EGfloat ax, EGfloat ay, EGfloat az)
{
    EGfloat axis[3] = { ax, ay, az };
    normalize3f(axis);
    
    EGfloat ar = angle * Deg2Rad;
    EGfloat s = sin(ar);
    EGfloat c = cos(ar);
    EGfloat a2 = axis[0] * axis[0];
    EGfloat b2 = axis[1] * axis[1];
    EGfloat c2 = axis[2] * axis[2];
    EGfloat as = axis[0] * s;
    EGfloat bs = axis[1] * s;
    EGfloat cs = axis[2] * s;
    EGfloat ab = axis[0] * axis[1] * (1 - c);
    EGfloat bc = axis[1] * axis[2] * (1 - c);
    EGfloat ca = axis[2] * axis[0] * (1 - c);
    
    rot[0] = a2 + c * (1 - a2);
    rot[1] = ab + cs;
    rot[2] = ca - bs;
    rot[3] = ab - cs;
    rot[4] = b2 + c * (1 - b2);
    rot[5] = bc + as;
    rot[6] = ca + bs;
    rot[7] = bc - as;
    rot[8] = c2 + c * (1 - c2);
}

void EGMath::makeRotateMat4f(EGfloat rot[16], EGfloat angle, EGfloat ax, EGfloat ay, EGfloat az)
{
    EGfloat axis[3] = { ax, ay, az };
    normalize3f(axis);
    
    EGfloat ar = angle * Deg2Rad;
    EGfloat s = sin(ar);
    EGfloat c = cos(ar);
    EGfloat a2 = axis[0] * axis[0];
    EGfloat b2 = axis[1] * axis[1];
    EGfloat c2 = axis[2] * axis[2];
    EGfloat as = axis[0] * s;
    EGfloat bs = axis[1] * s;
    EGfloat cs = axis[2] * s;
    EGfloat ab = axis[0] * axis[1] * (1 - c);
    EGfloat bc = axis[1] * axis[2] * (1 - c);
    EGfloat ca = axis[2] * axis[0] * (1 - c);
    
    rot[0] = a2 + c * (1 - a2);
    rot[1] = ab + cs;
    rot[2] = ca - bs;
    rot[3] = 0;
    rot[4] = ab - cs;
    rot[5] = b2 + c * (1 - b2);
    rot[6] = bc + as;
    rot[7] = 0;
    rot[8] = ca + bs;
    rot[9] = bc - as;
    rot[10] = c2 + c * (1 - c2);
    rot[11] = 0;
    rot[12] = 0;
    rot[13] = 0;
    rot[14] = 0;
    rot[15] = 1;
}

void EGMath::rotateMat3f(EGfloat r[9], EGfloat mat[9], EGfloat angle, EGfloat ax, EGfloat ay, EGfloat az)
{
    EGfloat rot[9];
    makeRotateMat3f(rot, angle, ax, ay, az);
    multMat3fMat3f(r, mat, rot);
}

void EGMath::rotateMat4f(EGfloat r[16], EGfloat mat[16], EGfloat angle, EGfloat ax, EGfloat ay, EGfloat az)
{
    EGfloat rot[16];
    makeRotateMat4f(rot, angle, ax, ay, az);
    multMat4fMat4f(r, mat, rot);
}

void EGMath::rot4fToQuat4f(EGfloat q[4], EGfloat r[4])
{
    EGfloat ar = r[0] * EGMath::Deg2Rad;
    EGfloat sin_ar2 = sinf(ar/2);
    q[0] = cosf(ar/2);
    q[1] = r[1] * sin_ar2;
    q[2] = r[2] * sin_ar2;
    q[3] = r[3] * sin_ar2;
}

void EGMath::quat4fToRot4f(EGfloat rot[4], EGfloat q[4])
{
    rot[0] = 2 * acosf(q[0]) * EGMath::Rad2Deg;
    rot[1] = q[1] / sqrtf(1 - q[0] * q[0]);
    rot[2] = q[2] / sqrtf(1 - q[0] * q[0]);
    rot[3] = q[3] / sqrtf(1 - q[0] * q[0]);
}

void EGMath::mat3fToQuat4f(EGfloat q[4], EGfloat mat[9])
{
    q[0] = sqrtf(1.0f + mat[0] + mat[4] + mat[8]) / 2.0f;
    q[1] = (mat[5] - mat[7])/( 4.0f * q[0]);
    q[2] = (mat[6] - mat[2])/( 4.0f * q[0]);
    q[3] = (mat[1] - mat[3])/( 4.0f * q[0]);
}

void EGMath::quat4fToMat3f(EGfloat mat[9], EGfloat q[4])
{
    mat[0] = 1 - 2 * q[2] * q[2] - 2 * q[3] * q[3];
    mat[3] = 2 * q[1] * q[2] - 2 * q[3] * q[0];
    mat[6] = 2 * q[1] * q[3] + 2 * q[2] * q[0];
    mat[1] = 2 * q[1] * q[2] + 2 * q[3] * q[0];
    mat[4] = 1 - 2 * q[1] * q[1] - 2 * q[3] * q[3];
    mat[7] = 2 * q[2] * q[3] - 2 * q[1] * q[0];
    mat[2] = 2 * q[1] * q[3] - 2 * q[2] * q[0];
    mat[5] = 2 * q[2] * q[3] + 2 * q[1] * q[0];
    mat[8] = 1 - 2 * q[1] * q[1] - 2 * q[2] * q[2];
}

void EGMath::scaleQuat4f(EGfloat r[4], EGfloat q[4], EGfloat m)
{
    r[0] = q[0] * m;
    r[1] = q[1] * m;
    r[2] = q[2] * m;
    r[3] = q[3] * m;
}

void EGMath::multQuat4fQuat4f(EGfloat r[4], EGfloat q1[4], EGfloat q2[4])
{
    r[1] =  q1[1] * q2[0] + q1[2] * q2[3] - q1[3] * q2[2] + q1[0] * q2[1];
    r[2] = -q1[1] * q2[3] + q1[2] * q2[0] + q1[3] * q2[1] + q1[0] * q2[2];
    r[3] =  q1[1] * q2[2] - q1[2] * q2[1] + q1[3] * q2[0] + q1[0] * q2[3];
    r[0] = -q1[1] * q2[1] - q1[2] * q2[2] - q1[3] * q2[3] + q1[0] * q2[0];
}

void EGMath::addQuat4fQuat4f(EGfloat r[4], EGfloat q1[4], EGfloat q2[4])
{
    r[0] = q1[0] + q2[0];
    r[1] = q1[1] + q2[1];
    r[2] = q1[2] + q2[2];
    r[3] = q1[3] + q2[3];
}

void EGMath::conjugateQuat4f(EGfloat r[4], EGfloat q[4])
{
    r[0] = q[0];
    r[1] = -q[1];
    r[2] = -q[2];
    r[3] = -q[3];
}

void EGMath::normalizeQuat4f(EGfloat q[4])
{
    EGfloat n = sqrtf(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
    q[0] /= n;
    q[1] /= n;
    q[2] /= n;
    q[3] /= n;
}

void EGMath::lerpQuat4f(EGfloat r[4], EGfloat q1[4], EGfloat q2[4], EGfloat t)
{
    r[0] = q1[0] + t * (q2[0] - q1[0]);
    r[1] = q1[1] + t * (q2[1] - q1[1]);
    r[2] = q1[2] + t * (q2[2] - q1[2]);
    r[3] = q1[3] + t * (q2[3] - q1[3]);
}

void EGMath::slerpQuat4f(EGfloat r[4], EGfloat q1[4], EGfloat q2[4], EGfloat t)
{
    EGfloat q3[4] = { q2[0], q2[1], q2[2], q2[3] };
    
    EGfloat c = dot4f(q1, q3);
    
    if (c < 0.0)
    {
        scaleQuat4f(q3, q3, -1);
        c = -c;
    }
    
    if (c > 1.0 - Epsilon) {
        lerpQuat4f(r, q1, q3, t);
        normalizeQuat4f(r);
        return;
    }
    
    EGfloat a = acosf(c);
    EGfloat sin_t1 = sinf((1.0f - t) * a);
    EGfloat sin_t2 = sinf(t * a);
    r[0] = q1[0] * sin_t1 + q3[0] * sin_t2;
    r[1] = q1[1] * sin_t1 + q3[1] * sin_t2;
    r[2] = q1[2] * sin_t1 + q3[2] * sin_t2;
    r[3] = q1[3] * sin_t1 + q3[3] * sin_t2;
    normalizeQuat4f(r);
}

EGfloat EGMath::slerpAngleQuat4f(EGfloat q1[4], EGfloat q2[4])
{    
    EGfloat c = dot4f(q1, q2);
    
    if (c < 0.0)
    {
        c = -c;
    }
    
    return acosf(c) * EGMath::Rad2Deg;
}

void EGMath::project(EGfloat r[3], const EGfloat v[3], const EGfloat finalMatrix[16])
{
    EGfloat a[4] = { v[0], v[1], v[2], 1.0 };
    EGfloat b[4];
    multMat4fVec4f(b, a, finalMatrix);
    EGfloat nr = 1.0f / b[3];
    r[0] = b[0] * nr;
    r[1] = b[1] * nr;
    r[2] = b[2] * nr;
}

void EGMath::sub3f(EGfloat r[3], const EGfloat v1[3], const EGfloat v2[3])
{
    r[0] = v1[0] - v2[0];
    r[1] = v1[1] - v2[1];
    r[2] = v1[2] - v2[2];     
}

void EGMath::cross3f(EGfloat n[3], const EGfloat v1[3], const EGfloat v2[3])
{
    n[0] = v1[1]*v2[2] - v1[2]*v2[1];
    n[1] = v1[2]*v2[0] - v1[0]*v2[2];
    n[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

void EGMath::cross3f(EGfloat n[3], const EGfloat a[3], const EGfloat b[3], const EGfloat c[3])
{
    const EGfloat v1[3] = { b[0] - a[0], b[1] - a[1], b[2] - a[2] };
    const EGfloat v2[3] = { c[0] - a[0], c[1] - a[1], c[2] - a[2] };

    n[0] = v1[1]*v2[2] - v1[2]*v2[1];
    n[1] = v1[2]*v2[0] - v1[0]*v2[2];
    n[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

/* cross product for 2d vectors that only returns z */
EGfloat EGMath::cross3fz(const EGfloat a[2], const EGfloat b[2], const EGfloat c[2])
{
    const EGfloat v1x = b[0] - a[0], v1y = b[1] - a[1];
    const EGfloat v2x = c[0] - a[0], v2y = c[1] - a[1];
    return v1x*v2y - v1y*v2x;
}

EGIntersectType EGMath::intersectPoint2DBox2D(EGfloat a[2], EGfloat b_lo[2], EGfloat b_hi[2])
{
    if (a[0] <= b_hi[0] && a[0] >= b_lo[0] &&
        a[1] <= b_hi[1] && a[1] >= b_lo[1]) {
        if (a[0] <= b_lo[0] && a[0] >= b_hi[0] &&
            a[1] <= b_lo[1] && a[1] >= b_hi[1]) {
            return kGKIntersectContains; /* A contains B */
        } else {
            return kGKIntersectIntersecting; /* A intersects B */
        }
    } else {
        return kGKIntersectNotIntersecting;
    }
}

EGIntersectType EGMath::intersectBox2DBox2D(EGfloat a_lo[2], EGfloat a_hi[2], EGfloat b_lo[2], EGfloat b_hi[2])
{
    if (a_lo[0] <= b_hi[0] && a_hi[0] >= b_lo[0] &&
        a_lo[1] <= b_hi[1] && a_hi[1] >= b_lo[1]) {
        if (a_lo[0] <= b_lo[0] && a_hi[0] >= b_hi[0] &&
            a_lo[1] <= b_lo[1] && a_hi[1] >= b_hi[1]) {
            return kGKIntersectContains; /* A contains B */
        } else {
            return kGKIntersectIntersecting; /* A intersects B */
        }
    } else {
        return kGKIntersectNotIntersecting;
    }
}

EGIntersectType EGMath::intersectPoint3DBox3D(EGfloat a[3], EGfloat b_lo[3], EGfloat b_hi[3])
{
    if (a[0] <= b_hi[0] && a[0] >= b_lo[0] &&
        a[1] <= b_hi[1] && a[1] >= b_lo[1] &&
        a[2] <= b_hi[2] && a[2] >= b_lo[2]) {
        if (a[0] <= b_lo[0] && a[0] >= b_hi[0] &&
            a[1] <= b_lo[1] && a[1] >= b_hi[1] &&
            a[2] <= b_lo[2] && a[2] >= b_hi[2]) {
            return kGKIntersectContains; /* A contains B */
        } else {
            return kGKIntersectIntersecting; /* A intersects B */
        }
    } else {
        return kGKIntersectNotIntersecting;
    }
}

EGIntersectType EGMath::intersectBox3DBox3D(EGfloat a_lo[3], EGfloat a_hi[3], EGfloat b_lo[3], EGfloat b_hi[3])
{
    if (a_lo[0] <= b_hi[0] && a_hi[0] >= b_lo[0] &&
        a_lo[1] <= b_hi[1] && a_hi[1] >= b_lo[1] &&
        a_lo[2] <= b_hi[2] && a_hi[2] >= b_lo[2]) {
        if (a_lo[0] <= b_lo[0] && a_hi[0] >= b_hi[0] &&
            a_lo[1] <= b_lo[1] && a_hi[1] >= b_hi[1] &&
            a_lo[2] <= b_lo[2] && a_hi[2] >= b_hi[2]) {
            return kGKIntersectContains; /* A contains B */
        } else {
            return kGKIntersectIntersecting; /* A intersects B */
        }
    } else {
        return kGKIntersectNotIntersecting;
    }
}

EGIntersectType EGMath::intersectLines(EGfloat a1x, EGfloat a1y, EGfloat a2x, EGfloat a2y,
									   EGfloat b1x, EGfloat b1y, EGfloat b2x, EGfloat b2y,
									   EGfloat *ix, EGfloat *iy)
{
    EGfloat denom = ((b2y - b1y) * (a2x - a1x)) - ((b2x - b1x) * (a2y - a1y));
    EGfloat nume_a = ((b2x - b1x) * (a1y - b1y)) - ((b2y - b1y) * (a1x - b1x));
    EGfloat nume_b = ((a2x - a1x) * (a1y - b1y)) - ((a2y - a1y) * (a1x - b1x));

    if (denom == 0.0f) {
        if (nume_a == 0.0f && nume_b == 0.0f) {
            return kGKIntersectCoincident;
        } else {
            return kGKIntersectNotIntersecting;
        }
    }

    EGfloat ua = nume_a / denom;
    EGfloat ub = nume_b / denom;

    if (ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f) {
        if (ix) {
            *ix = a1x + ua * (a2x - a1x);
        }
        if (iy) {
            *iy = a1y + ua * (a2y - a1y);
        }
        return kGKIntersectIntersecting;
    } else {
        return kGKIntersectNotIntersecting;
    }
}

/* derived from Fast, Minimum Storage Ray / Triangle Intersection, Tomas Moller */

int EGMath::intersectRayTriangle(EGfloat orig[3], EGfloat dir[3],
                                 EGfloat vert0[3], EGfloat vert1[3], EGfloat vert2[3],
                                 EGfloat *t, EGfloat *u, EGfloat *v)
{
    EGfloat edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
    EGfloat det, inv_det;
    
    /* find vectors for two edges sharing vert0 */
    sub3f(edge1, vert1, vert0);
    sub3f(edge2, vert2, vert0);
    
    /* begin calculating determinant - also used to calculate U parameter */
    cross3f(pvec, dir, edge2);
    
    /* if determinant is near zero, ray lies in plane of triangle */
    det = dot3f(edge1, pvec);

#define TEST_CULL
#ifdef TEST_CULL           /* define TEST_CULL if culling is desired */
    if (det < Epsilon)
        return 0;
    
    /* calculate distance from vert0 to ray origin */
    sub3f(tvec, orig, vert0);
    
    /* calculate U parameter and test bounds */
    *u = dot3f(tvec, pvec);
    if (*u < 0.0 || *u > det)
        return 0;
    
    /* prepare to test V parameter */
    cross3f(qvec, tvec, edge1);
    
    /* calculate V parameter and test bounds */
    *v = dot3f(dir, qvec);
    if (*v < 0.0 || *u + *v > det)
        return 0;
    
    /* calculate t, scale parameters, ray intersects triangle */
    *t = dot3f(edge2, qvec);
    inv_det = 1.0f / det;
    *t *= inv_det;
    *u *= inv_det;
    *v *= inv_det;
#else                    /* the non-culling branch */
    if (det > -Epsilon && det < Epsilon)
        return 0;
    inv_det = 1.0 / det;
    
    /* calculate distance from vert0 to ray origin */
    sub3f(tvec, orig, vert0);
    
    /* calculate U parameter and test bounds */
    *u = dot3f(tvec, pvec) * inv_det;
    if (*u < 0.0 || *u > 1.0)
        return 0;
    
    /* prepare to test V parameter */
    cross3f(qvec, tvec, edge1);
    
    /* calculate V parameter and test bounds */
    *v = dot3f(dir, qvec) * inv_det;
    if (*v < 0.0 || *u + *v > 1.0)
        return 0;
    
    /* calculate t, ray intersects triangle */
    *t = dot3f(edge2, qvec) * inv_det;
#endif
    return 1;
}

EGfloat EGMath::haversine(EGfloat lat1, EGfloat lon1, EGfloat lat2, EGfloat lon2)
{
    EGfloat dlat = lat2 - lat1;
    EGfloat dlon = lon2 - lon1;
    EGfloat a = sinf(dlat / 2.0f) * sinf(dlat / 2.0f) +
    cosf(lat1) * cosf(lat2) *
    sinf(dlon / 2.0f) * sinf(dlon / 2.0f);
    EGfloat c = 2.0f * atan2f(sqrtf(a), sqrtf(1.0f - a));
    EGfloat d = 6371.0f * c;
    return d;
}
