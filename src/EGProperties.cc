/*
 *  EGProperties.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGUtil.h"
#include "EGResource.h"
#include "EGProperties.h"


/* EGProperties */

static char class_name[] = "EGProperties";

EGbool EGProperties::debug = false;

EGProperties::EGProperties() {}

EGProperties::EGProperties(EGResourcePtr rsrc)
{
    load(rsrc);
}

EGProperties::~EGProperties() {}

void EGProperties::load(EGResourcePtr rsrc)
{
    char buf[256];
    
    // read data using readLine
    while (rsrc->readLine(buf, sizeof(buf))) {
        EGstring line(buf);
        if (line.size() == 0 || line[0] == '#') {
            continue;
        }
        size_t eqoffset = line.find_first_of("=");
        if (eqoffset != std::string::npos) {
            EGstring key = EGUtil::trim(line.substr(0, eqoffset));
            EGstring val = EGUtil::trim(line.substr(eqoffset+1));
            if (debug) {
                EGDebug("%s:%s \"%s\"=\"%s\"\n", class_name, __func__, key.c_str(), val.c_str());
            }
            map.insert(EGPropertiesMapEntry(key, val));
        } else {
            EGError("%s:%s parse error: %s\n", class_name, __func__, buf);
        }
    }
    rsrc->close();
}

EGsize EGProperties::size() { return map.size(); }

EGbool EGProperties::propertyExists(EGstring key)
{
    EGPropertiesMap::iterator mi = map.find(key);
    return (mi != map.end());
}

EGstring EGProperties::getProperty(EGstring key)
{
    EGPropertiesMap::iterator mi = map.find(key);
    return (mi != map.end()) ? (*mi).second : "";
}

void EGProperties::setProperty(EGstring key, EGstring val)
{
    map.insert(EGPropertiesMapEntry(key, val));
}

