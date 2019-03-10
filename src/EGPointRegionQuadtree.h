// See LICENSE for license details.

#pragma once

#include "EGTree.h"
#include "EGPointRegionTree.h"


/* EGPointRegionQuadtree */

template <class DataType, class PointAdaptor, class VecType,
          class ContainerType = EGPointRegionTreeNodeContainer<VecType>,
          class BranchStorage = EGTreeNodeStorageNone<DataType>,
          int BranchEntries = 4, int LeafEntries = 32, typename LeafStorage = EGTreeNodeStoragMany<DataType,LeafEntries>>
class EGPointRegionQuadtree : public EGPointRegionTree<DataType,PointAdaptor,VecType,ContainerType,BranchStorage,BranchEntries,LeafEntries,LeafStorage>
{
public:
    typedef EGTreeNodeBase<ContainerType> EGTreeNodeType;
    typedef EGTreeBranchNode<ContainerType,BranchStorage,BranchEntries> EGBranchNodeType;
    typedef EGTreeLeafNode<ContainerType,LeafStorage> EGLeafNodeType;
    
    static const EGint XL_YL = 0;
    static const EGint XL_YH = 1;
    static const EGint XH_YL = 2;
    static const EGint XH_YH = 3;

    EGPointRegionQuadtree() {}
    virtual ~EGPointRegionQuadtree() {}
    
protected:
    EGint childIndex(EGTreeNodeType *node, VecType &vec)
    {
        VecType &center = node->container.center;
        VecType delta = vec - center;
        
        if (delta[0] <= 0 && delta[1] <= 0) return XL_YL;
        else if (delta[0] <= 0 && delta[1] >= 0) return XL_YH;
        else if (delta[0] >= 0 && delta[1] <= 0) return XH_YL;
        else if (delta[0] >= 0 && delta[1] >= 0) return XH_YH;
		else return -1;
    }
    
    VecType childDirection(EGint idx)
    {
        switch (idx) {
            case XL_YL: return VecType(-1.0f, -1.0f);
            case XL_YH: return VecType(-1.0f, 1.0f);
            case XH_YL: return VecType(1.0f, -1.0f);
            case XH_YH: return VecType(1.0f, 1.0f);
            default: return VecType(0.0f, 0.0f);
        }
    }
    
    EGint oppositeIndex(EGint idx)
    {
        switch (idx) {
            case XL_YL: return XH_YH;
            case XL_YH: return XH_YL;
            case XH_YL: return XL_YH;
            case XH_YH: return XL_YL;
            default: return -1;
        }
    }
};
