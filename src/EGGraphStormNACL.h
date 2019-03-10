// See LICENSE for license details.

#pragma once

class EGGraphApp;
typedef std::shared_ptr<EGGraphApp> EGGraphAppPtr;
class EGGraphStormNACL;
typedef std::shared_ptr<EGGraphStormNACL> EGGraphStormNACLPtr;

typedef std::map<PP_Instance,EGGraphStormNACLPtr> EGGraphStormInstanceMap;
typedef std::map<PP_Instance,EGGraphStormNACLPtr>::iterator EGGraphStormInstanceIterator;
typedef std::pair<PP_Instance,EGGraphStormNACLPtr> EGGraphStormInstanceEntry;

/* EGGraphStormNACL */

class EGGraphStormNACL
{
public:
    static const EGuint HZ = 30;
    
    static EGGraphStormInstanceMap instances;

    EGGraphAppPtr graphapp;
    PP_Instance instance;
    PP_Resource context;
    pthread_t rsrcThread;
    
    EGMutex messageLock;
    std::list<std::string> messageOutbox;

    EGint bufferWidth;
    EGint bufferHeight;

    volatile bool stepping;
    volatile bool graphicsInited;
    volatile bool rsrcLoaded;
    volatile bool appInited;
    volatile bool inSwapBuffers;
    
    EGGraphStormNACL(PP_Instance g_instance);

    void graphicsInit(int width, int height);
    void appInit();
    void processEvent(PP_Resource input_event);
    void message(std::string message);
    void display(EGbool reschedule);

    static void postMessage(void *userdata, int);
    static void step(void *userdata, int);
    static void displaySchedule(void *userdata, int);
    static void displayDoneSchedule(void *userdata, int);
    static void displayDone(void *userdata, int);
};
