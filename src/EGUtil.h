//
//  EGUtil.h
//
//  Copyright (c) 2008 - 2012, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
//

#ifndef EGUTIL_H_
#define EGUTIL_H_

#include "EGColor.h"

typedef std::vector<std::string> EGMessage;
typedef std::map<std::string,std::string> EGMessageResult;

typedef enum {
    EGPlatformUnknown,
    EGPlatformMac,
    EGPlatformIos,
    EGPlatformAndroid,
    EGPlatformLinux,
    EGPlatformWin32,
} EGPlatform;

typedef enum {
    EGTrimOptionsLeading = 0x1,
    EGTrimOptionsTrailing = 0x2,
    EGTrimOptionsBoth = 0x3
} EGTrimOptions;

class EGUtil
{
public:
    static const char* PLATFORMS[];
    static const char* HEX_DIGITS;
    
    static EGenum platform();
    static std::string platformName();
    static EGuint calcCRC(std::string filename);
    static std::string replace(std::string src, std::string target, std::string replace);
    static size_t splitInplace(char *buf, char **data, size_t numFields, const char* sep);
    static std::vector<std::string> split(const char *str, const char *sep);
    static std::string join(std::vector<std::string> vec, const char *sep);
    static std::string join(std::set<std::string> set, const char *sep);
    static std::string jsonStringEncode(std::string input);
    static std::string urlEncode(const std::string &c);
    static std::string urlEncodeSpace(const std::string &c);
    static std::string toCamelCase(EGstring fontName);
    static std::string fromCamelCase(EGstring fontName);
    static std::string trim(std::string str, EGenum options = EGTrimOptionsBoth);
    static EGMessageResult parseResult(std::string result);
    static EGMessage parseMessage(std::string message);
    static std::string stringifyResult(EGMessageResult result);
    static std::string stringifyMessage(EGMessage message);
    static void hexDecode(std::string hex, unsigned char *buf, size_t len);
    static std::string hexEncode(const unsigned char *buf, size_t len);
    static std::string char2hex(char dec);
    static void HSVtoRGB(int &r, int &g, int &b, int h, int s, int v);
    static EGColor colorForTemperature(EGfloat tempc, EGfloat alphaFactor);
    static tm parseISODateTime(std::string isodatestr);
    static int parseDayOfWeek(char *day_of_week);
    static int parseMonth(char *month);
    static int parseTimeZone(int *offset, const char *s);
    static int parseUnixDate(tm *dt, const char *date_str);
    static int parseRFC822Date(tm *dt, const char *date_str);
    static int parseISO8601Date(tm *dt, const char *date_str);
    static std::string formatISO8601Date(tm *dt);
};

#endif
