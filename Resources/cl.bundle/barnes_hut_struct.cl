/*
 *  barnes_hut_struct.cl
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


//
// Coulombs Law - Barnes Hut Optimized
//

// <numPoints>
__kernel void applyCoulombsLawBarnesHut(__global VecType *pointPosition,
                                        __global VecType *pointAcceleration,
                                        __global ScalarType *pointMass,
                                        const ScalarType repulsion,
                                        __global struct tree_node *nodes)
{
    const int p1 = get_global_id(0);
    
    VecType pointPositionP1 = pointPosition[p1];
    ScalarType pointMassP1 = pointMass[p1];
    VecType accelerationSum = 0;
    ScalarType distanceLimit = length(nodes[0].containerHalf) / 4.0;
    
    int nodeDepth = 0;
    uint nodeIdx[8] = { 0 };
    uint nodeChildIdx[8] = { 0 };
    
    do {
        // assumes LeafEntries == BranchEntries (currently true)
        __global struct tree_node *parentNode = nodes + nodeIdx[nodeDepth];
        int childNodeIdx = parentNode->entries[nodeChildIdx[nodeDepth]];
        int parentNodeState = parentNode->state;
        
        // visit node
        if (childNodeIdx >= 0) {
            
            if (parentNodeState == 2) {
                
                // visit branch node
                __global struct tree_node *node = nodes + childNodeIdx;
                VecType direction = pointPositionP1 - node->centerOfGravity;
                ScalarType distance = length(direction);
                
                // apply barnes hut optmization (force of node center of gravity)
                // if the node container center of gravity is over the distance limit
                //
                // NOTE: we also make sure the distance is greater than the node width to avoid applying the
                // center of gravity of the root node
                if (distance > distanceLimit && distance > length(node->containerHalf) * 2.0) {
                    
                    direction *= (ScalarType)1 / fmax(distance, (ScalarType)0.1f);
                    
                    // apply force to each end point
                    ScalarType f = repulsion / fmax(distance * distance * (ScalarType)0.5f, (ScalarType)0.005f);
                    VecType force = direction * f * node->mass;
                    accelerationSum += force * ((ScalarType)1 / pointMassP1);
                    
                } else {
                    // descend
                    nodeDepth++;
                    nodeIdx[nodeDepth] = childNodeIdx;
                    nodeChildIdx[nodeDepth] = 0;
                    continue;
                }

            } else if (parentNodeState == 3) {

                // get distance between the points
                VecType direction = pointPositionP1 - pointPosition[childNodeIdx];
                ScalarType distance = length(direction);
                direction *= (ScalarType)1 / fmax(distance, (ScalarType)0.1f);
                
                // apply force to each end point
                ScalarType f = repulsion / fmax(distance * distance * (ScalarType)0.5f, (ScalarType)0.005f);
                VecType force = direction * f;
                accelerationSum += force * ((ScalarType)1 / pointMassP1);

            }
        }
        
        // advance to next node
        nodeChildIdx[nodeDepth]++;
        
        // if we have reached the last index then pop up and across
        while (nodeDepth > 0 && nodeChildIdx[nodeDepth] >= LeafEntries) {
            nodeDepth--;
            nodeChildIdx[nodeDepth]++;
        }
        
    } while (nodeChildIdx[nodeDepth] < LeafEntries);
        
    pointAcceleration[p1] += accelerationSum;
}
