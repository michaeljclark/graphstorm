// See LICENSE for license details.

#pragma once

/* EGGraphDataLabel */

class EGGraphDataLabel : public EGGraphData
{
protected:
    EGstring label;
    
    EGGraphDataLabel(EGstring label) : label(label) {}
    
public:
    static const EGuint ID;
    
    EGstring getLabel() { return label; }

    static EGGraphDataPair create(EGstring label);
};


/* EGGraphDataColor */

class EGGraphDataColor : public EGGraphData
{
protected:
    EGColor color;

    EGGraphDataColor(EGColor color) : color(color) {}

public:
    static const EGuint ID;
    
    EGColor getColor() { return color; }
    
    static EGGraphDataPair create(EGColor color);
    static EGGraphDataPair create(EGstring colorstr);
};


/* EGGraphDataSize */

class EGGraphDataSize : public EGGraphData
{
protected:
    EGfloat size;
    
    EGGraphDataSize(EGfloat size) : size(size) {}
    
public:
    static const EGuint ID;
    
    EGfloat getSize() { return size; }
    
    static EGGraphDataPair create(EGfloat size);
};
