/*
 *  EGGraphStormWidgetsNACL.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGGraphStormWidgetsNACL_H
#define EGGraphStormWidgetsNACL_H

class EGGraphStormWidgets;
typedef std::shared_ptr<EGGraphStormWidgets> EGGraphStormWidgetsPtr;
class EGGraphStormWidgetsNACL;
typedef std::shared_ptr<EGGraphStormWidgetsNACL> EGGraphStormWidgetsNACLPtr;

typedef std::map<PP_Instance,EGGraphStormWidgetsNACLPtr> EGGraphStormInstanceMap;
typedef std::map<PP_Instance,EGGraphStormWidgetsNACLPtr>::iterator EGGraphStormInstanceIterator;
typedef std::pair<PP_Instance,EGGraphStormWidgetsNACLPtr> EGGraphStormInstanceEntry;

/* EGGraphStormWidgetsNACL */

class EGGraphStormWidgetsNACL
{
public:
    static const EGuint HZ = 30;
    
    static EGGraphStormInstanceMap instances;
    
    EGGraphStormWidgetsPtr widgets;
    PP_Instance instance;
    PP_Resource context;
    pthread_t rsrcThread;
    
    EGMutex messageLock;
    std::list<std::string> messageOutbox;
    
    volatile bool graphicsInited;
    volatile bool rsrcLoaded;
    volatile bool appInited;
    volatile bool inSwapBuffers;
    
    EGGraphStormWidgetsNACL(PP_Instance g_instance);
    
    void graphicsInit(int width, int height);
    void appInit();
    void processEvent(PP_Resource input_event);
    void message(std::string message);
    void display(EGbool reschedule);
    
    static void postMessage(void *userdata, int);
    static void displaySchedule(void *userdata, int);
    static void displayDoneSchedule(void *userdata, int);
    static void displayDone(void *userdata, int);
};

#endif
