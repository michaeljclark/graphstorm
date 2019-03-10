// See LICENSE for license details.

#pragma once

#include "EGTree.h"
#include "EGPointRegionTree.h"


/* EGPointRegionOctree */

template <class DataType, class PointAdaptor, typename VecType,
          typename ContainerType = EGPointRegionTreeNodeContainer<VecType>,
          typename BranchStorage = EGTreeNodeStorageNone<DataType>,
          int BranchEntries = 8, int LeafEntries = 32, typename LeafStorage = EGTreeNodeStoragMany<DataType,LeafEntries>>
class EGPointRegionOctree : public EGPointRegionTree<DataType,PointAdaptor,VecType,ContainerType,BranchStorage,BranchEntries,LeafEntries,LeafStorage>
{
public:
    typedef EGTreeNodeBase<ContainerType> EGTreeNodeType;
    typedef EGTreeBranchNode<ContainerType,BranchStorage,BranchEntries> EGBranchNodeType;
    typedef EGTreeLeafNode<ContainerType,LeafStorage> EGLeafNodeType;
    
    static const EGint XL_YL_ZL = 0;
    static const EGint XL_YL_ZH = 1;
    static const EGint XL_YH_ZL = 2;
    static const EGint XL_YH_ZH = 3;
    static const EGint XH_YL_ZL = 4;
    static const EGint XH_YL_ZH = 5;
    static const EGint XH_YH_ZL = 6;
    static const EGint XH_YH_ZH = 7;
    
    EGPointRegionOctree() {}
    virtual ~EGPointRegionOctree() {}
    
protected:
    EGint childIndex(EGTreeNodeType *node, VecType &vec)
    {
        VecType &center = node->container.center;
        VecType delta = vec - center;
        
        if (delta[0] <= 0 && delta[1] <= 0 && delta[2] <= 0)      return XL_YL_ZL;
        else if (delta[0] <= 0 && delta[1] <= 0 && delta[2] >= 0) return XL_YL_ZH;
        else if (delta[0] <= 0 && delta[1] >= 0 && delta[2] <= 0) return XL_YH_ZL;
        else if (delta[0] <= 0 && delta[1] >= 0 && delta[2] >= 0) return XL_YH_ZH;
        else if (delta[0] >= 0 && delta[1] <= 0 && delta[2] <= 0) return XH_YL_ZL;
        else if (delta[0] >= 0 && delta[1] <= 0 && delta[2] >= 0) return XH_YL_ZH;
        else if (delta[0] >= 0 && delta[1] >= 0 && delta[2] <= 0) return XH_YH_ZL;
        else if (delta[0] >= 0 && delta[1] >= 0 && delta[2] >= 0) return XH_YH_ZH;
		else return -1;
    }
    
    VecType childDirection(EGint idx)
    {
        switch (idx) {
            case XL_YL_ZL: return VecType(-1.0f, -1.0f, -1.0f);
            case XL_YL_ZH: return VecType(-1.0f, -1.0f, 1.0f);
            case XL_YH_ZL: return VecType(-1.0f, 1.0f, -1.0f);
            case XL_YH_ZH: return VecType(-1.0f, 1.0f, 1.0f);
            case XH_YL_ZL: return VecType(1.0f, -1.0f, -1.0f);
            case XH_YL_ZH: return VecType(1.0f, -1.0f, 1.0f);
            case XH_YH_ZL: return VecType(1.0f, 1.0f, -1.0f);
            case XH_YH_ZH: return VecType(1.0f, 1.0f, 1.0f);
            default: return VecType(0.0f, 0.0f, 0.0f);
        }
    }

    EGint oppositeIndex(EGint idx)
    {
        switch (idx) {
            case XL_YL_ZL: return XH_YH_ZH;
            case XL_YL_ZH: return XH_YH_ZL;
            case XL_YH_ZL: return XH_YL_ZH;
            case XL_YH_ZH: return XH_YL_ZL;
            case XH_YL_ZL: return XL_YH_ZH;
            case XH_YL_ZH: return XL_YH_ZL;
            case XH_YH_ZL: return XL_YL_ZH;
            case XH_YH_ZH: return XL_YL_ZL;
            default: return -1;
        }
    }
};
