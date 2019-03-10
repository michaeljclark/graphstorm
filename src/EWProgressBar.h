// See LICENSE for license details.

#pragma once

class EWProgressBar;
typedef std::shared_ptr<EWProgressBar> EWProgressBarPtr;

/* EWProgressBar  */

class EWProgressBar : public EWWidget
{
protected:
    static const int MIN_PROGRESSBAR_LENGTH;
    
    EGColor progressBarColor;
    EGint progressBarThickness;
    EGfloat progress;
    
public:
    EWProgressBar(EGenum widgetFlags = 0);
    virtual ~EWProgressBar();
    
    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual void setDefaults(const char *class_name);    
    virtual const char* widgetTypeName();

    virtual void setProgressBarColor(EGColor progressBarColor);
    virtual void setProgressBarThickness(EGint progressBarThickness);
    virtual EGint getProgressBarThickness();

    virtual EGColor getProgressBarColor();
    virtual void setProgress(EGfloat progress);
    virtual EGfloat getProgress();

    virtual EGSize calcMinimumSize();
    virtual void layout();
};
