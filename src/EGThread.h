/*
 *  EGThread.h
 *
 *  Copyright (c) 2008 - 2012, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGTHREAD_H_
#define EGTHREAD_H_

#if defined (_WIN32)

#include <windows.h>
#include <process.h>

#define EGThreadFunc unsigned __stdcall
#define EGThreadExitCode unsigned
typedef CONDITION_VARIABLE EGCondVarType;
typedef CRITICAL_SECTION EGMutexType;
typedef unsigned EGThreadType;

inline EGint EGThreadCreate(unsigned &thread, unsigned (__stdcall *threadfunc) (void*), void* arg) {
    return (EGint)_beginthreadex(NULL, 0, threadfunc, arg, 0, &thread);
}
#define EGThreadExit(rc) _endthreadex(rc)
inline void EGThreadJoin(unsigned &thread) { WaitForSingleObject( &thread, INFINITE ); CloseHandle(&thread); }

inline void EGCondVarInit(EGCondVarType &cond) { InitializeConditionVariable(&cond); }
inline void EGCondVarDestroy(EGCondVarType &cond) { }
inline void EGCondVarWait(EGCondVarType &cond, CRITICAL_SECTION &mutex) { SleepConditionVariableCS(&cond, &mutex, INFINITE); }
inline void EGCondVarSignal(EGCondVarType &cond) { WakeConditionVariable(&cond); }
inline void EGCondVarBroadcast(EGCondVarType &cond) { WakeAllConditionVariable(&cond); }
inline void EGMutexInit(EGMutexType &mutex) { InitializeCriticalSection(&mutex); }
inline void EGMutexDestroy(EGMutexType &mutex) { DeleteCriticalSection(&mutex); }
inline void EGMutexLock(EGMutexType &mutex) { EnterCriticalSection(&mutex); }
inline void EGMutexUnlock(EGMutexType &mutex) { LeaveCriticalSection(&mutex); }
#else

#include <pthread.h>

#define EGThreadFunc void*
#define EGThreadExitCode void*
typedef pthread_cond_t EGCondVarType;
typedef pthread_mutex_t EGMutexType;
typedef pthread_t EGThreadType;

inline EGint EGThreadCreate(pthread_t &thread, void* (*threadfunc)(void*), void* arg) {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    return (EGint)pthread_create(&thread, &attr, threadfunc, arg);
}
#define EGThreadExit(rc) do { pthread_exit((void*)rc); return NULL } while (0);
inline void EGThreadJoin(pthread_t &thread) { void *result; pthread_join(thread, &result); }

inline void EGCondVarInit(EGCondVarType &cond) { pthread_cond_init(&cond, NULL); }
inline void EGCondVarDestroy(EGCondVarType &cond) { pthread_cond_destroy(&cond); }
inline void EGCondVarWait(EGCondVarType &cond, pthread_mutex_t &mutex) { pthread_cond_wait(&cond, &mutex); }
inline void EGCondVarSignal(EGCondVarType &cond) { pthread_cond_signal(&cond); }
inline void EGCondVarBroadcast(EGCondVarType &cond) { pthread_cond_broadcast(&cond); }
inline void EGMutexInit(EGMutexType &mutex) { pthread_mutex_init(&mutex, NULL); }
inline void EGMutexDestroy(EGMutexType &mutex) { pthread_mutex_destroy(&mutex); }
inline void EGMutexLock(EGMutexType &mutex) { pthread_mutex_lock(&mutex); }
inline void EGMutexUnlock(EGMutexType &mutex) { pthread_mutex_unlock(&mutex); }

#endif

class EGMutex;
typedef std::shared_ptr<EGMutex> EGMutexPtr;
class EGThread;
typedef std::shared_ptr<EGThread> EGThreadPtr;
class EGCondVar;
typedef std::shared_ptr<EGCondVar> EGCondVarPtr;


/* EGMutex */

class EGMutex {
private:
    EGMutexType mutex;
    friend class EGCondVar;
    
public:
    EGMutex() { EGMutexInit(mutex); }
    ~EGMutex() { EGMutexDestroy(mutex); }
    
    void lock() { EGMutexLock(mutex); }
    void unlock() { EGMutexUnlock(mutex); }
};


/* EGCondVar */

class EGCondVar {
private:
    EGCondVarType cond;
    EGMutexType &mutex;
    
public:
    EGCondVar(EGMutex &mutex) : mutex(mutex.mutex) { EGCondVarInit(cond); }
    ~EGCondVar() { EGCondVarDestroy(cond); }
    
    void wait() { EGCondVarWait(cond, mutex); }
    void signal() { EGCondVarSignal(cond); }
    void broadcast() { EGCondVarBroadcast(cond); }
};


/* EGRunnable */

class EGRunnable {
public:
    virtual ~EGRunnable() {}
    
    virtual EGThreadExitCode run() = 0;
};


/* EGThread */

class EGThread : public EGRunnable {
private:
    EGThreadType thread;
    EGRunnable *runnable;

    static EGThreadFunc threadStart(void *userdata) { return (EGThreadExitCode)((EGRunnable*)userdata)->run(); }

public:
    EGThread() : runnable(static_cast<EGRunnable*>(this)) {}
    EGThread(EGRunnable *runnable) : runnable(runnable) {}
    virtual ~EGThread() {}
    
    EGint start() { return EGThreadCreate(thread, &threadStart, (void*)runnable); }
    void join() { EGThreadJoin(thread); }
};

#endif
