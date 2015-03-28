/*
 *  EGProperties.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGProperties_H
#define EGProperties_H

class EGResource;
typedef std::shared_ptr<EGResource> EGResourcePtr;
class EGProperties;
typedef std::shared_ptr<EGProperties> EGPropertiesPtr;
typedef std::map<EGstring,EGstring> EGPropertiesMap;
typedef std::pair<EGstring,EGstring> EGPropertiesMapEntry;

/* EGProperties */

class EGProperties
{
protected:
    static EGbool debug;
    
    EGPropertiesMap map;
    
public:
    EGProperties();
    EGProperties(EGResourcePtr rsrc);
    virtual ~EGProperties();
    
    void load(EGResourcePtr rsrc);
    EGsize size();
    EGbool propertyExists(EGstring key);
    EGstring getProperty(EGstring key);
    void setProperty(EGstring key, EGstring val);
};

#endif
