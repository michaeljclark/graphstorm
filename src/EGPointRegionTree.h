// See LICENSE for license details.

#pragma once

//
// Point Region Tree Node Container
//


/* EGPointRegionTreeNodeContainer */

template <typename VecType, EGbool centerZeroHalfPow2 = true>
class EGPointRegionTreeNodeContainer
{
public:
    VecType center;
    VecType half;
    
    EGPointRegionTreeNodeContainer() : center(), half() {}
        
    EGPointRegionTreeNodeContainer(const EGPointRegionTreeNodeContainer<VecType> &o) : center(o.center), half(o.half) {}
    
    virtual ~EGPointRegionTreeNodeContainer() {}
    
    virtual EGbool withinBounds(VecType &vec)
    {
        return (vec >= center - half && vec <= center + half);
    }
    
    /* round floating point mantissa to nearest power of 2 */
    static float roundPow2(float val)
    {
        /* get mantissa and exponent */
        int exp;
        float m = frexp(val, &exp);
        
        /* get integer reciprocal of mantissa */
        int r = (int)(1.0f / m);
        
        /* round integer reciprocal to nearest power of 2 */
        /* source: bit twiddling hacks: http://graphics.stanford.edu/~seander/bithacks.html */
        r--;
        r |= r >> 1;
        r |= r >> 2;
        r |= r >> 4;
        r |= r >> 8;
        r |= r >> 16;
        r++;
        
        /* reconstruct float */
        return (1.0f / r) * pow(2.0f, exp);
    }
    
    virtual void setBounds(VecType &minbound, VecType &maxbound)
    {
        /* NOTE: we use a reasonable minimum half bound of 1/64 just incase initial values are 0.0f */
        typename VecType::value_type max = 0.015625f;
        
        if (centerZeroHalfPow2) {
            /* initial bounds are centered around 0.0f and round half extent to power of 2 fraction */
            /* This improve precision of boundary comparisons and avoids error when computing sub node bounds */
            for (EGsize i = 0; i < half.dim(); i++) {
                typename VecType::value_type v = (std::max)(fabs(minbound[i]),fabs(maxbound[i]));
                if (v > max) max = v;
            }
            half = roundPow2(max);
            center = 0.0f;
        } else {
            /* NOTE: this second approach suffers from floating point precision issues with boundary comparisons
               and finding node by value can fail due to loss of precision when computing sub node bounds */
            half = (maxbound - minbound) * 0.5f;
            for (EGsize i = 0; i < half.dim(); i++) {
                if (half[i] > max) max = half[i];
            }
            half = max;
            center = minbound + half;
        }
    }
};


//
// Point Region Tree
//

/* EGPointRegionTree */

template <typename DataType, typename PointAdaptor, typename VecType,
          typename ContainerType, typename BranchStorage, int BranchEntries, int LeafEntries, typename LeafStorage>
class EGPointRegionTree : public EGTree<DataType>
{
public:
    typedef EGTreeNodeBase<ContainerType> EGTreeNodeType;
    typedef EGTreeBranchNode<ContainerType,BranchStorage,BranchEntries> EGBranchNodeType;
    typedef EGTreeLeafNode<ContainerType,LeafStorage> EGLeafNodeType;
    
protected:
    PointAdaptor adaptor;
    EGTreeNodeType *rootNode;
    
    EGPointRegionTree() : rootNode(new EGLeafNodeType(NULL)) {}
    
    virtual ~EGPointRegionTree()
    {
        if (rootNode) delete rootNode;
    }
    
    virtual EGint childIndex(EGTreeNodeType *node, VecType &vec) = 0;
    virtual VecType childDirection(EGint idx) = 0;
    virtual EGint oppositeIndex(EGint idx) = 0;
    
    virtual EGTreeNodeType* upwardsSplit(EGTreeNodeType *node, EGint idx)
    {
        /* upwards split should only be called on the root node */
        assert(node == this->rootNode);
        
        /* create new branch node, set this as the root and make it the parent of the existing node */
        EGTreeNodeType* newNode = this->rootNode = new EGBranchNodeType(NULL);
        node->setParent(newNode);
        
        /*
         * Expand container center and half size on the new node.
         * The old node gets added as a child in the opposite quadrant to
         * the out of bound quadrant we are expanding towards
         */
        newNode->container.half = node->container.half * 2.0f;
        newNode->container.center = node->container.center + node->container.half * childDirection(idx);
        EGint childIdx = oppositeIndex(idx);
        newNode->setChild(childIdx, node);
        
        return newNode;
    }
    
    virtual EGTreeNodeType* downwardsSplit(EGTreeNodeType *node)
    {
        /* downwards split is always on a leaf */
        assert(node->getNodeKind() == EGTreeNodeLeaf);
        
        /* create new branch node and set it in our parent */
        EGTreeNode* parent = node->getParent();
        EGTreeNodeType* newNode = new EGBranchNodeType(parent);
        if (node == rootNode) {
            /* if the existing node is the root, then replace the root node */
            rootNode = newNode;
        } else {
            /* otherwise replace the leaf in our parent with the new branch */
            EGoffset idx = parent->findChild(node);
            /* node must exist in parent */
            assert(idx != -1);
            parent->setChild(idx, newNode);
        }
        
        /* new branch node inherits center and half dimension of the splitting leaf node */
        newNode->container = node->container;
        
        /* transfer items into our new child leaves */
        EGLeafNodeType *leaf = static_cast<EGLeafNodeType*>(node);
        EGsize elemcount = leaf->storage.count();
        for (EGsize elem = 0; elem < elemcount; elem++) {
            DataType data = leaf->storage.at(elem);
            VecType vec = adaptor(data);
            EGint idx = childIndex(newNode, vec);
            EGTreeNodeType *child = static_cast<EGTreeNodeType*>(newNode->childAt(idx));
            if (!child) child = newChild(newNode, idx);
            static_cast<EGLeafNodeType*>(child)->storage.insert(data);
        }
        
        /* delete old leaf */
        delete node;
        
        return newNode;
    }
    
    virtual void getLeafBounds(EGLeafNodeType *leaf, VecType &minbound, VecType &maxbound)
    {
        minbound = (std::numeric_limits<typename VecType::value_type>::max)();
        maxbound = -(std::numeric_limits<typename VecType::value_type>::max)();
        for (EGsize elem = 0; elem < leaf->storage.count(); elem++) {
            VecType vec = adaptor(leaf->storage.at(elem));
            for (EGsize i = 0; i < vec.dim(); i++) {
                if (vec[i] < minbound[i]) minbound[i] = vec[i];
                if (vec[i] > maxbound[i]) maxbound[i] = vec[i];
            }
        }
    }
    
    virtual EGTreeNodeType* newChild(EGTreeNodeType *node, EGint idx)
    {
        EGTreeNodeType *child = new EGLeafNodeType(node);
        node->setChild(idx, child);
        child->container.half = node->container.half / 2.0f;
        child->container.center = node->container.center + child->container.half * childDirection(idx);
        return child;
    }
    
    virtual EGbool insert(EGTreeNodeType *node, const DataType& data, EGbool boundsCheck = true)
    {
        /* check point is within our node bounds */
        VecType vec = adaptor(data);
        EGbool withinbounds = !boundsCheck || node->container.withinBounds(vec);
        
        if (node->getNodeKind() == EGTreeNodeBranch)
        {
            /* if we are within bounds then insert into child node */
            if (withinbounds) {
                EGint idx = childIndex(node, vec);
                EGTreeNodeType *child = static_cast<EGTreeNodeType*>(node->childAt(idx));
                if (!child) child = newChild(node, idx);
                return insert(child, data, /* boundsCheck = */ false);
            }
        }
        else if (node->getNodeKind() == EGTreeNodeLeaf)
        {
            EGLeafNodeType *leaf = static_cast<EGLeafNodeType*>(node);
            
            /* insert into the node if we are within bounds and the node has space
             * special case, the root node is a leaf that is not full */
            if ((withinbounds || node == rootNode) && leaf->storage.count() < leaf->storage.capacity())
            {
                EGbool result = leaf->storage.insert(data);
                
                /* special case, if we are the leaf root node we re-compute bounds after each insert */
                if (node == rootNode) {
                    VecType minbound, maxbound;
                    getLeafBounds(leaf, minbound, maxbound);
                    node->container.setBounds(minbound, maxbound);
                }
                return result;
            }
            
            /* no space, if we are within bounds then then perform a downwards split */
            if (withinbounds) {
                return insert(downwardsSplit(node), data, /* boundsCheck = */ false);
            }
        }
        
        /* outside bounds of this node */
        if (node == rootNode) {
            /* if we are at the root node then perform an upwards split */
            return insert(upwardsSplit(node, childIndex(node, vec)), data);
        } else {
            /* if we are not at the root then keep inserting upwards
             *
             * Note: This code is to allow points to be re-inserted in the node they were
             *       removed from. Assuming spatial coherence, this will result in a shorter
             *       tree traversal than re-inserting into the root node.
             */
            return insert(static_cast<EGTreeNodeType*>(node->getParent()), data);
        }
    }
    
    virtual EGLeafNodeType* find(EGTreeNodeType *node, const DataType& data, EGbool boundsCheck = true)
    {
        /* check point is within our node bounds */
        VecType vec = adaptor(data);
        EGbool withinbounds = !boundsCheck || node->container.withinBounds(vec);
        
        if (node->getNodeKind() == EGTreeNodeBranch)
        {
            /* if we are within bounds then search child node */
            if (withinbounds) {
                EGint idx = childIndex(node, vec);
                EGTreeNodeType *child = static_cast<EGTreeNodeType*>(node->childAt(idx));
                return find(child, data);
            }
        }
        else if (node->getNodeKind() == EGTreeNodeLeaf)
        {
            return static_cast<EGLeafNodeType*>(node);
        }
        return NULL;
    }
    
    virtual void prune(EGTreeNode *node)
    {
        if (node->getNodeKind() == EGTreeNodeLeaf)
        {
            EGLeafNodeType *leaf = static_cast<EGLeafNodeType*>(node);
            if (leaf->storage.count() > 0) return;
        }
        else if (node->getNodeKind() == EGTreeNodeBranch)
        {
            for (EGsize idx = 0; idx < node->childCount(); idx++) {
                if (node->childAt(idx) != NULL) return;
            }
        }
        
        /* if we get here then we have a leaf with no data or a branch with no children
         * so we can delete this node and erase link in our parent or convert emprt root
         * branch to a leaf node */
        if (node == rootNode) {
            if (node->getNodeKind() == EGTreeNodeBranch) {
                /* convert empty root branch node to a leaf node */
                delete rootNode;
                rootNode = new EGLeafNodeType(NULL);
            }
        } else {
            /* erase node in our parent */
            EGTreeNode* parent = node->getParent();
            EGoffset idx = parent->findChild(node);
            
            /* node must exist in parent */
            assert(idx != -1);
            parent->setChild(idx, NULL);
            
            /* continue pruning upwards */
            prune(parent);
        }
    }
    
    virtual void print(EGTreeNodeType *node, EGint depth, EGsize idx)
    {
        char fmt[8];
        char indent[128];

        snprintf(fmt, sizeof(fmt), "%%%ds", depth * 4);
        snprintf(indent, sizeof(indent), fmt, "");

		EGDebug("%s %s[%d] depth=%d center=%s half=%s\n",
                indent, (node->getNodeKind() == EGTreeNodeLeaf ? "leaf" : "branch"), idx, depth,
                ((std::string)node->container.center).c_str(), ((std::string)node->container.half).c_str());
        
        if (node->getNodeKind() == EGTreeNodeBranch)
        {
            for (EGsize idx = 0; idx < node->childCount(); idx++) {
                EGTreeNodeType *child = static_cast<EGTreeNodeType*>(node->childAt(idx));
                if (child) {
                    print(child, depth+1, idx);
                }
            }
        }
        else if (node->getNodeKind() == EGTreeNodeLeaf)
        {
            EGLeafNodeType *leaf = static_cast<EGLeafNodeType*>(node);
            EGsize elemcount = leaf->storage.count();
            for (EGsize elem = 0; elem < elemcount; elem++) {
                DataType data = leaf->storage.at(elem);
                VecType vec = adaptor(data);
                EGDebug("%s     data[%d] = %s\n", indent, elem, ((std::string)vec).c_str());
            }
        }
    }
    
    virtual void visit(EGTreeNode *node, EGTreeNodeVisitor& visitor)
    {
        visitor.preDescend(node);
        if (node->getNodeKind() == EGTreeNodeBranch)
        {
            for (EGsize idx = 0; idx < node->childCount(); idx++) {
                EGTreeNodeType *child = static_cast<EGTreeNodeType*>(node->childAt(idx));
                if (child && visitor.shouldDescend(child)) {
                    visit(child, visitor);
                }
            }
        }
        visitor.postDescend(node);
    }

public:
    
    virtual EGTreeNode* getRootNode()
    {
        return rootNode;
    };
    
    virtual void clear()
    {
        delete rootNode;
        rootNode = new EGLeafNodeType(NULL);
    }
    
    virtual EGTreeNode* find(DataType& data)
    {
        return find(rootNode, data);
    }
    
    virtual EGbool insert(DataType& data)
    {
        return insert(rootNode, data);
    }
    
    virtual EGbool erase(DataType& data)
    {
        EGLeafNodeType *leaf = find(rootNode, data);
        if (leaf) {
            EGbool result = leaf->storage.erase(data);
            prune(leaf);
            return result;
        }
        return false;
    }
    
    virtual void print()
    {
        print(rootNode, 0, 0);
    }
    
    virtual void visit(EGTreeNodeVisitor& visitor)
    {
        visit(rootNode, visitor);
    }

};
