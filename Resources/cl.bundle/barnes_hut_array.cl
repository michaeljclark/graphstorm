/*
 *  barnes_hut_array.cl
 */

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


//
// Coulombs Law - Barnes Hut Optimized
//

// <numPoints>
__kernel void applyCoulombsLawBarnesHut(__global VecType *pointPosition,
                                        __global VecType *pointAcceleration,
                                        __global ScalarType *pointMass,
                                        const ScalarType repulsion,
                                        __global uint* nodeState,
                                        __global int* nodeEntries,
                                        __global VecType *nodeHalf,
                                        __global ScalarType* nodeMass,
                                        __global VecType* nodeCenterOfGravity)
{
    const int p1 = get_global_id(0);
    
    VecType pointPositionP1 = pointPosition[p1];
    ScalarType pointMassP1 = pointMass[p1];
    VecType accelerationSum = 0;

    // not implemented
    
    pointAcceleration[p1] += accelerationSum;
}
