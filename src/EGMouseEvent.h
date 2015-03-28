/*
 *  EGMouseEvent.h
 *
 *  Copyright (c) 2008 - 2012, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGMouseEvent_H
#define EGMouseEvent_H

/* EGMouseEvent */

class EGMouseEvent : public EGEvent
{
public:
    static EGEventQualifier PRESSED;
    static EGEventQualifier RELEASED;
    static EGEventQualifier MOTION;
    
    static const int LEFT_BUTTON   = 0;
    static const int MIDDLE_BUTTON = 1;
    static const int RIGHT_BUTTON  = 2;
    
    EGint x, y;
    EGint button;
    
    EGMouseEvent(EGEventQualifier q, EGint x, EGint y);
    
    EGMouseEvent(EGEventQualifier q, EGint x, EGint y, EGint button);
    
    std::string toString();
};

#endif
