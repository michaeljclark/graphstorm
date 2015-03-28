/*
 *  NSStubs.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef NSStubs_H
#define NSStubs_H


/* NSStubs */

class NSStubs
{
public:
    static std::string getTempDir();
    static std::string getHomeDir();
    static std::string getPublicDir();
    static std::string localizedString(std::string key);
};

#endif
