/*
 *  EWWidgetDefaults.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGUtil.h"
#include "EGColor.h"
#include "EGImage.h"
#include "EGResource.h"
#include "EGProperties.h"

#include "EWWidget.h"
#include "EWWidgetDefaults.h"

/* EWWidgetDefaults */

static char class_name[] = "EWWidgetDefaults";

EGstring EWWidgetDefaults::DEFAULT_PROPERTIES_FILE = "Resources/ui.bundle/defaults.properties";

EWWidgetDefaults::EWWidgetDefaults() : props(new EGProperties())
{
    load(EGResource::getResource(DEFAULT_PROPERTIES_FILE));
}

EWWidgetDefaults::EWWidgetDefaults(EGResourcePtr rsrc) : props(new EGProperties())
{
    load(rsrc);
}

EWWidgetDefaults::~EWWidgetDefaults() {}

void EWWidgetDefaults::load(EGResourcePtr rsrc)
{
    props->load(rsrc);
    EGDebug("%s:%s loaded %d properties\n", class_name, __func__, props->size());
}

EGstring EWWidgetDefaults::getString(EGstring widgetClass, EGstring widgetKey, EGstring defaultVal)
{
    std::string key = widgetClass + std::string(".") + widgetKey;
    if (props->propertyExists(key)) {
        return props->getProperty(key);
    } else if (widgetClass != "*") {
        return getString("*", widgetKey, defaultVal);
    }
    return defaultVal;
}

EGbool EWWidgetDefaults::getBoolean(EGstring widgetClass, EGstring widgetKey, EGbool defaultVal)
{
    std::string key = widgetClass + std::string(".") + widgetKey;
    if (props->propertyExists(key)) {
        EGstring strval = EGUtil::trim(props->getProperty(key));
        return (strval == "true" || strval == "1" || strval == "TRUE");
    } else if (widgetClass != "*") {
        return getBoolean("*", widgetKey, defaultVal);
    }
    return defaultVal;
}

EGint EWWidgetDefaults::getInteger(EGstring widgetClass, EGstring widgetKey, EGint defaultVal)
{
    std::string key = widgetClass + std::string(".") + widgetKey;
    if (props->propertyExists(key)) {
        EGstring strval = props->getProperty(key);
        return atoi(strval.c_str());
    } else if (widgetClass != "*") {
        return getInteger("*", widgetKey, defaultVal);
    }
    return defaultVal;
}
                                   
EGfloat EWWidgetDefaults::getFloat(EGstring widgetClass, EGstring widgetKey, EGfloat defaultVal)
{
    std::string key = widgetClass + std::string(".") + widgetKey;
    if (props->propertyExists(key)) {
        EGstring strval = props->getProperty(key);
        return atof(strval.c_str());
    } else if (widgetClass != "*") {
        return getFloat("*", widgetKey, defaultVal);
    }
    return defaultVal;
}
                                   
EGIndent EWWidgetDefaults::getIndent(EGstring widgetClass, EGstring widgetKey, EGIndent defaultVal)
{
    std::string key = widgetClass + std::string(".") + widgetKey;
    if (props->propertyExists(key)) {
        EGstring strval = props->getProperty(key);
        std::vector<std::string> strvec = EGUtil::split(strval.c_str(), ",");
        if (strvec.size() == 1) {
            EGint indent = atoi(strvec[0].c_str());
            return EGIndent(indent, indent, indent, indent);
        } else if (strvec.size() == 4) {
            EGint top = atoi(strvec[0].c_str());
            EGint bottom = atoi(strvec[1].c_str());
            EGint left = atoi(strvec[2].c_str());
            EGint right = atoi(strvec[3].c_str());
            return EGIndent(top, bottom, left, right);
        }
    } else if (widgetClass != "*") {
        return getIndent("*", widgetKey, defaultVal);
    }
    return defaultVal;
}
                                     
EGMargin EWWidgetDefaults::getMargin(EGstring widgetClass, EGstring widgetKey, EGMargin defaultVal)
{
    return getIndent(widgetClass, widgetKey, defaultVal);
}
                                     
EGPadding EWWidgetDefaults::getPadding(EGstring widgetClass, EGstring widgetKey, EGPadding defaultVal)
{
    return getIndent(widgetClass, widgetKey, defaultVal);
}
                                       
EGSize EWWidgetDefaults::getSize(EGstring widgetClass, EGstring widgetKey, EGSize defaultVal)
{
    std::string key = widgetClass + std::string(".") + widgetKey;
    if (props->propertyExists(key)) {
        EGstring strval = props->getProperty(key);
        std::vector<std::string> strvec = EGUtil::split(strval.c_str(), ",");
        if (strvec.size() == 1) {
            EGint size = atoi(strvec[0].c_str());
            return EGSize(size, size);
        } else if (strvec.size() == 2) {
            EGint width = atoi(strvec[0].c_str());
            EGint height = atoi(strvec[1].c_str());
            return EGSize(width, height);
        }
    } else if (widgetClass != "*") {
        return getSize("*", widgetKey, defaultVal);
    }
    return defaultVal;
}
                                 
EGRect EWWidgetDefaults::getRect(EGstring widgetClass, EGstring widgetKey, EGRect defaultVal)
{
    std::string key = widgetClass + std::string(".") + widgetKey;
    if (props->propertyExists(key)) {
        EGstring strval = props->getProperty(key);
        std::vector<std::string> strvec = EGUtil::split(strval.c_str(), ",");
        if (strvec.size() == 4) {
            EGint x = atoi(strvec[0].c_str());
            EGint y = atoi(strvec[1].c_str());
            EGint width = atoi(strvec[2].c_str());
            EGint height = atoi(strvec[3].c_str());
            return EGRect(x, y, width, height);
        }
    } else if (widgetClass != "*") {
        return getRect("*", widgetKey, defaultVal);
    }
    return defaultVal;
}
                                 
EGColor EWWidgetDefaults::getColor(EGstring widgetClass, EGstring widgetKey, EGColor defaultVal)
{
    std::string key = widgetClass + std::string(".") + widgetKey;
    if (props->propertyExists(key)) {
        EGstring strval = props->getProperty(key);
        if (strval.size() > 0 && strval[0] == '#') {
            return EGColor::fromHex(strval);
        } else {
            std::vector<std::string> strvec = EGUtil::split(strval.c_str(), ",");
            if (strvec.size() == 3) {
                EGfloat r = atof(strvec[0].c_str());
                EGfloat g = atof(strvec[1].c_str());
                EGfloat b = atof(strvec[2].c_str());
                return EGColor(r, g, b, 1);
            } else if (strvec.size() == 4) {
                EGfloat r = atof(strvec[0].c_str());
                EGfloat g = atof(strvec[1].c_str());
                EGfloat b = atof(strvec[2].c_str());
                EGfloat a = atof(strvec[3].c_str());
                return EGColor(r, g, b, a);
            }
        }
    } else if (widgetClass != "*") {
        return getColor("*", widgetKey, defaultVal);
    }
    return defaultVal;
}
                                   
EGImagePtr EWWidgetDefaults::getImage(EGstring widgetClass, EGstring widgetKey, EGImagePtr defaultVal)
{
    std::string key = widgetClass + std::string(".") + widgetKey;
    if (props->propertyExists(key)) {
        EGstring strval = props->getProperty(key);
        return EGImage::createFromResource(EGResource::getResource(strval));
    } else if (widgetClass != "*") {
        return getImage("*", widgetKey, defaultVal);
    }
    return defaultVal;
}
