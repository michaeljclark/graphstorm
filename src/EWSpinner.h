// See LICENSE for license details.

#pragma once

class EWSpinner;
typedef std::shared_ptr<EWSpinner> EWSpinnerPtr;

/* EWSpinner */

class EWSpinner : public EWWidget
{
protected:
    static const EGSize SPINNER_SIZE;
    static const EGint NUM_SPINNER_FRAMES;
    
    EGImagePtr spinnerImage;
    EGint spinnerFrame;
    
public:
    EWSpinner(EGenum widgetFlags = 0);
    virtual ~EWSpinner();
    
    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual void setDefaults(const char *class_name);
    virtual const char* widgetTypeName();
    
    virtual EGSize calcMinimumSize();
    virtual void layout();
    virtual void draw();
};
