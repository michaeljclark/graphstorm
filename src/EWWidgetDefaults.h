/*
 *  EWWidgetDefaults.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EWWidgetDefaults_H
#define EWWidgetDefaults_H

class EGProperties;
typedef std::shared_ptr<EGProperties> EGPropertiesPtr;
class EWWidgetDefaults;
typedef std::shared_ptr<EWWidgetDefaults> EWWidgetDefaultsPtr;

/* EWWidgetDefaults */

class EWWidgetDefaults
{
private:
    EGPropertiesPtr props;
    
public:
    static EGstring DEFAULT_PROPERTIES_FILE;
    
    EWWidgetDefaults();
    EWWidgetDefaults(EGResourcePtr rsrc);
    virtual ~EWWidgetDefaults();
    
    virtual void load(EGResourcePtr rsrc);
    
    virtual EGstring getString(EGstring widgetClass, EGstring widgetKey, EGstring defaultVal = "");
    virtual EGbool getBoolean(EGstring widgetClass, EGstring widgetKey, EGbool defaultVal = false);
    virtual EGint getInteger(EGstring widgetClass, EGstring widgetKey, EGint defaultVal = 0);
    virtual EGfloat getFloat(EGstring widgetClass, EGstring widgetKey, EGfloat defaultVal = 0.0f);
    virtual EGIndent getIndent(EGstring widgetClass, EGstring widgetKey, EGIndent defaultVal = EGIndent());
    virtual EGMargin getMargin(EGstring widgetClass, EGstring widgetKey, EGMargin defaultVal = EGMargin());
    virtual EGPadding getPadding(EGstring widgetClass, EGstring widgetKey, EGPadding defaultVal = EGPadding());
    virtual EGSize getSize(EGstring widgetClass, EGstring widgetKey, EGSize defaultVal = EGSize());
    virtual EGRect getRect(EGstring widgetClass, EGstring widgetKey, EGRect defaultVal = EGRect());
    virtual EGColor getColor(EGstring widgetClass, EGstring widgetKey, EGColor defaultVal = EGColor(1,1,1,1));
    virtual EGImagePtr getImage(EGstring widgetClass, EGstring widgetKey, EGImagePtr defaultVal = EGImagePtr());
};

#endif
