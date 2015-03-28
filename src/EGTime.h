/*
 *  EGTime.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGTime_H
#define EGTime_H

#if defined (_WIN32)

/* EGTime - Windows implementation */

class EGTime
{
private:
    LARGE_INTEGER stime, proc_freq;
    
public:
    EGTime()
    {
        QueryPerformanceFrequency(&proc_freq);
        QueryPerformanceCounter(&stime);
    }
    
    EGTime(long long usec)
    {
        stime.QuadPart = usec;
        proc_freq.QuadPart = 1000000LL;
    }
    
    EGTime(const EGTime &o)
    {
        stime  = o.stime;
        proc_freq  = o.proc_freq;
    }
    
    inline long long getUSec() const
    {
        return (long long)stime.QuadPart * 1000000LL / proc_freq.QuadPart;
    }

    inline EGuint getSec() const
    {
        return (EGuint)(stime.QuadPart / proc_freq.QuadPart);
    }
};


/* EGTimer - Windows implementation */

class EGTimer
{
private:
    LARGE_INTEGER stime, etime, proc_freq;
    
public:
    inline EGTimer() : stime(), etime(), proc_freq() {}
    inline ~EGTimer() {}
    
    inline void start()
    {
        QueryPerformanceCounter(&stime);
    }
    
    inline void stop()
    {
        QueryPerformanceFrequency(&proc_freq);
        QueryPerformanceCounter(&etime);
    }
    
    inline long long getUSec() const
    {
        return (long long)(etime.QuadPart - stime.QuadPart) * 1000000LL / proc_freq.QuadPart;
    }

    inline EGuint getSec() const
    {
        return (EGuint)((etime.QuadPart - stime.QuadPart) / proc_freq.QuadPart);
    }
};

#else

#include <time.h>
#include <sys/time.h>


/* EGTime - UNIX implementation */

class EGTime
{
private:
    struct timeval stime;
    
public:
    EGTime() {
        gettimeofday(&stime, NULL);
    }

    EGTime(long long usec)
    {
        stime.tv_sec = usec / 1000000LL;
        stime.tv_usec = usec % 1000000LL;
    }
    
    EGTime(struct timespec ts)
    {
        stime.tv_sec = ts.tv_sec;
        stime.tv_usec = (int)(ts.tv_nsec * 1000);
    }

    EGTime(const EGTime &o)
    {
        stime  = o.stime;
    }
        
    inline long long getUSec() const
    {
        return (long long)stime.tv_sec * 1000000LL + stime.tv_usec;
    }
    
    inline EGuint getSec() const
    {
        return (EGuint)stime.tv_sec;
    }    
};


/* EGTimer - UNIX implementation */

class EGTimer
{
private:
    struct timeval stime, etime;
    
public:
    inline EGTimer() : stime(), etime() {}
    inline ~EGTimer() {}
    
    inline void start()
    {
        gettimeofday(&stime, NULL);
    }
    
    inline void stop()
    {
        gettimeofday(&etime, NULL);
    }
    
    inline long long getUSec() const
    {
        return (long long)(etime.tv_sec - stime.tv_sec) * 1000000LL + (etime.tv_usec - stime.tv_usec);
    }
    
    inline EGuint getSec() const
    {
        return (EGuint)(etime.tv_sec - stime.tv_sec);
    }    
};

#endif

inline bool operator==(const EGTime& lhs, const EGTime& rhs) { return lhs.getUSec() == rhs.getUSec(); }
inline bool operator!=(const EGTime& lhs, const EGTime& rhs) { return lhs.getUSec() != rhs.getUSec(); }
inline bool operator>(const EGTime& lhs, const EGTime& rhs) { return lhs.getUSec() > rhs.getUSec(); }
inline bool operator<(const EGTime& lhs, const EGTime& rhs) { return lhs.getUSec() < rhs.getUSec(); }
inline bool operator>=(const EGTime& lhs, const EGTime& rhs) { return lhs.getUSec() >= rhs.getUSec(); }
inline bool operator<=(const EGTime& lhs, const EGTime& rhs) { return lhs.getUSec() <= rhs.getUSec(); }

#endif /* EGTIMER_H_ */
