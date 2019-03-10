// See LICENSE for license details.

#pragma once

// emscripten
#if EMSCRIPTEN
#define USE_VERTEX_32 0
#define USE_OPENCL 0
#define USE_HARFBUZZ 1
#elif __native_client__
#define USE_VERTEX_32 0
#define USE_OPENCL 0
#define USE_HARFBUZZ 1
#elif _WIN32
#define USE_DIRECT_WRITE 1
#define USE_OPENCL 1
#define USE_VERTEX_32 1
#else
#define USE_OPENCL 1
#define USE_VERTEX_32 1
#define USE_HARFBUZZ 1
#endif

#define ALIGN_ROUND_UP(x,page_size) ( (((size_t)(x)) + page_size-1)  & (~(page_size-1)) )

/* very simple tests at the moment */

#if defined (_WIN32)
#include <Windows.h>
#define fileno _fileno
#define atoll _atoi64
#define snprintf _snprintf
#define strdup _strdup
#define strncasecmp _strnicmp
#define usleep(x) Sleep((x)/1000);
#define timegm _mkgmtime
#define __func__  __FUNCTION__
#define roundf(n) floorf(n + 0.5f)
#pragma warning( disable : 4351 )
#endif

#if defined (ANDROID)
#include <time64.h>
#define timegm(x) (time_t)timegm64(x)
#endif

/* include common headers */

#if defined (__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#include <QuartzCore/QuartzCore.h>
#endif

#if defined (__APPLE__) || defined(__unix__) || defined(__linux__)
#include <libgen.h>
#include <sys/time.h>
#include <unistd.h>
#endif

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <cstddef>
#include <cmath>
#include <cassert>
#include <cstdarg>
#include <ctime>
#include <thread>
#include <atomic>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <utility>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <type_traits>
#include <algorithm>

#if !defined (PATH_MAX)
#define PATH_MAX FILENAME_MAX
#endif

#if !defined (DIR_SEPARATOR)
#if defined (WIN32)
#define DIR_SEPARATOR  "\\"
#else
#define DIR_SEPARATOR  "/"
#endif
#endif

#if defined (_WIN32)
#define EG_CALLBACK __stdcall
#else
#define EG_CALLBACK
#endif

typedef float          EGfloat;
typedef double         EGdouble;
typedef int            EGint;
typedef long           EGlong;
typedef long long      EGllong;
typedef size_t         EGsize;
typedef off_t          EGoffset;
typedef unsigned int   EGuint;
typedef unsigned long  EGulong;
typedef unsigned long long EGullong;
typedef unsigned int   EGenum;
typedef short          EGshort;
typedef unsigned short EGushort;
typedef char           EGbyte;
typedef unsigned char  EGubyte;
typedef void           EGvoid;
typedef std::string    EGstring;
typedef unsigned char  EGbool;

class EG
{
private:
    static FILE* logfile;
    
    static void openlog();
    
public:
    static const char time_format[];
    static const char error_prefix[];
    static const char warn_prefix[];
    static const char profile_begin_fmt[];
    static const char profile_end_fmt[];
    static EGbool debug;
    static EGbool profile;
    static EGbool logtofile;
    static EGbool logtoconsole;
    static EGbool logtime;
    
    static void log(const char* msg);
    static void logDebug(const char* fmt, ...);
    static void logError(const char* fmt, ...);
    static void logWarn(const char* fmt, ...);
};

#define EGDebug(fmt, ...) \
if (EG::debug) { EG::logDebug(fmt, __VA_ARGS__); }

#define EGError(fmt, ...) \
EG::logError(fmt, __VA_ARGS__);

#define EGWarn(fmt, ...) \
EG::logWarn(fmt, __VA_ARGS__);

#define EGProfileBegin() \
EGTimer timer; \
if (EG::profile) { \
    EG::logDebug(EG::profile_begin_fmt, class_name, __func__); \
    timer.start(); \
}

#define EGProfileEnd() \
if (EG::profile) { \
    timer.stop(); \
    EG::logDebug(EG::profile_end_fmt, class_name, __func__, timer.getUSec()); \
}

#define EGProfileNameBegin(name) \
EGTimer timer; \
if (EG::profile) { \
    EG::logDebug(EG::profile_begin_fmt, __func__, name); \
    timer.start(); \
}

#define EGProfileNameEnd(name) \
if (EG::profile) { \
    timer.stop(); \
    EG::logDebug(EG::profile_end_fmt, __func__, name, timer.getUSec()); \
}
