/*
 *  tree_struct.cl
 */

#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable

#ifndef VecType
#define VecType float4
#endif

#ifndef ScalarType
#define ScalarType float
#endif

#ifndef BranchEntries
#define BranchEntries 4
#endif

#ifndef LeafEntries
#define LeafEntries 4
#endif


// globals

struct tree_global
{
    volatile int nextNode;
    volatile int activeNodes;
    int depth;
    int pad;
};


// tree node 

struct tree_node
{
    VecType containerCenter;        /* node center */
    VecType containerHalf;          /* node half dimension */
    VecType centerOfGravity;        /* node center of gravity */
    int entries[LeafEntries];       /* holds either child nodes in branches or point indexes in leafs */
    uint childCount[BranchEntries]; /* holds number of children for each of the indicies */
    uint state;                     /* 0 = inactive, 1 = active, 2 = branch, 3 = leaf */
    int depth;                      /* node depth */
    int parent;                     /* node parent */
	ScalarType mass;                /* node mass */
};


// forward declarations

void insertPoint(__global struct tree_node *node, const uint pointIdx);
int childIndex(__global struct tree_node *node, VecType pos);
VecType childDirection(int idx);
float roundPow2(float val);


// private functions

void insertPoint(__global struct tree_node *node, const uint pointIdx)
{
    // atomically compare each entry to -1 and attempt to replace with pointIdx
    // if success old val -1 is returned and entry is updated and we break
    // otherwise we continue on to find the next empty slot
    
    for (int childIdx = 0; childIdx < LeafEntries; childIdx++) {
        if (atomic_cmpxchg(&node->entries[childIdx], -1, pointIdx) == -1) {
            break;
        }
    }
}

#if BranchEntries == 4
int childIndex(__global struct tree_node *node, VecType pos)
{
    VecType delta = pos - node->containerCenter;
    
    if (delta.x <= 0 && delta.y <= 0) return 0;
    else if (delta.x <= 0 && delta.y >= 0) return 1;
    else if (delta.x >= 0 && delta.y <= 0) return 2;
    else return 3;
}

VecType childDirection(int idx)
{
    switch (idx) {
        case 0: return (VecType)(-1.0f, -1.0f, 0.0f, 0.0f);
        case 1: return (VecType)(-1.0f, 1.0f, 0.0f, 0.0f);
        case 2: return (VecType)(1.0f, -1.0f, 0.0f, 0.0f);
        case 3: default: return (VecType)(1.0f, 1.0f, 0.0f, 0.0f);
    }
}
#endif

#if BranchEntries == 8
int childIndex(__global struct tree_node *node, VecType pos)
{
    VecType delta = pos - node->containerCenter;

    if (delta.x <= 0 && delta.y <= 0 && delta.z <= 0) return 0;
    else if (delta.x <= 0 && delta.y <= 0 && delta.z >= 0) return 1;
    else if (delta.x <= 0 && delta.y >= 0 && delta.z <= 0) return 2;
    else if (delta.x <= 0 && delta.y >= 0 && delta.z >= 0) return 3;
    else if (delta.x >= 0 && delta.y <= 0 && delta.z <= 0) return 4;
    else if (delta.x >= 0 && delta.y <= 0 && delta.z >= 0) return 5;
    else if (delta.x >= 0 && delta.y >= 0 && delta.z <= 0) return 6;
    else return 7;
}

VecType childDirection(int idx)
{
    switch (idx) {
        case 0: return (VecType)(-1.0f, -1.0f, -1.0f, 0.0f);
        case 1: return (VecType)(-1.0f, -1.0f, 1.0f, 0.0f);
        case 2: return (VecType)(-1.0f, 1.0f, -1.0f, 0.0f);
        case 3: return (VecType)(-1.0f, 1.0f, 1.0f, 0.0f);
        case 4: return (VecType)(1.0f, -1.0f, -1.0f, 0.0f);
        case 5: return (VecType)(1.0f, -1.0f, 1.0f, 0.0f);
        case 6: return (VecType)(1.0f, 1.0f, -1.0f, 0.0f);
        case 7: default: return (VecType)(1.0f, 1.0f, 1.0f, 0.0f);
    }
}
#endif

float roundPow2(float val)
{
    /* get mantissa and exponent */
    int exp;
    float m = frexp(val, &exp);
    
    /* get integer reciprocal of mantissa */
    int r = (int)(1.0f / m);
    
    /* round integer reciprocal to nearest power of 2 */
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


// tree kernel functions

/*
 * VARIABLES
 *
 *   __global VecType *pointPosition,          // sizeof(VecType) * numpoints
 *   __global struct tree_global *globals,     // sizeof(struct tree_global)
 *   __global struct tree_node *nodes,         // sizeof(struct tree_node) * maxnodes
 *   __global uint *pointParent,               // sizeof(uint) * numpoints
 *   __global uint *pointIndex,                // sizeof(uint) * numpoints
 *   const uint maxNodes,                      // = numpoints * 2
 *   const uint numPoints
 *
 * PSEUDO CODE
 *
 *   // requires <depth> iterations e.g. ~(6 to 10) iterations for 1024 points
 *   // parallelism increases with each subsequent iteration
 *
 *   treeInitPoints<numpoints>();
 *   treeInitNodes<maxnodes>();
 *   treeGetRootBounds();
 *   
 *   globals->nextnode = 1;
 *   globals->activeNodes = 1;
 *   do {
 *       treeUpdateChildCounts<numpoints>();
 *       treeCreateNodes<nextnode>();
 *       treeInsertPoints<numpoints>();
 *       readBuffer(activeNodes)
 *       readBuffer(nextNode)
 *   } while (globals->activeNodes > 0)
 *
 */

// task
__kernel void treeNodeSize(__global size_t *sizeoftreeNode)
{
    // used to double check structure alignment
    *sizeoftreeNode = sizeof(struct tree_node);
}

// <numpoints>
__kernel void treeInitPoints(__global uint *pointParent)
{
    const uint pointIdx = get_global_id(0);

    // set every points parent to 0 (root node)
    pointParent[pointIdx] = 0;
}

// <maxnodes>
__kernel void treeInitNodes(__global struct tree_node *nodes)
{
    const uint nodeIdx = get_global_id(0);
    __global struct tree_node *node = nodes + nodeIdx;

    // set every nodes state to 0 (inactive) and all child entries to -1
    node->state = 0;
    node->parent = -1;
    node->depth = -1;
    for (int childIdx = 0; childIdx < LeafEntries; childIdx++) {
        node->entries[childIdx] = -1;
	}
    for (int childIdx = 0; childIdx < BranchEntries; childIdx++) {
        node->childCount[childIdx] = 0;
    }
}

// <numworkitems>
__kernel void treeGetRootBoundsGroup(__global struct tree_global *globals,
                                     __global VecType *pointPosition,
                                     __global VecType *minBoundGroup,
                                     __global VecType *maxBoundGroup,
                                     const uint workGroupSize,
                                     const uint numPoints)
{
    // compute root node dimensions
    const uint wi = get_global_id(0);
    const uint pstart = wi * workGroupSize;
    const uint pend = min((wi + 1) * workGroupSize, numPoints);
    
    VecType minBound = (VecType)FLT_MAX;
    VecType maxBound = (VecType)-FLT_MAX;
    for (uint pointIdx = pstart; pointIdx < pend; pointIdx++) {
        VecType position = pointPosition[pointIdx];
        minBound = min(position, minBound);
        maxBound = max(position, maxBound);
    }
    minBoundGroup[wi] = minBound;
    maxBoundGroup[wi] = maxBound;
}

// task
__kernel void treeGetRootBoundsSum(__global struct tree_global *globals,
                                   __global struct tree_node *nodes,
                                   __global VecType *minBoundGroup,
                                   __global VecType *maxBoundGroup,
                                   const uint numWorkItems)
{
    VecType minBound = (VecType)FLT_MAX;
    VecType maxBound = (VecType)-FLT_MAX;
    for (uint wi = 0; wi < numWorkItems; wi++) {
        minBound = min(minBoundGroup[wi], minBound);
        maxBound = max(maxBoundGroup[wi], maxBound);
    }

    float max = fmax(fmax(fabs(minBound.x),fabs(maxBound.x)),fmax(fabs(minBound.y),fabs(maxBound.y)));
    
    nodes[0].containerCenter = 0.0f;
    nodes[0].containerHalf = roundPow2(max);
    nodes[0].state = 1; // mark root as active
    nodes[0].depth = 0;
    
    globals->nextNode = 1;
    globals->activeNodes = 1;
}

// <numpoints>
__kernel void treeUpdateChildCounts(__global VecType *pointPosition,
                                    __global struct tree_node *nodes,
                                    __global uint *pointParent,
                                    __global uint *pointIndex)
{
    const uint pointIdx = get_global_id(0);
    __global struct tree_node *parentNode = nodes + pointParent[pointIdx];
    
    // for points whose parent is active, find quadrant relative to parent container
    if (parentNode->state == 1) {
        VecType pos = pointPosition[pointIdx];
        const uint childIdx = childIndex(parentNode, pos);
        pointIndex[pointIdx] = childIdx;
        atomic_inc(&parentNode->childCount[childIdx]);
    }
}

// <nextnode>
__kernel void treeCreateNodes(__global struct tree_global *globals,
                              __global struct tree_node *nodes,
                              __global uint *pointParent,
                              __global uint *pointIndex,
                              const int depth)
{
    const uint nodeIdx = get_global_id(0);
    __global struct tree_node *node = nodes + nodeIdx;
    
    // for active nodes (nodeIdx < maxNodes && nodes[nodeIdx].state == 1)
    //     for each child index
    //         if childCount[childIndex] == 0
    //             set entry to -1 (unoccupied)
    //         if childCount[childIndex] <= LeafEntries
    //             create leaf node and mark state as leaf (3)
    //             insert points into leaf
    //                 (points insert themselves into their parent in the kernel: treeInsertPoints)
    //             set new parent on points and set entries on parent node
    //         if childCount[childIndex] > LeafEntries
    //             create branch node and mark state as active (1)
    //             create child up to BranchEntries child node containers for indices with counts > 0, and mark them active
    //             set new parent on points and set entries on parent node
    //     mark node as branch

    // create nodes
    if (node->state == 1) {
        // create nodes for quadrants with children
		int activeNodesDelta = -1;
        for (int childIdx = 0; childIdx < BranchEntries; childIdx++) {
            uint count = node->childCount[childIdx];
            if (count == 0) continue;
            // allocate a new node index and set container on new node
            uint newNodeIdx = atomic_inc(&globals->nextNode);
            node->entries[childIdx] = newNodeIdx;
            __global struct tree_node *newNode = nodes + newNodeIdx;
            newNode->containerHalf = node->containerHalf / 2.0f;
            newNode->containerCenter = node->containerCenter + newNode->containerHalf * childDirection(childIdx);
            newNode->parent = nodeIdx;
            newNode->depth = depth;
            globals->depth = depth;
            if (count <= LeafEntries) {
                // set new node type to leaf
                newNode->state = 3;
            } else {
                // set new node type to active and atomically increment activeNodes
                newNode->state = 1;
                activeNodesDelta++;
            }
        }
        // set type to branch and atomically decrement activeNodes
        node->state = 2;
        atomic_add(&globals->activeNodes, activeNodesDelta);
    }
}

// <numpoints>
__kernel void treeInsertPoints(__global struct tree_node *nodes,
                               __global uint *pointParent,
                               __global uint *pointIndex)
{
    const uint pointIdx = get_global_id(0);
    
    // insert points and update parents for all branches
    if (nodes[pointParent[pointIdx]].state == 2) {
        uint parentNodeIdx = pointParent[pointIdx];
        uint childIdx = pointIndex[pointIdx];
        __global struct tree_node *parentNode = nodes + parentNodeIdx;
        uint count = parentNode->childCount[childIdx];
        uint childNodeIdx = parentNode->entries[childIdx];
        
        // child now either points to leaf (finished) or a new lower branch
        pointParent[pointIdx] = childNodeIdx;
        
        // if our count is less than LeafEntries then we are inserting to a leaf (finished)
        if (count <= LeafEntries) {
            insertPoint(nodes + childNodeIdx, pointIdx);
        }
    }
}

//
// Calculate center of gravity
//

// <numnodes>
__kernel void treeSumNodes(__global VecType *pointPosition,
                           __global ScalarType *pointMass,
                           __global struct tree_node *nodes,
                           const int depth)
{
    const uint nodeIdx = get_global_id(0);
    __global struct tree_node *node = nodes + nodeIdx;
    
    if (node->depth == depth) {
        if (node->state == 2) {
            VecType centerOfGravitySum = (ScalarType)0;
            ScalarType mass = (ScalarType)0;
            for (uint childIdx = 0; childIdx < BranchEntries; childIdx++) {
                int childNodeIdx = node->entries[childIdx];
                if (childNodeIdx < 0) continue;
                centerOfGravitySum += nodes[childNodeIdx].centerOfGravity;
                mass += nodes[childNodeIdx].mass;
            }
            node->centerOfGravity = centerOfGravitySum / mass;
            node->mass = mass;
        } else if (node->state == 3) {
            VecType centerOfGravitySum = (ScalarType)0;
            ScalarType mass = (ScalarType)0;
            for (uint childIdx = 0; childIdx < LeafEntries; childIdx++) {
                int pointIdx = node->entries[childIdx];
                if (pointIdx < 0) break;
                centerOfGravitySum += pointPosition[pointIdx] * pointMass[pointIdx];
                mass += pointMass[pointIdx];
            }
            node->centerOfGravity = centerOfGravitySum / mass;
            node->mass = mass;
        }
    }
}
