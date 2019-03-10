// See LICENSE for license details.

#pragma once

class EGTrackBall;
typedef std::shared_ptr<EGTrackBall> EGTrackBallPtr;

class EGTrackBall
{
public:
    EGfloat gTrackBallRotation[4];
    EGfloat gRadiusTrackball;
    EGfloat gStartPtTrackball[3];
    EGfloat gEndPtTrackball[3];
    EGint gXCenterTrackball, gYCenterTrackball;

    EGTrackBall();

    void reset();
    void startTrackball(EGint x, EGint y, EGint originX, EGint originY, EGint width, EGint height);
    void rollToTrackball(EGint x, EGint y, EGfloat rot [4]);
    void addToRotationTrackball(EGfloat *dA, EGfloat *A);
};
