/*
 *  EGTrackBall.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGTrackBall_H
#define EGTrackBall_H

class EGTrackBall;
typedef std::shared_ptr<EGTrackBall> EGTrackBallPtr;


/* EGTrackBall */

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


#endif /* EGTRACKBALL_H_ */
