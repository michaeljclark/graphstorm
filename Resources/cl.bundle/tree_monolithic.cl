/*
 *  tree_monolithic.cl
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


// forward declarations

void insertPoint(__global int *entries, const uint pointIdx);
int childIndex(VecType delta);
VecType childDirection(int idx);
float roundPow2(float val);


// private functions

void insertPoint(__global int *entries, const uint pointIdx)
{
    // atomically compare each entry to -1 and attempt to replace with pointIdx
    // if success old val -1 is returned and entry is updated and we break
    // otherwise we continue on to find the next empty slot
    
    for (int childIdx = 0; childIdx < LeafEntries; childIdx++) {
        if (atomic_cmpxchg(&entries[childIdx], -1, pointIdx) == -1) {
            break;
        }
    }
}

#if BranchEntries == 4
int childIndex(VecType delta)
{
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
int childIndex(VecType delta)
{
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
 *   __global VecType *pointPosition,          // sizeof(VecType) * numpoints             - point positions
 *   __global struct tree_global *globals,     // sizeof(struct tree_global)              - tree globals
 *   __global VecType* nodeCenter,             // sizeof(VecType) * maxnodes              - node center
 *   __global VecType* nodeHalf,               // sizeof(VecType) * maxnodes              - node half dimension
 *   __global VecType* nodeCenterOfGravity,    // sizeof(VecType) * maxnodes              - node center of gravity
 *   __global ScalarType* nodeMass,            // sizeof(ScalarType) * maxnodes           - node mass
 *   __global int* nodeEntries,                // sizeof(int) * maxnodes * LeafEntries    - node child indicies in branches or point indicies in leafs
 *   __global uint* nodeChildCount,            // sizeof(uint) * maxnodes * BranchEntries - node child counts for each of the indicies
 *   __global uint* nodeState,                 // sizeof(uint) * maxnodes                 - node state, 0 = inactive, 1 = active, 2 = branch, 3 = leaf
 *   __global uint *pointParent,               // sizeof(uint) * numpoints                - point parent node
 *   __global uint *pointIndex,                // sizeof(uint) * numpoints                - point node indice
 *   const uint maxNodes,                      // = numpoints * 2                         - maximum number of nodes
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

// <workgroupsize> <workgroupsize>
__kernel void treeBuild(__global struct tree_global *globals,
                        __global VecType *pointPosition,
                        __global ScalarType *pointMass,
                        __local VecType *minBoundGroup,
                        __local VecType *maxBoundGroup,
                        __global VecType *nodeCenter,
                        __global VecType *nodeHalf,
                        __global ScalarType* nodeMass,
                        __global VecType* nodeCenterOfGravity,
                        __global uint* nodeState,
                        __global int* nodeParent,
                        __global int* nodeDepth,
                        __global uint* nodeChildCount,
                        __global int* nodeEntries,
                        __global uint *pointParent,
                        __global uint *pointIndex,
                        const uint numPoints,
                        const uint maxNodes)
{
    uint i;
    
    
    // init points
    
    i = get_global_id(0);
    while (i < numPoints) {
        pointParent[i] = 0;
        i += get_global_size(0);
    }

    
    // init nodes
    
    i = get_global_id(0);
    while (i < maxNodes) {
        // set every nodes state to 0 (inactive) and all child entries to -1
        nodeState[i] = 0;
        nodeParent[i] = -1;
        for (int childIdx = 0; childIdx < LeafEntries; childIdx++) {
            nodeEntries[i * LeafEntries + childIdx] = -1;
        }
        for (int childIdx = 0; childIdx < BranchEntries; childIdx++) {
            nodeChildCount[i * BranchEntries + childIdx] = 0;
        }
        i += get_global_size(0);
    }
    
    
    // get root bounds
    
    VecType minBound = (VecType)FLT_MAX;
    VecType maxBound = (VecType)-FLT_MAX;
    i = get_global_id(0);
    while (i < numPoints) {
        VecType position = pointPosition[i];
        minBound = min(position, minBound);
        maxBound = max(position, maxBound);
        i += get_global_size(0);
    }
    minBoundGroup[get_global_id(0)] = minBound;
    maxBoundGroup[get_global_id(0)] = maxBound;

    barrier(CLK_LOCAL_MEM_FENCE);
    
    
    // root bounds sum
    
    if (get_global_id(0) == 0)
    {
        minBound = (VecType)FLT_MAX;
        maxBound = (VecType)-FLT_MAX;
        for (i = 0; i < get_global_size(0); i++) {
            minBound = min(minBoundGroup[i], minBound);
            maxBound = max(maxBoundGroup[i], maxBound);
        }
        float max = fmax(fmax(fabs(minBound.x),fabs(maxBound.x)),fmax(fabs(minBound.y),fabs(maxBound.y)));
        nodeCenter[0] = 0.0f;
        nodeHalf[0] = roundPow2(max);
        nodeState[0] = 1; // mark root as active
        nodeDepth[0] = 0;
        globals->nextNode = 1;
        globals->activeNodes = 1;
    }
    
    barrier(CLK_GLOBAL_MEM_FENCE);

    
    // main loop
    
    int depth = 0;
    
    do {

        depth++;

        // update child counts
        
        i = get_global_id(0);
        while (i < numPoints) {
            const uint parentIdx = pointParent[i];
            // for points whose parent is active, find quadrant relative to parent container
            if (nodeState[parentIdx] == 1) {
                VecType delta = pointPosition[i] - nodeCenter[parentIdx];
                const uint childIdx = childIndex(delta);
                pointIndex[i] = childIdx;
                atomic_inc(&nodeChildCount[parentIdx * BranchEntries + childIdx]);
            }
            i += get_global_size(0);
        }
        
        barrier(CLK_GLOBAL_MEM_FENCE | CLK_LOCAL_MEM_FENCE);
        
        // create nodes
        
        uint nextNode = globals->nextNode;
        
        i = get_global_id(0);
        while (i < nextNode) {
            if (nodeState[i] == 1) {
                // create nodes for quadrants with children
				int activeNodesDelta = -1;
                for (int childIdx = 0; childIdx < BranchEntries; childIdx++) {
                    uint count = nodeChildCount[i * BranchEntries + childIdx];
                    if (count == 0) continue;
                    // allocate a new node index and set container on new node
                    uint newNodeIdx = atomic_inc(&globals->nextNode);
                    nodeEntries[i * LeafEntries + childIdx] = newNodeIdx;
                    nodeHalf[newNodeIdx] = nodeHalf[i] / 2.0f;
                    nodeCenter[newNodeIdx] = nodeCenter[i] + nodeHalf[newNodeIdx] * childDirection(childIdx);
                    nodeParent[newNodeIdx] = i;
                    nodeDepth[newNodeIdx] = depth;
                    globals->depth = depth;
                    if (count <= LeafEntries) {
                        // set new node type to leaf
                        nodeState[newNodeIdx] = 3;
                    } else {
                        // set new node type to active and atomically increment activeNodes
                        nodeState[newNodeIdx] = 1;
                        activeNodesDelta++;
                    }
                }
                // set type to branch and atomically decrement activeNodes
                nodeState[i] = 2;
                atomic_add(&globals->activeNodes, activeNodesDelta);
            }
            i += get_global_size(0);
        }
        
        barrier(CLK_GLOBAL_MEM_FENCE);

        
        // insert points
        
        i = get_global_id(0);
        while (i < numPoints) {
            // insert points and update parents for all branches
            if (nodeState[pointParent[i]] == 2) {
                uint parentNodeIdx = pointParent[i];
                uint childIdx = pointIndex[i];
                uint count = nodeChildCount[parentNodeIdx * BranchEntries + childIdx];
                uint childNodeIdx = nodeEntries[parentNodeIdx * LeafEntries + childIdx];
                
                // child now either points to leaf (finished) or a new lower branch
                pointParent[i] = childNodeIdx;
                
                // if our count is less than Q then we are inserting to a leaf (finished)
                if (count <= LeafEntries) {
                    insertPoint(nodeEntries + childNodeIdx * LeafEntries, i);
                }
            }
            i += get_global_size(0);
        }
        
        barrier(CLK_GLOBAL_MEM_FENCE);
        
    } while (depth <= globals->depth);
    
    
    // center of gravity
    
    for (int rdepth = depth; rdepth >= 0; rdepth--) {
        
        i = get_global_id(0);
        while (i < maxNodes) {
            // calculate center of gravity for this node
            if (nodeDepth[i] == rdepth) {
                if (nodeState[i] == 2) {
                    VecType centerOfGravitySum = (ScalarType)0;
                    ScalarType mass = (ScalarType)0;
                    for (uint childIdx = 0; childIdx < BranchEntries; childIdx++) {
                        int childNodeIdx = nodeEntries[i * LeafEntries + childIdx];
                        if (childNodeIdx < 0) continue;
                        centerOfGravitySum += nodeCenterOfGravity[childNodeIdx];
                        mass += nodeMass[childNodeIdx];
                    }
                    nodeCenterOfGravity[i] = centerOfGravitySum / mass;
                    nodeMass[i] = mass;
                } else if (nodeState[i] == 3) {
                    VecType centerOfGravitySum = (ScalarType)0;
                    ScalarType mass = (ScalarType)0;
                    for (uint childIdx = 0; childIdx < LeafEntries; childIdx++) {
                        int pointIdx = nodeEntries[i * LeafEntries + childIdx];
                        if (pointIdx < 0) break;
                        centerOfGravitySum += pointPosition[pointIdx] * pointMass[pointIdx];
                        mass += pointMass[pointIdx];
                    }
                    nodeCenterOfGravity[i] = centerOfGravitySum / mass;
                    nodeMass[i] = mass;
                }
            }
            i += get_global_size(0);
        }

        barrier(CLK_GLOBAL_MEM_FENCE);
        
    }
}
