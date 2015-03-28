/*
 *  EGTree.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGTree_H
#define EGTree_H


//
// Tree Node Storage
//


/* EGTreeNodeStorage */

template <typename DataType>
class EGTreeNodeStorage
{
public:
    EGTreeNodeStorage() {}
    virtual ~EGTreeNodeStorage() {}
    
    virtual EGsize capacity() const = 0;
    virtual EGsize count() const = 0;
    virtual DataType at(EGsize index) = 0;
    virtual EGint find(const DataType &node) = 0;
    virtual EGbool insert(const DataType &node) = 0;
    virtual EGbool erase(const DataType &node) = 0;
};


/* EGTreeNodeStorageNone */

template <typename DataType>
class EGTreeNodeStorageNone : public EGTreeNodeStorage<DataType>
{    
public:
    EGTreeNodeStorageNone() {}
    virtual ~EGTreeNodeStorageNone() {}
    
    virtual EGsize capacity() const { return 0; };
    virtual EGsize count() const { return 0; };
    virtual DataType at(EGsize index) { return DataType(); }
    virtual EGint find(const DataType &node) { return -1; }
    virtual EGbool insert(const DataType &node) { return false; }
    virtual EGbool erase(const DataType &node) { return false; }
};


/* EGTreeNodeStoragMany */

template <typename DataType, EGint N>
class EGTreeNodeStoragMany : public EGTreeNodeStorage<DataType>
{
protected:
    EGint n;
    DataType nodes[N];
    
public:
    EGTreeNodeStoragMany() : n(0) { memset(nodes, 0, sizeof(nodes)); }
    virtual ~EGTreeNodeStoragMany() {}

    virtual EGsize capacity() const { return N; };
    virtual EGsize count() const { return n; };
    
    virtual DataType at(EGsize index)
    {
        return nodes[index];
    }
    
    virtual EGint find(const DataType &node)
    {
        DataType *p = std::find(nodes, nodes + n, node);
        if (p == nodes + n) return -1;
        return (int)(p - nodes) / sizeof(DataType);
    }
    
    virtual EGbool insert(const DataType &node)
    {
        if (n == N) return false;
        nodes[n++] = node;
        return true;
    }
    
    virtual EGbool erase(const DataType &node)
    {
        DataType *p = std::find(nodes, nodes + n, node);
        if (p == nodes + n) return false;
        EGint o = (EGint)(p - nodes) / sizeof(DataType);
        n--;
        if (o < n) {
            memmove(p, p + 1, sizeof(DataType) * (n - o));
        }
        return true;
    }
};


//
// Tree Node
//


/* EGTreeNodeKind */

typedef enum {
    EGTreeNodeLeaf,
    EGTreeNodeBranch,
} EGTreeNodeKind;


/* EGTreeNode */

class EGTreeNode
{
protected:
    EGTreeNode *parent;
    
public:
    EGTreeNode(EGTreeNode *parent) : parent(parent) {}
    virtual ~EGTreeNode() {}

    virtual EGTreeNode* getParent() { return parent; }
    virtual void setParent(EGTreeNode* parent) { this->parent = parent; }

    virtual EGTreeNodeKind getNodeKind() const = 0;
    virtual EGsize childCount() const = 0;
    virtual EGTreeNode* childAt(EGsize index) = 0;
    virtual void setChild(EGsize index, EGTreeNode* child) = 0;
    virtual EGoffset findChild(EGTreeNode *child) = 0;
};


/* EGTreeNodeBase */

template<typename ContainerType>
class EGTreeNodeBase : public EGTreeNode
{
public:
    ContainerType container;
    
    EGTreeNodeBase(EGTreeNode *parent) : EGTreeNode(parent), container() {}
    virtual ~EGTreeNodeBase() {}
    
    virtual ContainerType& getContainer() { return container; }
};


/* EGTreeBranchNode */

template<typename ContainerType, typename BranchStorageType, EGint N>
class EGTreeBranchNode : public EGTreeNodeBase<ContainerType>
{
protected:
    EGTreeNode *children[N];
    
public:
    BranchStorageType storage;
    
    EGTreeBranchNode(EGTreeNode *parent) : EGTreeNodeBase<ContainerType>(parent), storage()
    {
        memset(children, 0, sizeof(EGTreeNodeBase<ContainerType> *) * N);
    }
    
    virtual ~EGTreeBranchNode()
    {
        for (int i = 0; i < N; i ++) {
            if (children[i]) delete children[i];
        }
    }
    
    EGTreeNodeKind getNodeKind() const { return EGTreeNodeBranch; }
    virtual EGsize childCount() const { return N; };
    virtual EGTreeNode* childAt(EGsize index) { return children[index]; }
    virtual void setChild(EGsize index, EGTreeNode* child) { children[index] = child; }
    
    virtual EGoffset findChild(EGTreeNode *child)
    {
        for (EGsize idx = 0; idx < N; idx++) if (children[idx] == child) return (EGoffset)idx;
        return -1;
    }
};


/* EGTreeLeafNode */

template<typename ContainerType, typename LeafStorageType>
class EGTreeLeafNode : public EGTreeNodeBase<ContainerType>
{
public:
    LeafStorageType storage;
    
    EGTreeLeafNode(EGTreeNode *parent) : EGTreeNodeBase<ContainerType>(parent), storage() {}
    virtual ~EGTreeLeafNode() {}
    
    EGTreeNodeKind getNodeKind() const { return EGTreeNodeLeaf; }
    virtual EGsize childCount() const { return 0; }
    virtual EGTreeNode* childAt(EGsize index) { return NULL; }
    virtual void setChild(EGsize index, EGTreeNode* child) {}
    virtual EGoffset findChild(EGTreeNode *child) { return -1; }
};


//
// Tree Node Visitor
//


/* EGTreeNodeVisitor */

struct EGTreeNodeVisitor
{
    virtual void preDescend(EGTreeNode *node) = 0;
    virtual EGbool shouldDescend(EGTreeNode *node) = 0;
    virtual void postDescend(EGTreeNode *node) = 0;
    virtual ~EGTreeNodeVisitor() {}
};


//
// Tree
//


/* EGTree */

template <typename DataType>
class EGTree
{
public:
    typedef std::shared_ptr<EGTree<DataType> > TreePtr;
    
    EGTree() {}
    virtual ~EGTree() {}
    
    virtual EGTreeNode* getRootNode() = 0;
    virtual void clear() = 0;
    virtual EGTreeNode* find(DataType& data) = 0;
    virtual EGbool insert(DataType& data) = 0;
    virtual EGbool erase(DataType& data) = 0;
    virtual void visit(EGTreeNode *node, EGTreeNodeVisitor& visitor) = 0;
    virtual void visit(EGTreeNodeVisitor& visitor) = 0;
    virtual void print() = 0;
};

#endif
