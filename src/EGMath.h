// See LICENSE for license details.

#pragma once

/* EGIntersectType */

enum EGIntersectType
{
    kGKIntersectNotIntersecting,
    kGKIntersectIntersecting,
    kGKIntersectCoincident,
    kGKIntersectContains
};


/* EGMath */

class EGMath
{
public:
    static const EGfloat Pi;
    static const EGfloat Rad2Deg;
    static const EGfloat Deg2Rad;
    static const EGfloat Epsilon;

    static EGfloat dot2f(EGfloat a[3], EGfloat b[3]);
    static EGfloat dot3f(EGfloat a[3], EGfloat b[3]);
    static EGfloat dot4f(EGfloat a[4], EGfloat b[4]);
    static void normalize2f(EGfloat v[]);
    static void normalize3f(EGfloat v[]);
    static void multMat2fMat2f(EGfloat r[4], const EGfloat a[4], const EGfloat b[4]);
    static void multMat2fVec2f(EGfloat r[2], const EGfloat v[2], const EGfloat m[4]);
    static void multMat3fMat3f(EGfloat r[9], const EGfloat a[9], const EGfloat b[9]);
    static void multMat3fVec3f(EGfloat r[3], const EGfloat v[3], const EGfloat m[9]);
    static void multMat4fMat4f(EGfloat r[16], const EGfloat a[16], const EGfloat b[16]);
    static void multMat4fVec4f(EGfloat r[4], const EGfloat v[4], const EGfloat m[16]);    
    static void multMat4fVec4fHomogenousDivide(EGfloat r[4], const EGfloat v[4], const EGfloat m[16]);
    static void multMat4f(EGfloat m[16], const EGfloat l[16]);
    static void identityMat3f(EGfloat m[9]);
    static void identityMat4f(EGfloat m[16]);
    static void makeScaleMat4f(EGfloat m[16], EGfloat x, EGfloat y, EGfloat z);
    static void scaleMat4f(EGfloat m[16], EGfloat x, EGfloat y, EGfloat z);
    static void makeTranslateMat4f(EGfloat m[16], EGfloat x, EGfloat y, EGfloat z);
    static void translateMat4f(EGfloat m[16], EGfloat x, EGfloat y, EGfloat z);
    static void rotateMat4f(EGfloat m[16], EGfloat angle, EGfloat ax, EGfloat ay, EGfloat az);
    static void makeFrustumMat4f(EGfloat m[16], EGfloat left, EGfloat right, EGfloat bottom, EGfloat top, EGfloat zNear, EGfloat zFar);
    static void frustumMat4f(EGfloat m[16], EGfloat left, EGfloat right, EGfloat bottom, EGfloat top, EGfloat zNear, EGfloat zFar);
    static void makeLookAtMat4f(EGfloat m[16], EGfloat eyex, EGfloat eyey, EGfloat eyez, EGfloat centerx, EGfloat centery, EGfloat centerz, EGfloat upx, EGfloat upy, EGfloat upz);
    static void lookAtMat4f(EGfloat m[16], EGfloat eyex, EGfloat eyey, EGfloat eyez, EGfloat centerx, EGfloat centery, EGfloat centerz, EGfloat upx, EGfloat upy, EGfloat upz);
    static void transposeMat3f(EGfloat r[9], EGfloat m[9]);
    static void transposeMat3f(EGfloat m[9]);
    static void transposeMat4f(EGfloat r[16], EGfloat m[16]);
    static void transposeMat4f(EGfloat m[16]);
    static int invertMat3f(EGfloat inverse[9], const EGfloat src[9]);
    static int invertMat4f(EGfloat inverse[16], const EGfloat src[16]);
    static void mat4toMat3f(EGfloat r[9], EGfloat m[16]);
    static void makeRotateMat3f(EGfloat rot[9], EGfloat angle, EGfloat ax, EGfloat ay, EGfloat az);
    static void makeRotateMat4f(EGfloat rot[16], EGfloat angle, EGfloat ax, EGfloat ay, EGfloat az);
    static void rotateMat3f(EGfloat r[9], EGfloat mat[16], EGfloat angle, EGfloat ax, EGfloat ay, EGfloat az);
    static void rotateMat4f(EGfloat r[16], EGfloat mat[16], EGfloat angle, EGfloat ax, EGfloat ay, EGfloat az);
    static void rot4fToQuat4f(EGfloat q[4], EGfloat r[4]);
    static void quat4fToRot4f(EGfloat rot[4], EGfloat q[4]);
    static void mat3fToQuat4f(EGfloat q[4], EGfloat mat[9]);
    static void quat4fToMat3f(EGfloat mat[9], EGfloat q[4]);
    static void scaleQuat4f(EGfloat r[4], EGfloat q[4], EGfloat m);
    static void multQuat4fQuat4f(EGfloat r[4], EGfloat q1[4], EGfloat q2[4]);
    static void addQuat4fQuat4f(EGfloat r[4], EGfloat q1[4], EGfloat q2[4]);
    static void conjugateQuat4f(EGfloat r[4], EGfloat q[4]);
    static void normalizeQuat4f(EGfloat q[4]);
    static void lerpQuat4f(EGfloat r[4], EGfloat q1[4], EGfloat q2[4], EGfloat t);
    static void slerpQuat4f(EGfloat r[4], EGfloat q1[4], EGfloat q2[4], EGfloat t);
    static EGfloat slerpAngleQuat4f(EGfloat q1[4], EGfloat q2[4]);
    static void project(EGfloat r[3], const EGfloat v[3], const EGfloat finalMatrix[16]);
    static void sub3f(EGfloat r[3], const EGfloat v1[3], const EGfloat v2[3]);
    static void cross3f(EGfloat n[3], const EGfloat v1[3], const EGfloat v2[3]);
    static void cross3f(EGfloat n[3], const EGfloat a[3], const EGfloat b[3], const EGfloat c[3]);
    static EGfloat cross3fz(const EGfloat a[3], const EGfloat b[3], const EGfloat c[3]);
    static EGIntersectType intersectPoint2DBox2D(EGfloat a[2], EGfloat b_lo[2], EGfloat b_hi[2]);
    static EGIntersectType intersectBox2DBox2D(EGfloat a_lo[2], EGfloat a_hi[2], EGfloat b_lo[2], EGfloat b_hi[2]);
    static EGIntersectType intersectPoint3DBox3D(EGfloat a[3], EGfloat b_lo[3], EGfloat b_hi[3]);
    static EGIntersectType intersectBox3DBox3D(EGfloat a_lo[3], EGfloat a_hi[3], EGfloat b_lo[3], EGfloat b_hi[3]);
    static EGIntersectType intersectLines(EGfloat a1x, EGfloat a1y, EGfloat a2x, EGfloat a2y,
                                          EGfloat b1x, EGfloat b1y, EGfloat b2x, EGfloat b2y,
                                          EGfloat *ix, EGfloat *iy);
    static int intersectRayTriangle(EGfloat orig[3], EGfloat dir[3],
                                    EGfloat vert0[3], EGfloat vert1[3], EGfloat vert2[3],
                                    EGfloat *t, EGfloat *u, EGfloat *v);
    static EGfloat haversine(EGfloat lat1, EGfloat lon1, EGfloat lat2, EGfloat lon2);
};
