// See LICENSE for license details.

#pragma once

/* EGGraphRendererParamNodeShape */

enum EGGraphRendererParamNodeShape {
    EGGraphRendererParamNodeShapePoint,
    EGGraphRendererParamNodeShapeCube,
};

enum EGGraphRendererParamNodeSize {
    EGGraphRendererParamNodeSizeConstant,
    EGGraphRendererParamNodeSizeWeight,
};

/* EGGraphRendererParams */

template <typename VecType>
struct EGGraphRendererParams
{
    VecType minBound;
    VecType maxBound;
    VecType viewBound;
    VecType viewOffset;
    
    EGGraphRendererParamNodeShape defaultNodeShape;
    EGGraphRendererParamNodeSize defaultNodeSize;
    EGColor defaultNodeColor;
    EGfloat defaultNodePointSize;
    EGbool defaultNodeShowLabels;
    
    EGColor defaultEdgeColor;
    EGfloat defaultEdgeLineWidth;
    EGbool defaultEdgeShowLabels;
    
    EGbool defaultShowBounds;
    
    EGint viewDimensions;
    std::vector<EGint> visibleDimensions;
    
    bool operator==(const EGGraphRendererParams<VecType> &o)
    {
        return defaultNodeShape == o.defaultNodeShape &&
               defaultNodeSize == o.defaultNodeSize &&
               defaultNodeColor == o.defaultNodeColor &&
               defaultNodePointSize == o.defaultNodePointSize &&
               defaultNodeShowLabels == o.defaultNodeShowLabels &&
               defaultEdgeColor == o.defaultEdgeColor &&
               defaultEdgeLineWidth == o.defaultEdgeLineWidth &&
               defaultEdgeShowLabels == o.defaultEdgeShowLabels &&
               defaultShowBounds == o.defaultShowBounds &&
               viewDimensions == o.viewDimensions;
    }

    bool operator!=(const EGGraphRendererParams<VecType> &o)
    {
        return !(*this == o);
    }
};
