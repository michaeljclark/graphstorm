/*
 *  EG.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGPlatformNACL.h"
#include "NSStubs.h"

#ifdef ANDROID
#include <android/log.h>
#endif

#if _MSC_VER == 1500
/* work around missing symbol in vs9 */
extern "C" void __report_rangecheckfailure(void) {}
#endif

const char EG::time_format[] = "%Y-%m-%dT%H-%M-%S";
const char EG::error_prefix[] = "*** ERROR *** ";
const char EG::warn_prefix[] = "+++ WARNING +++ ";
const char EG::profile_begin_fmt[] = "%s:%s PROFILE BEGIN\n";
const char EG::profile_end_fmt[] =   "%s:%s PROFILE END time=%lldus\n";

FILE* EG::logfile = NULL;

#if DISTRIBUTION_BUILD
EGbool EG::debug = false;
EGbool EG::profile = false;
EGbool EG::logtofile = false;
EGbool EG::logtoconsole = false;
EGbool EG::logtime = false;
#else
EGbool EG::debug = true;
EGbool EG::profile = true;
EGbool EG::logtofile = false;
EGbool EG::logtoconsole = true;
EGbool EG::logtime = false;
#endif

static std::string getTimeString(const char* format)
{
    char timebuf[128];
    time_t dt;
    time(&dt);
    struct tm *locdt = localtime(&dt);
    strftime(timebuf, sizeof(timebuf), format, locdt);
    return timebuf;
}

void EG::openlog()
{
    if (!logfile) {
#if TARGET_OS_MAC
        std::string logfname = NSStubs::getPublicDir() + std::string("/graphstorm-") + getTimeString(time_format) + std::string(".log");
#else
	std::string logfname = std::string("graphstorm-") + getTimeString(time_format) + std::string(".log");
#endif
        if (!(logfile = fopen(logfname.c_str(), "a"))) {
            printf("*** error opening logfile: %s: %s\n", logfname.c_str(), strerror(errno));
        } else {
            logDebug("opened log file: %s\n", logfname.c_str());
        }
    }
}

void EG::log(const char* msgbuf)
{
#if ! defined (_WIN32)
    if (logtime) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        if (logtofile) {
            openlog();
            fprintf(logfile, "%d.%06d %s", (EGint)tv.tv_sec, (EGint)tv.tv_usec, msgbuf);
            fflush(logfile);
        }
        if (logtoconsole) {
            printf("%d.%06d %s", (EGint)tv.tv_sec, (EGint)tv.tv_usec, msgbuf);
        }
    } else {
#endif
        if (logtofile) {
            openlog();
            fprintf(logfile, "%s", msgbuf);
            fflush(logfile);
        }
        if (logtoconsole) {
#if defined (__native_client__)
            int len = strlen(msgbuf);
            if (len > 0 && msgbuf[len - 1] == '\n') len--;
            PP_Var logmsg = ppb_var_interface->VarFromUtf8(msgbuf, len);
            ppb_console_interface->Log(pp_current_instance, PP_LOGLEVEL_LOG, logmsg);
            ppb_var_interface->Release(logmsg);         
#elif defined (_WIN32) && !defined (_CONSOLE)
            OutputDebugStringA(msgbuf);
#elif defined (ANDROID)
            EGApplication *app = EGApplication::getApplication();
            __android_log_print(ANDROID_LOG_INFO, app ? app->name() : "EGDebug", "%s", msgbuf);
#else
            printf("%s", msgbuf);
#endif
        }
#if ! defined (_WIN32)
    }
#endif
}

void EG::logDebug(const char* fmt, ...)
{
    char msgbuf[2048];
    va_list ap;
    
    va_start(ap, fmt);
#ifdef __native_client__
    vsprintf(msgbuf, fmt, ap);
#else
    vsnprintf(msgbuf, sizeof(msgbuf), fmt, ap);
#endif
    va_end(ap);
    
    log(msgbuf);
}

void EG::logError(const char* fmt, ...)
{
    char msgbuf[2048];
    va_list ap;
    
    strcpy(msgbuf, error_prefix);
    va_start(ap, fmt);
#ifdef __native_client__
    vsprintf(msgbuf + strlen(error_prefix), fmt, ap);
#else
    vsnprintf(msgbuf + strlen(error_prefix), sizeof(msgbuf) - strlen(error_prefix), fmt, ap);
#endif
    va_end(ap);
    
    log(msgbuf);
}

void EG::logWarn(const char* fmt, ...)
{
    char msgbuf[2048];
    va_list ap;
    
    strcpy(msgbuf, warn_prefix);
    va_start(ap, fmt);
#ifdef __native_client__
    vsprintf(msgbuf + strlen(warn_prefix), fmt, ap);
#else
    vsnprintf(msgbuf + strlen(warn_prefix), sizeof(msgbuf) - strlen(warn_prefix), fmt, ap);
#endif
    va_end(ap);
    
    log(msgbuf);
}
