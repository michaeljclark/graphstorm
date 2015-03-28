/*
 *  springlayout.cl
 */

#ifndef VecType
#define VecType float4
#endif

#ifndef ScalarType
#define ScalarType float
#endif

union BufferType
{
    float f;
    uint u;
};

// <numEntries>
__kernel void clearPointAccel(__global VecType *pointAcceleration)
{
    pointAcceleration[get_global_id(0)] = (ScalarType)0;
}

// <numPoints>
__kernel void applyCoulombsLaw(__global VecType *pointPosition,
                               __global VecType *pointAcceleration,
                               __global ScalarType *pointMass,
                               const int numPoints,
                               const ScalarType repulsion)
{
    const int p1 = get_global_id(0);
    if (p1 >= numPoints) return;
    
    VecType pointPositionP1 = pointPosition[p1];
    ScalarType pointMassP1 = pointMass[p1];
    VecType accelerationSum = 0;
    for (int p2 = 0; p2 < numPoints; p2++) {
        if (p1 == p2) continue;
        
        // get distance between the points
        VecType direction = pointPositionP1 - pointPosition[p2];
        ScalarType distance = length(direction);
        direction *= (ScalarType)1 / fmax(distance, (ScalarType)0.1f);
        
        // apply force to each end point
        ScalarType f = repulsion / fmax(distance * distance * (ScalarType)0.5f, (ScalarType)0.005f);
        VecType force = direction * f;
        accelerationSum += force * ((ScalarType)1 / pointMassP1);
    }
    pointAcceleration[p1] += accelerationSum;
}

// <numPoints,numWorkItems> <workGroupSize,1>
// This version is optimized to store a block of points in local memory
// It requires a second pass to sum numWorkItems accelerations per point
__kernel void applyCoulombsLawGroup(__global VecType *pointPosition,
                                    __global VecType *pointAcceleration,
                                    __global VecType *pointGroupAcceleration,
                                    __global ScalarType *pointMass,
                                    __local VecType *pointPositionLocal, /* sizeof(VecType) * workGroupSize */
                                    const int numPoints,
                                    const ScalarType repulsion)
{
    const int p1 = get_global_id(0);
    const int wg = get_global_id(1);
    const int numWorkItems = get_global_size(1);
    const int workGroupSize = get_local_size(0);
    const int lp2 = get_local_id(0);
    
    // read positions into local memory
    pointPositionLocal[lp2] = pointPosition[wg * workGroupSize + lp2];
    
    barrier(CLK_LOCAL_MEM_FENCE);
    
    if (p1 >= numPoints) return;
    
    // read our position and mass
    VecType pointPositionP1 = pointPosition[p1];
    ScalarType pointMassP1 = pointMass[p1];
    
    VecType accelerationSum = 0;
    const int p2offset = wg * workGroupSize;
    for (int wp2 = 0; wp2 < workGroupSize; wp2++) {
        const int p2 = p2offset + wp2;
        if (p1 == p2) continue;
        if (p2 >= numPoints) break;
        
        // get distance between the points
        VecType direction = pointPositionP1 - pointPositionLocal[wp2];
        ScalarType distance = length(direction);
        direction *= (ScalarType)1 / fmax(distance, (ScalarType)0.1f);
        
        // apply force to each end point
        ScalarType f = repulsion / fmax(distance * distance * (ScalarType)0.5f, (ScalarType)0.005f);
        VecType force = direction * f;
        accelerationSum += force * ((ScalarType)1 / pointMassP1);
    }
    
    // output sum to point acceleration slot in global memory
    pointGroupAcceleration[p1 * numWorkItems + wg] += accelerationSum;
}

// <numPoints>
__kernel void sumPointGroupAccel(__global VecType *pointAcceleration,
                                 __global VecType *pointGroupAcceleration,
                                 const int numPoints,
                                 const int numWorkItems)
{
    const int p = get_global_id(0);
    if (p >= numPoints) return;
    
    VecType accelerationSum = (ScalarType)0;
    for (int accelslot = 0; accelslot < numWorkItems; accelslot++) {
        accelerationSum += pointGroupAcceleration[p * numWorkItems + accelslot];
    }
    pointAcceleration[p] += accelerationSum;
}

// <numSprings>
__kernel void applyHookesLaw(__global VecType *pointPosition,
                             __global VecType *pointAcceleration,
                             __global VecType *pointSpringAcceleration,
                             __global ScalarType *pointMass,
                             __global ScalarType *springLength,
                             __global ScalarType *springStiffness,
                             __global uint *springPointIndex1,
                             __global uint *springPointIndex2,
                             __global uint *springAccelSlotPoint1,
                             __global uint *springAccelSlotPoint2,
                             const int numSprings)
{
    const int s = get_global_id(0);
    if (s >= numSprings) return;
    const int p1 = springPointIndex1[s];
    const int p2 = springPointIndex2[s];
    
    // get the direction of the spring
    VecType direction = pointPosition[p2] - pointPosition[p1];
    ScalarType distance = length(direction);
    ScalarType displacement = springLength[s] - distance;
    direction *= (ScalarType)1 / fmax(distance, (ScalarType)0.1f);
    
    // apply forces
    ScalarType f = springStiffness[s] * displacement * (ScalarType)0.5f;
    VecType force1 = direction * -f;
    VecType force2 = direction * f;
    
    // output sum to point acceleration slot in global memory
    pointSpringAcceleration[springAccelSlotPoint1[s]] = force1 * ((ScalarType)1 / pointMass[p1]);
    pointSpringAcceleration[springAccelSlotPoint2[s]] = force2 * ((ScalarType)1 / pointMass[p2]);
}

// <numPoints>
__kernel void sumPointSpringAccel(__global VecType *pointAcceleration,
                                  __global VecType *pointSpringAcceleration,
                                  __global uint* pointSpringAccelSlotOffset,
                                  __global uint* pointSpringAccelSlotCount,
                                  const int numPoints)
{
    const int p = get_global_id(0);
    if (p >= numPoints) return;
    
    const uint start = pointSpringAccelSlotOffset[p];
    const uint end = start + pointSpringAccelSlotCount[p];
    
    VecType accelerationSum = (ScalarType)0;
    for (uint accelslot = start; accelslot < end; accelslot++) {
        accelerationSum += pointSpringAcceleration[accelslot];
    }
    pointAcceleration[p] += accelerationSum;
}

// <numPoints>
__kernel void attractToCenter(__global VecType *pointPosition,
                              __global VecType *pointAcceleration,
                              __global ScalarType *pointMass,
                              const int numPoints,
                              const ScalarType centerAttraction)
{
    int p = get_global_id(0);
    if (p >= numPoints) return;
    
    VecType direction = pointPosition[p] * (ScalarType)-1;
    VecType force = direction * centerAttraction;
    pointAcceleration[p] += force * ((ScalarType)1 / pointMass[p]);
}

// <numPoints>
__kernel void updatePosition(__global VecType *pointPosition,
                             __global VecType *pointAcceleration,
                             __global VecType *pointVelocity,
                             const int numPoints,
                             const ScalarType t,
                             const ScalarType damping,
                             const ScalarType maxSpeed)
{
    int p = get_global_id(0);
    if (p >= numPoints) return;
    
    VecType velocity = (pointVelocity[p] + pointAcceleration[p] * t) * damping;
    ScalarType speed = length(velocity);
    if (speed > maxSpeed) {
        velocity *= maxSpeed / speed;
    }
    pointVelocity[p] = velocity;
    pointAcceleration[p] = (ScalarType)0;
    pointPosition[p] += pointVelocity[p] * t;
}

// <numWorkItems>
__kernel void totalEnergyGroup(__global VecType *pointVelocity,
                               __global ScalarType *pointMass,
                               __global ScalarType *pointEnergy,
                               const int workGroupSize,
                               const int numPoints)
{
    int p = get_global_id(0);
    ScalarType energy = (ScalarType)0;
    while (p < numPoints) {
        // calculate this points energy
        ScalarType speed = length(pointVelocity[p]);
        energy += (ScalarType)0.5f * pointMass[p] * speed * speed;
        p += get_global_size(0);
    }
    pointEnergy[get_global_id(0)] = energy;
}

// <task>
__kernel void totalEnergySum(__global ScalarType *pointEnergy,
                             __global ScalarType *energyReturn,
                             const int numWorkItems)
{
    ScalarType energy = (ScalarType)0;
    for (int wi = 0; wi < numWorkItems; wi++) {
        energy += pointEnergy[wi];
    }
    *energyReturn = energy;
}

// <numworkitems>
__kernel void getBoundsGroup(__global VecType *pointPosition,
                             __global VecType *minBoundGroup,
                             __global VecType *maxBoundGroup,
                             const int workGroupSize,
                             const int numPoints)
{
    int p = get_global_id(0);
    VecType minBound = (VecType)FLT_MAX;
    VecType maxBound = (VecType)-FLT_MAX;
    while (p < numPoints) {
        VecType position = pointPosition[p];
        minBound = min(position, minBound);
        maxBound = max(position, maxBound);
        p += get_global_size(0);
    }
    minBoundGroup[get_global_id(0)] = minBound;
    maxBoundGroup[get_global_id(0)] = maxBound;
}

// task
__kernel void getBoundsGather(__global VecType *minBoundGroup,
                              __global VecType *maxBoundGroup,
                              __global VecType *bounds,
                              const int numWorkItems)
{
    VecType minBound = (VecType)FLT_MAX;
    VecType maxBound = (VecType)-FLT_MAX;
    for (int wi = 0; wi < numWorkItems; wi++) {
        minBound = min(minBoundGroup[wi], minBound);
        maxBound = max(maxBoundGroup[wi], maxBound);
    }

#if 0
    printf("minBound=(%f,%f,%f,%f) maxBound=(%f,%f,%f,%f)\n",
      minBound.x, minBound.y, minBound.z, minBound.w,
      maxBound.x, maxBound.y, maxBound.z, maxBound.w);
#endif

    bounds[0] = minBound;
    bounds[1] = maxBound;
}

// <numPoints>
__kernel void createVertexBufferPoints(__global VecType *pointPosition,
                                       __global ScalarType *pointSize,
                                       __global uint *pointColor,
                                       __global union BufferType *vertexBuffer,
                                       __global uint *indexBuffer,
                                       VecType minBound,
                                       VecType maxBound,
                                       VecType viewBound,
                                       VecType viewOffset,
                                       ScalarType h,
                                       __global uint *d,
                                       int nDim)
{
    const int p = get_global_id(0);
    const VecType pos = pointPosition[p];
    const uint rgba = pointColor[p];
    const VecType bound = maxBound - minBound;
    const VecType vec = (pos - minBound) / bound * viewBound + viewOffset;
    const float3 proj = (float3)(vec.x * (d[0] == 0) + vec.y * (d[0] == 1) + vec.z * (d[0] == 2) + vec.w * (d[0] == 3),
                                 vec.x * (d[1] == 0) + vec.y * (d[1] == 1) + vec.z * (d[1] == 2) + vec.w * (d[1] == 3),
                                 vec.x * (d[2] == 0) + vec.y * (d[2] == 1) + vec.z * (d[2] == 2) + vec.w * (d[2] == 3));
    h *= pointSize[p];
    
    if (nDim == 2) {
        __global union BufferType *v = &vertexBuffer[4 * p];
        (*v++).f = proj.x; (*v++).f = proj.y; (*v++).f = h; (*v++).u = rgba;
        indexBuffer[p] = p;
    } else if (nDim == 3) {
        __global union BufferType *v = &vertexBuffer[5 * p];
        (*v++).f = proj.x; (*v++).f = proj.y; (*v++).f = proj.z; (*v++).f = h; (*v++).u = rgba;
        indexBuffer[p] = p;
    }
}

// <numPoints>
__kernel void createVertexBufferCubes(__global VecType *pointPosition,
                                      __global ScalarType *pointSize,
                                      __global uint *pointColor,
                                      __global union BufferType *vertexBuffer,
                                      __global uint *indexBuffer,
                                      VecType minBound,
                                      VecType maxBound,
                                      VecType viewBound,
                                      VecType viewOffset,
                                      ScalarType h,
                                      __global uint *d,
                                      int nDim)
{
    const int p = get_global_id(0);
    const VecType pos = pointPosition[p];
    const uint rgba = pointColor[p];
    const VecType bound = maxBound - minBound;
    const VecType vec = (pos - minBound) / bound * viewBound + viewOffset;
    const float3 proj = (float3)(vec.x * (d[0] == 0) + vec.y * (d[0] == 1) + vec.z * (d[0] == 2) + vec.w * (d[0] == 3),
                                 vec.x * (d[1] == 0) + vec.y * (d[1] == 1) + vec.z * (d[1] == 2) + vec.w * (d[1] == 3),
                                 vec.x * (d[2] == 0) + vec.y * (d[2] == 1) + vec.z * (d[2] == 2) + vec.w * (d[2] == 3));
    h *= pointSize[p];
    
    if (nDim == 2) {
        ScalarType x1 = proj.x - h, y1 = proj.y - h;
        ScalarType x2 = proj.x + h, y2 = proj.y + h;
        __global union BufferType *v = &vertexBuffer[12 * p];
        (*v++).f = x1; (*v++).f = y1; (*v++).u = rgba;
        (*v++).f = x1; (*v++).f = y2; (*v++).u = rgba;
        (*v++).f = x2; (*v++).f = y2; (*v++).u = rgba;
        (*v++).f = x2; (*v++).f = y1; (*v++).u = rgba;
        __global uint *i = &indexBuffer[6 * p];
        uint vert = 4 * p;
        *(i++) = vert + 0;
        *(i++) = vert + 1;
        *(i++) = vert + 3;
        *(i++) = vert + 1;
        *(i++) = vert + 2;
        *(i++) = vert + 3;
    } else if (nDim == 3) {
        ScalarType x1 = proj.x - h, y1 = proj.y - h, z1 = proj.z - h;
        ScalarType x2 = proj.x + h, y2 = proj.y + h, z2 = proj.z + h;
        __global union BufferType *v = &vertexBuffer[168 * p];
        // front
        (*v++).f = x1; (*v++).f = y1; (*v++).f = z1; (*v++).f = 0.0f; (*v++).f = 0.0f; (*v++).f = -1.0f; (*v++).u = rgba; // 0
        (*v++).f = x2; (*v++).f = y1; (*v++).f = z1; (*v++).f = 0.0f; (*v++).f = 0.0f; (*v++).f = -1.0f; (*v++).u = rgba; // 1
        (*v++).f = x2; (*v++).f = y2; (*v++).f = z1; (*v++).f = 0.0f; (*v++).f = 0.0f; (*v++).f = -1.0f; (*v++).u = rgba; // 2
        (*v++).f = x1; (*v++).f = y2; (*v++).f = z1; (*v++).f = 0.0f; (*v++).f = 0.0f; (*v++).f = -1.0f; (*v++).u = rgba; // 3
        // back
        (*v++).f = x1; (*v++).f = y1; (*v++).f = z2; (*v++).f = 0.0f; (*v++).f = 0.0f; (*v++).f = 1.0f; (*v++).u = rgba; // 4
        (*v++).f = x2; (*v++).f = y1; (*v++).f = z2; (*v++).f = 0.0f; (*v++).f = 0.0f; (*v++).f = 1.0f, (*v++).u = rgba; // 5
        (*v++).f = x2; (*v++).f = y2; (*v++).f = z2; (*v++).f = 0.0f; (*v++).f = 0.0f; (*v++).f = 1.0f; (*v++).u = rgba; // 6
        (*v++).f = x1; (*v++).f = y2; (*v++).f = z2; (*v++).f = 0.0f; (*v++).f = 0.0f; (*v++).f = 1.0f; (*v++).u = rgba; // 7
        // top
        (*v++).f = x1; (*v++).f = y2; (*v++).f = z1; (*v++).f = 0.0f; (*v++).f = 1.0f; (*v++).f = 0.0f; (*v++).u = rgba; // 8
        (*v++).f = x2; (*v++).f = y2; (*v++).f = z1; (*v++).f = 0.0f; (*v++).f = 1.0f; (*v++).f = 0.0f; (*v++).u = rgba; // 9
        (*v++).f = x2; (*v++).f = y2; (*v++).f = z2; (*v++).f = 0.0f; (*v++).f = 1.0f; (*v++).f = 0.0f; (*v++).u = rgba; // 10
        (*v++).f = x1; (*v++).f = y2; (*v++).f = z2; (*v++).f = 0.0f; (*v++).f = 1.0f; (*v++).f = 0.0f; (*v++).u = rgba; // 11
        // bottom
        (*v++).f = x1; (*v++).f = y1; (*v++).f = z1; (*v++).f = 0.0f; (*v++).f = -1.0f; (*v++).f = 0.0f; (*v++).u = rgba; // 12
        (*v++).f = x2; (*v++).f = y1; (*v++).f = z1; (*v++).f = 0.0f; (*v++).f = -1.0f; (*v++).f = 0.0f; (*v++).u = rgba; // 13
        (*v++).f = x2; (*v++).f = y1; (*v++).f = z2; (*v++).f = 0.0f; (*v++).f = -1.0f; (*v++).f = 0.0f; (*v++).u = rgba; // 14
        (*v++).f = x1; (*v++).f = y1; (*v++).f = z2; (*v++).f = 0.0f; (*v++).f = -1.0f; (*v++).f = 0.0f; (*v++).u = rgba; // 15
        // left
        (*v++).f = x1; (*v++).f = y1; (*v++).f = z2; (*v++).f = -1.0f; (*v++).f = 0.0f; (*v++).f = 0.0f; (*v++).u = rgba; // 16
        (*v++).f = x1; (*v++).f = y1; (*v++).f = z1; (*v++).f = -1.0f; (*v++).f = 0.0f; (*v++).f = 0.0f; (*v++).u = rgba; // 17
        (*v++).f = x1; (*v++).f = y2; (*v++).f = z1; (*v++).f = -1.0f; (*v++).f = 0.0f; (*v++).f = 0.0f; (*v++).u = rgba; // 18
        (*v++).f = x1; (*v++).f = y2; (*v++).f = z2; (*v++).f = -1.0f; (*v++).f = 0.0f; (*v++).f = 0.0f; (*v++).u = rgba; // 19
        // right
        (*v++).f = x2; (*v++).f = y1; (*v++).f = z1; (*v++).f = 1.0f; (*v++).f = 0.0f; (*v++).f = 0.0f; (*v++).u = rgba; // 20
        (*v++).f = x2; (*v++).f = y1; (*v++).f = z2; (*v++).f = 1.0f; (*v++).f = 0.0f; (*v++).f = 0.0f; (*v++).u = rgba; // 21
        (*v++).f = x2; (*v++).f = y2; (*v++).f = z2; (*v++).f = 1.0f; (*v++).f = 0.0f; (*v++).f = 0.0f; (*v++).u = rgba; // 22
        (*v++).f = x2; (*v++).f = y2; (*v++).f = z1; (*v++).f = 1.0f; (*v++).f = 0.0f; (*v++).f = 0.0f; (*v++).u = rgba; // 23
        __global uint *i = &indexBuffer[36 * p];
        uint vert = 24 * p;
        // front
        *(i++) = vert + 0; *(i++) = vert + 2; *(i++) = vert + 1;
        *(i++) = vert + 0; *(i++) = vert + 3; *(i++) = vert + 2;
        // back
        *(i++) = vert + 4; *(i++) = vert + 5; *(i++) = vert + 6;
        *(i++) = vert + 6; *(i++) = vert + 7; *(i++) = vert + 4;
        // top
        *(i++) = vert + 8; *(i++) = vert + 10; *(i++) = vert + 9;
        *(i++) = vert + 8; *(i++) = vert + 11; *(i++) = vert + 10;
        // bottom
        *(i++) = vert + 12; *(i++) = vert + 13; *(i++) = vert + 14;
        *(i++) = vert + 14; *(i++) = vert + 15; *(i++) = vert + 12;
        // left
        *(i++) = vert + 16; *(i++) = vert + 18; *(i++) = vert + 17;
        *(i++) = vert + 16; *(i++) = vert + 19; *(i++) = vert + 18;
        // right
        *(i++) = vert + 20; *(i++) = vert + 21; *(i++) = vert + 22;
        *(i++) = vert + 22; *(i++) = vert + 23; *(i++) = vert + 20;
    }
}

// <numSprings>
__kernel void createVertexBufferEdges(__global VecType *pointPosition,
                                      __global uint *springColor,
                                      __global uint *springPointIndex1,
                                      __global uint *springPointIndex2,
                                      __global union BufferType *vertexBuffer,
                                      __global uint *indexBuffer,
                                      VecType minBound,
                                      VecType maxBound,
                                      VecType viewBound,
                                      VecType viewOffset,
                                      __global uint *d,
                                      int nDim)
{
    const int s = get_global_id(0);
    const int p1 = springPointIndex1[s];
    const int p2 = springPointIndex2[s];
    const uint rgba = springColor[s];
    const VecType pos1 = pointPosition[p1];
    const VecType pos2 = pointPosition[p2];
    const VecType bound = maxBound - minBound;
    const VecType vec1 = (pos1 - minBound) / bound * viewBound + viewOffset;
    const VecType vec2 = (pos2 - minBound) / bound * viewBound + viewOffset;
    const float3 proj1 = (float3)(vec1.x * (d[0] == 0) + vec1.y * (d[0] == 1) + vec1.z * (d[0] == 2) + vec1.w * (d[0] == 3),
                                  vec1.x * (d[1] == 0) + vec1.y * (d[1] == 1) + vec1.z * (d[1] == 2) + vec1.w * (d[1] == 3),
                                  vec1.x * (d[2] == 0) + vec1.y * (d[2] == 1) + vec1.z * (d[2] == 2) + vec1.w * (d[2] == 3));
    const float3 proj2 = (float3)(vec2.x * (d[0] == 0) + vec2.y * (d[0] == 1) + vec2.z * (d[0] == 2) + vec2.w * (d[0] == 3),
                                  vec2.x * (d[1] == 0) + vec2.y * (d[1] == 1) + vec2.z * (d[1] == 2) + vec2.w * (d[1] == 3),
                                  vec2.x * (d[2] == 0) + vec2.y * (d[2] == 1) + vec2.z * (d[2] == 2) + vec2.w * (d[2] == 3));
    
    if (nDim == 2) {
        ScalarType x1 = proj1.x, y1 = proj1.y;
        ScalarType x2 = proj2.x, y2 = proj2.y;
        __global union BufferType *v = &vertexBuffer[6 * s];
        (*v++).f = x1; (*v++).f = y1; (*v++).u = rgba;
        (*v++).f = x2; (*v++).f = y2; (*v++).u = rgba;
        __global uint *i = &indexBuffer[2 * s];
        uint vert = 2 * s;
        *(i++) = vert + 0;
        *(i++) = vert + 1;
    } else if (nDim == 3) {
        ScalarType x1 = proj1.x, y1 = proj1.y, z1 = proj1.z;
        ScalarType x2 = proj2.x, y2 = proj2.y, z2 = proj2.z;
        __global union BufferType *v = &vertexBuffer[8 * s];
        (*v++).f = x1; (*v++).f = y1; (*v++).f = z1; (*v++).u = rgba;
        (*v++).f = x2; (*v++).f = y2; (*v++).f = z2; (*v++).u = rgba;
        __global uint *i = &indexBuffer[2 * s];
        uint vert = 2 * s;
        *(i++) = vert + 0;
        *(i++) = vert + 1;
    }
}
