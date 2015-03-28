/*
 *  EG.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EG_H
#define EG_H

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

#include <algorithm>

#if defined( _LIBCPP_VERSION )
#define __EG_USE_CXX11_INCLUDES__ 1
#else
#if defined(__GNUC__)
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 2)
#error "requires g++ >= 4.2"
#elif __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ <= 5)
#define __EG_USE_TR1_INCLUDES__ 1
#define __EG_USE_TR1_NAMESPACE__ 1
#else
#define __EG_USE_CXX11_INCLUDES__ 1
#endif
#endif
#endif

#if defined(_MSC_VER)
#if _MSC_VER < 1500
#error "requires Visual Studio >= 2008"
#else
#define __EG_USE_CXX11_INCLUDES__ 1
#if _MSC_VER < 1600
#define __EG_USE_TR1_NAMESPACE__ 1
#endif
#endif
#endif

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

#if defined (__EG_USE_CXX11_INCLUDES__)
#include <utility>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <type_traits>
#endif

#if defined (__EG_USE_TR1_INCLUDES__)
#include <tr1/utility>
#include <tr1/memory>
#include <tr1/unordered_set>
#include <tr1/unordered_map>
#include <tr1/functional>
#include <tr1/type_traits>
#endif

#if defined (__EG_USE_TR1_NAMESPACE__)
#define __EG_TR1_COMPAT_NAMESPACE_BEGIN__ namespace tr1 {
#define __EG_TR1_COMPAT_NAMESPACE_END__ }
#else
#define __EG_TR1_COMPAT_NAMESPACE_BEGIN__
#define __EG_TR1_COMPAT_NAMESPACE_END__
#endif

#if defined (__EG_USE_TR1_NAMESPACE__)
#define function tr1::function
#define weak_ptr tr1::weak_ptr
#define shared_ptr tr1::shared_ptr
#define enable_shared_from_this tr1::enable_shared_from_this
#define unordered_set tr1::unordered_set
#define make_shared tr1::make_shared
//#define forward tr1::forward
#endif

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

#endif
