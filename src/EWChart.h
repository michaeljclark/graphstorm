/*
 *  EWChart.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EWChart_h
#define EWChart_h

class EWChart;
typedef std::shared_ptr<EWChart> EWChartPtr;
typedef std::vector<EWChartPtr> EWChartList;
class EWChartAxis;
typedef std::shared_ptr<EWChartAxis> EWChartAxisPtr;


/* EWChartAxisOrientation */

typedef enum {
    EWChartAxisOrientationHorizontalBottom,
    EWChartAxisOrientationHorizontalTop,
    EWChartAxisOrientationVerticalLeft,
    EWChartAxisOrientationVerticalRight,
} EWChartAxisOrientation;


/* EWChartAxisDirection */

typedef enum {
    EWChartAxisDirectionAscending,
    EWChartAxisDirectionDescending,
} EWChartAxisDirection;


/* EWChartAxis */

class EWChartAxis : public EWWidget
{
public:
    EWChart *chart;
    EWDataSourcePtr datasource;
    EGuint datacolumn;
    EWChartAxisOrientation orientation;
    EWChartAxisDirection direction;
    EGuint precision;
    EGfloat presetScaleMin;
    EGfloat presetScaleMax;
    
    EGfloat minValue;
    EGfloat maxValue;
    EGfloat scaleMin;
    EGfloat scaleMax;
    EGfloat scaleIncr;
    EGfloat scaleRange;
    EGint numLabels;
    EGTextPtr axisLegend;
    std::vector<std::pair<EGfloat,EGText*> > axisLabels;
    
    EWChartAxis(EGenum widgetFlags = 0);
    virtual ~EWChartAxis();

    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual const char* widgetTypeName();

    void setDataSource(EWDataSourcePtr &datasource);
    EWDataSourcePtr getDataSource() const;

    void setScaleMin(EGfloat scaleMin);
    void getScaleMin() const;

    void setScaleMax(EGfloat scaleMax);
    void getScaleMax() const;

    virtual EGSize calcMinimumSize();
    virtual void layout();

    virtual EGbool mouseEvent(EGMouseEvent *evt);
    virtual EGbool keyEvent(EGKeyEvent *evt);
};


/* EWChart */

class EWChart : public EWWidget
{
public:
    EWDataSourcePtr dataSource;
    EWChartAxisPtr xAxis;
    EWChartAxisPtr yAxis;

    EWChart(EGenum widgetFlags = 0);
    virtual ~EWChart();

    static const EGClass* classFactory();
    virtual const EGClass* getClass() const;
    virtual const char* widgetTypeName();

    void setDataSource(EWDataSourcePtr &datasource);
    EWDataSourcePtr getDataSource() const;
    
    EWChartAxisPtr getXAxis() const;
    EWChartAxisPtr getYAxis() const;

    virtual EGSize calcMinimumSize();
    virtual void layout();

    virtual EGbool mouseEvent(EGMouseEvent *evt);
    virtual EGbool keyEvent(EGKeyEvent *evt);
};


#endif
