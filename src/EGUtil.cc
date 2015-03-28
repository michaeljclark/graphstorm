//
//  EGUtil.cc
//
//  Copyright (c) 2008 - 2012, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
//

#include <stdio.h>
#include <time.h>

#include <string>
#include <map>

#include <zlib.h>

#include "EG.h"
#include "EGUtil.h"


/* EGUtil */

static char class_name[] = "EGUtil";

const char* EGUtil::HEX_DIGITS = "0123456789ABCDEF";

const char* EGUtil::PLATFORMS[] = {
    "unknown",
    "mac",
    "ios",
    "android",
    "linux",
    "win32"
};

EGenum EGUtil::platform()
{
#if TARGET_OS_MAC && !TARGET_OS_IPHONE
    return EGPlatformMac;
#elif TARGET_OS_IPHONE
    return EGPlatformIos;
#elif ANDROID
    return EGPlatformAndroid;
#elif __linux__
    return EGPlatformLinux;
#elif WIN32
    return EGPlatformWin32;
#else
    return EGPlatformUnknown;
#endif
}

std::string EGUtil::platformName()
{
    return PLATFORMS[platform()];
}

EGuint EGUtil::calcCRC(std::string filename)
{
    EGuint crc = (EGuint)crc32(0, Z_NULL, 0);
    FILE *infile;
    
    if ((infile = fopen(filename.c_str(), "rb")) == NULL) {
        EGError("%s:%s couldn't open file %s\n", class_name, __func__, filename.c_str());
        return 0;
    }
    
    char buf[32768];
    EGsize bytesRead;
    while ((bytesRead = fread(buf, 1, sizeof(buf), infile)) > 0) {
        crc = (EGuint)crc32(crc, (const Bytef*)buf, (EGuint)bytesRead);
    }
    fclose(infile);
    
    return crc;
}

#if defined (_WIN32)
/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

extern "C"
char *
strsep (char **stringp,
	 const char *delim)
{
	register char *s;
	register const char *spanp;
	register int c, sc;
	char *tok;

	if ((s = *stringp) == NULL)
		return (NULL);
	for (tok = s;;) {
		c = *s++;
		spanp = delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				*stringp = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}

#endif

std::string EGUtil::replace(std::string src, std::string target, std::string replace)
{
    if (target.length() == 0) return src;
    if (src.length() == 0) return src;
    
    size_t idx = 0;
    for (;;) {
        idx = src.find( target, idx);
        if (idx == std::string::npos)  break;
        
        src.replace(idx, target.length(), replace);
        idx += replace.length();
    }
    return src;
}

size_t EGUtil::splitInplace(char *buf, char **data, size_t numFields, const char* sep)
{
    size_t i = 0;
    char *token;
    while ((token = strsep(&buf, sep)) != NULL) {
        if (i < numFields) {
            data[i++] = token;
        } else {
            break;
        }
    }
    return i;
}

std::vector<std::string> EGUtil::split(const char *str, const char *sep)
{
    char *tofree = strdup(str);
    char *buf = tofree;
    char *token;
    std::vector<std::string> list;
    while ((token = strsep(&buf, sep)) != NULL) {
        list.push_back(token);
    }
    free(tofree);
    return list;
}

std::string EGUtil::join(std::vector<std::string> vec, const char *sep)
{
    std::stringstream ss;
    EGuint i = 0;
    for (std::vector<std::string>::iterator vi = vec.begin(); vi != vec.end(); vi++) {
        if (i != 0) ss << sep;
        ss << *vi;
        i++;
    }
    return ss.str();
}

std::string EGUtil::join(std::set<std::string> set, const char *sep)
{
    std::stringstream ss;
    EGuint i = 0;
    for (std::set<std::string>::iterator si = set.begin(); si != set.end(); si++) {
        if (i != 0) ss << sep;
        ss << *si;
        i++;
    }
    return ss.str();
}

std::string EGUtil::jsonStringEncode(std::string input)
{
    std::stringstream ss;
    for (std::string::const_iterator iter = input.begin(); iter != input.end(); iter++) {
        switch (*iter) {
            case '"': ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '/': ss << "\\/"; break;
            case '\b': ss << "\\b"; break;
            case '\f': ss << "\\f"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default: ss << *iter; break;
        }
    }
    return ss.str();
}

std::string EGUtil::urlEncode(const std::string &c)
{
    
    std::string escaped="";
    EGsize max = c.length();
    for (EGsize i = 0; i < max; i++)
    {
        if ( (48 <= c[i] && c[i] <= 57) ||//0-9
            (65 <= c[i] && c[i] <= 90) ||//abc...xyz
            (97 <= c[i] && c[i] <= 122) || //ABC...XYZ
            (c[i]=='~' || c[i]=='!' || c[i]=='*' || c[i]=='(' || c[i]==')' || c[i]=='\'')
            )
        {
            escaped.append( &c[i], 1);
        }
        else
        {
            escaped.append("%");
            escaped.append(char2hex(c[i]));
        }
    }
    return escaped;
}

std::string EGUtil::urlEncodeSpace(const std::string &c)
{
    
    std::string escaped = "";
    EGsize max = c.length();
    for(EGsize i = 0; i < max; i++)
    {
        if ( c[i] != ' ')
        {
            escaped.append( &c[i], 1);
        }
        else
        {
            escaped.append("%");
            escaped.append(char2hex(c[i]));
        }
    }
    return escaped;
}

std::string EGUtil::trim(std::string str, EGenum options)
{
    int first_nonws = -1;
    int last_nonws = 0;
    for (int i = 0; i < (int)str.length(); i++) {
        char c = str[i];
        if (c != ' ' && c != '\t') {
            if (first_nonws == -1) {
                first_nonws = i;
            }
            last_nonws = i + 1;
        }
    }
    if (first_nonws == -1) {
        first_nonws = 0;
    }
    int start = (options & EGTrimOptionsLeading) ? first_nonws : 0;
    int len = (options & EGTrimOptionsTrailing) ? last_nonws - start : (int)str.length() - start;
    return str.substr(start, len);
}

std::string EGUtil::toCamelCase(EGstring fontName)
{
    char l = '\0';
    std::stringstream ss;
    for (EGstring::iterator i = fontName.begin(); i != fontName.end(); i++) {
        char c = *i;
        if (c != ' ') {
            ss << ((i == fontName.begin() || l == ' ') ? toupper(c) : c);
        }
        l = c;
    }
    return ss.str();
}

std::string EGUtil::fromCamelCase(EGstring fontName)
{
    char l = '\0';
    std::stringstream ss;
    for (EGstring::iterator i = fontName.begin(); i != fontName.end(); i++) {
        char c = *i;
        if (islower(l) && isupper(c)) {
            ss << " ";
        }
        ss << c;
        l = c;
    }
    return ss.str();
}

EGMessageResult EGUtil::parseResult(std::string result)
{
    EGMessageResult map;
    
    size_t current;
    size_t next = -1;
    do {
        current = next + 1;
        next = result.find_first_of(",", current);
        std::string keyvalpair = result.substr(current, next - current);
        size_t eqoffset = keyvalpair.find_first_of("=");
        std::string key, val;
        if (eqoffset == std::string::npos) {
            key = keyvalpair;
            val = "true";
        } else {
            key = keyvalpair.substr(0, eqoffset);
            val = keyvalpair.substr(eqoffset+1);
        }
        map[key] = val;
    } while (next != std::string::npos);
    
    return map;
}

EGMessage EGUtil::parseMessage(std::string message)
{
    EGMessage list;
    
    size_t current;
    size_t next = -1;
    do {
        current = next + 1;
        next = message.find_first_of(",", current);
        list.push_back(message.substr(current, next - current));
    } while (next != std::string::npos);
    
    return list;
}

std::string EGUtil::stringifyResult(EGMessageResult result)
{
    std::stringstream r;
    int i = 0;
    for (EGMessageResult::iterator mri = result.begin(); mri != result.end(); mri++) {
        if (i++ != 0) r << ",";
        r << (*mri).first << "=" << (*mri).second;
    }
    return r.str();
}

std::string EGUtil::stringifyMessage(EGMessage message)
{
    std::stringstream r;
    int i = 0;
    for (EGMessage::iterator mi = message.begin(); mi != message.end(); mi++) {
        if (i++ != 0) r << ",";
        r << (*mi);
    }
    return r.str();
}

std::string EGUtil::hexEncode(const unsigned char *buf, size_t len)
{
    std::string hex;
    for (size_t i = 0; i < len; i++) {
        unsigned char b = buf[i];
        hex.append(HEX_DIGITS + ((b >> 4) & 0x0F), 1);
        hex.append(HEX_DIGITS + (b & 0x0F), 1);
    }
    return hex;
}

void EGUtil::hexDecode(std::string hex, unsigned char *buf, size_t len)
{
    for (size_t i = 0; i < hex.length()/2 && i < len; i++) {
        const char tmp[3] = { hex[i*2], hex[i*2+1], 0 };
        *buf++ = (EGubyte)strtoul(tmp, NULL, 16);
    }
}

std::string EGUtil::char2hex(char dec)
{
    char dig1 = (dec&0xF0)>>4;
    char dig2 = (dec&0x0F);
    if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48inascii
    if (10<= dig1 && dig1<=15) dig1+=97-10; //a,97inascii
    if ( 0<= dig2 && dig2<= 9) dig2+=48;
    if (10<= dig2 && dig2<=15) dig2+=97-10;
    
    std::string r;
    r.append(&dig1, 1);
    r.append(&dig2, 1);
    return r;
}

void EGUtil::HSVtoRGB(int &r, int &g, int &b, int h, int s, int v)
{
    EGint f, p, q, t;
    
    if(s == 0) {
        r = g = b = v;
        return;
    }
    
    f = ((h%60)*255)/60;
    h /= 60;
    
    p = (v * (256 - s))/256;
    q = (v * (256 - (s * f) / 256)) /256;
    t = (v * (256 - (s * (256-f))/256))/256;
    
    switch( h ) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        default: r = v; g = p; b = q; break;
    }
}

EGColor EGUtil::colorForTemperature(EGfloat tempc, EGfloat alphaFactor)
{
    // H values in the range 280-cold down to 0-hot.
    // cold clamped at -15.0c, hot clamped at 35.0c
    if (tempc < -15.0f) tempc = -15.0f;
    if (tempc > 35.0f) tempc = 35.0f;
    
    // calculate hue on color wheel -15cc to 35.0c map to 280 down to 0
    EGint hue = (EGint)(280.0f - 280.0f * ((tempc + 15.0f) / 50.0f));
    
    EGint r, g, b;
    EGUtil::HSVtoRGB(r, g, b, hue, 192, 192);
    return EGColor(r/255.0f, g/255.0f, b/255.0f, alphaFactor);
}

tm EGUtil::parseISODateTime(std::string isodatestr)
{
    // parse YYYYMMDDHHMISS
    tm dt;
    memset(&dt, 0, sizeof(dt));
    if (isodatestr.length() >= 4) {
        dt.tm_year = atoi(isodatestr.substr(0, 4).c_str()) - 1900;
    }
    if (isodatestr.length() >= 6) {
        dt.tm_mon = atoi(isodatestr.substr(4, 2).c_str()) - 1;
    }
    if (isodatestr.length() >= 8) {
        dt.tm_mday = atoi(isodatestr.substr(6, 2).c_str());
    }
    if (isodatestr.length() >= 10) {
        dt.tm_hour = atoi(isodatestr.substr(8, 2).c_str());
    }
    if (isodatestr.length() >= 12) {
        dt.tm_min = atoi(isodatestr.substr(10, 2).c_str());
    }
    if (isodatestr.length() >= 14) {
        dt.tm_sec = atoi(isodatestr.substr(12, 2).c_str());
    }
    return dt;
}

static const char day_snames[7][4] =
{
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat",
    "Sun"
};

static const char month_snames[12][4] =
{
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
};

struct EETimeZoneEntry {
    std::string name;
    int hours;
    int minutes;
    
    EETimeZoneEntry() : name(), hours(0), minutes(0) {}
    EETimeZoneEntry(std::string name, int hours, int minutes) : name(name), hours(hours), minutes(minutes) {}
    EETimeZoneEntry(const EETimeZoneEntry &ent) : name(ent.name), hours(ent.hours), minutes(ent.minutes) {}
};

typedef std::map<std::string,EETimeZoneEntry> EETimeZoneMap;

struct EETimeZoneDB
{
    EETimeZoneMap db;
    
    EETimeZoneDB()
    {
        add(EETimeZoneEntry("UTC", 0, 0));
    }
    
    void add(EETimeZoneEntry ent)
    {
        db[ent.name] = ent;
    }
    
    EETimeZoneEntry* get(std::string tz)
    {
        EETimeZoneMap::iterator enti;
        if ((enti = db.find(tz)) != db.end()) {
            return &((*enti).second);
        }
        return NULL;
    }
};

EETimeZoneDB timezones;

int EGUtil::parseDayOfWeek(char *day_of_week)
{
    int i;
    for(i = 0; i < 7; i++) {
        if(strcmp(day_of_week, day_snames[i]) == 0) {
            return i;
        }
    }
    return -1;
}

int EGUtil::parseMonth(char *month)
{
    int i;
    for(i = 0; i < 12; i++) {
        if(strcmp(month, month_snames[i]) == 0) {
            return i;
        }
    }
    return -1;
}

int EGUtil::parseTimeZone(int *offset, const char *s)
{
    if (strlen(s) == 5 && (s[0] == '+' || s[0] == '-')) {
        char hrs[3], mins[3];
        strncpy(hrs, s+1, 2);
        strncpy(mins, s+3, 2);
        hrs[2] = '\0';
        mins[2] = '\0';
        *offset = atoi(hrs) * (s[0] == '+' ? 1 : -1) * 3600 + atoi(mins);
        return 0;
    }
        
    EETimeZoneEntry *ent = timezones.get(s);
    
    if (ent) {
        *offset = ent->hours * 3600 + ent->minutes * 60;
        return 0;
    } else {
        EGError("%s:%s couldn't find zone: %s\n", class_name, __func__, s);        
    }
    return -1;
}

int EGUtil::parseUnixDate(tm *dt, const char *date_str)
{
    int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0, day_of_week = 0, offset = 0;
    char day_of_week_str[4];
    char month_str[4];
    char ampm[3];
    char zone[6];
    int err = 0;
    
    /* Mon Sep 19 01:51:30 UTC 2011 */
    if (sscanf(date_str, "%3s%*c %3s %u %u:%u:%u %5s %u",
               (char*)&day_of_week_str, (char*)&month_str, &day, 
               &hour, &minute, &second, (char*)&zone, &year) == 8) {}
    else return -1;
    
    day_of_week = parseDayOfWeek(day_of_week_str);
    month = parseMonth(month_str);
    err = parseTimeZone(&offset, zone);
    
    if (day_of_week == -1 || month == -1 || err != 0) {
        return -1;
    }
    
    dt->tm_year   = year - 1900;
    dt->tm_mon    = month;
    dt->tm_mday   = day;
    dt->tm_hour   = hour + (strcmp(ampm, "pm") == 0 ? 12 : 0);
    dt->tm_min    = minute;
    dt->tm_sec    = second;
    dt->tm_wday   = day_of_week;
#if ! defined (_WIN32) && ! defined (__native_client__)
    dt->tm_gmtoff = offset;
    dt->tm_zone   = zone;
#endif
    return 0;
}

int EGUtil::parseRFC822Date(tm *dt, const char *date_str)
{
    int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0, day_of_week = 0, offset = 0;
    char day_of_week_str[4];
    char month_str[4];
    char ampm[3];
    char zone[6];
    int err = 0;

    //Day of week, seconds
    if (sscanf(date_str, "%3s%*c %u %3s %u %u:%u:%u %5s",
               (char*)&day_of_week_str, &day, (char*)&month_str,
               &year, &hour, &minute, &second, (char*)&zone) == 8) {}
    //Day of week, no seconds, am/pm
    else if (sscanf(date_str, "%3s%*c %u %3s %u %u:%u %2s %5s",
                    (char*)&day_of_week_str, &day, (char*)month_str,
                    &year, &hour, &minute, (char*)&ampm, (char*)&zone) == 8) {}
    //Day of week, no seconds
    else if (sscanf(date_str, "%3s%*c %u %3s %u %u:%u %5s",
                    (char*)&day_of_week_str, &day, (char*)month_str,
                    &year, &hour, &minute, (char*)&zone) == 7) {}
    //No day of week, seconds
    else if (sscanf(date_str, "%u %3s %u %u:%u:%u %5s",
                    &day, (char*)&month_str,  &year, &hour, &minute,
                    &second, (char*)&zone) == 7) {}
    //No day of week, no seconds
    else if (sscanf(date_str, "%u %3s %u %u:%u %5s",
                    &day, (char*)&month_str, &year,  &hour, &minute,
                    (char*)&zone) == 6) {}
    else return -1;
     
    day_of_week = parseDayOfWeek(day_of_week_str);
    month = parseMonth(month_str);
    err = parseTimeZone(&offset, zone);
     
    if (day_of_week == -1 || month == -1 || err != 0) {
        return -1;
    }
    
    dt->tm_year   = year - 1900;
    dt->tm_mon    = month;
    dt->tm_mday   = day;
    dt->tm_hour   = hour + (strcmp(ampm, "pm") == 0 ? 12 : 0);
    dt->tm_min    = minute;
    dt->tm_sec    = second;
    dt->tm_wday   = day_of_week;
#if ! defined (_WIN32) && ! defined (__native_client__)
    dt->tm_gmtoff = offset;
    dt->tm_zone   = zone;
#endif
    return 0;
}

int EGUtil::parseISO8601Date(tm *dt, const char *date_str)
{
    int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0, subsecond = 0, day_of_week = 0, offset = 0;
    static char zone[7];
    int err = 0;
        
    //Day of week, seconds
    if (sscanf(date_str, "%u-%u-%uT%u:%u:%u.%u%3s:%2s",
               &year, &month, &day, &hour, &minute, &second, &subsecond, &zone[0], &zone[3]) == 9) {
        err = parseTimeZone(&offset, zone);
    } else if (sscanf(date_str, "%u-%u-%uT%u:%u:%uZ",
               &year, &month, &day, &hour, &minute, &second) == 6) {
        memcpy(zone, "+0000", 6);
    }
    else return -1;
        
    
    if (day_of_week == -1 || month == -1 || err != 0) {
        return -1;
    }
    
    dt->tm_year   = year - 1900;
    dt->tm_mon    = month - 1;
    dt->tm_mday   = day;
    dt->tm_hour   = hour;
    dt->tm_min    = minute;
    dt->tm_sec    = second;
    dt->tm_wday   = day_of_week;
#if ! defined (_WIN32) && ! defined (__native_client__)
    dt->tm_gmtoff = offset;
    dt->tm_zone   = zone;
#endif
    
    return 0;
}

std::string EGUtil::formatISO8601Date(tm *dt)
{
    char buf[32];
#if ! defined (_WIN32) && ! defined (__native_client__)
    snprintf(buf, sizeof(buf)-1, "%04u-%02u-%02uT%02u:%02u:%02u.00%c%02d:%02d",
             dt->tm_year + 1900, dt->tm_mon + 1, dt->tm_mday, dt->tm_hour, dt->tm_min, dt->tm_sec,
             dt->tm_gmtoff >= 0 ? '+' : '-', abs((EGint)dt->tm_gmtoff) / 3600, (abs((EGint)dt->tm_gmtoff) % 3600) / 60);
#else
    snprintf(buf, sizeof(buf)-1, "%04u-%02u-%02uT%02u:%02u:%02u.00+00:00",
             dt->tm_year + 1900, dt->tm_mon + 1, dt->tm_mday, dt->tm_hour, dt->tm_min, dt->tm_sec);
#endif
    return std::string(buf);
}

