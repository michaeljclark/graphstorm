/*
 *  EGSpringLayoutBarnesHut.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGSpringLayoutBarnesHut_H
#define EGSpringLayoutBarnesHut_H


template <typename VecType, typename NodePairType, typename ScalarType = typename VecType::value_type> struct EGSpringPoint;
template <typename VecType, typename NodePairType, typename ScalarType = typename VecType::value_type> struct EGSpring;


/* EGSpringLayoutSettings */

struct EGSpringLayoutSettings : public EGGraphLayoutSettings
{
    EGfloat stiffness;
    EGfloat repulsion;
    EGfloat centerAttraction;
    EGfloat damping;
    EGfloat timeStep;
    EGfloat maxSpeed;
    EGfloat maxEnergy;
    
    EGSpringLayoutSettings()
        : stiffness(400.0f), repulsion(400.0f), centerAttraction(0.0f), damping(0.5f), timeStep(0.03f), maxSpeed(1000.0f), maxEnergy(0.01f) {}
    
    EGSpringLayoutSettings(EGfloat stiffness, EGfloat repulsion, EGfloat centerAttraction, EGfloat damping, EGfloat timeStep, EGfloat maxSpeed, EGfloat maxEnergy)
        : stiffness(stiffness), repulsion(repulsion), centerAttraction(centerAttraction), damping(damping), timeStep(timeStep), maxSpeed(maxSpeed), maxEnergy(maxEnergy) {}
    
    std::string toString() const
    {
        std::stringstream ss;
        ss << "stiffness=" << stiffness << " repulsion=" << repulsion << " centerAttraction=" << centerAttraction << " damping=" << damping
           << " timeStep=" << timeStep << " maxSpeed=" << maxSpeed << " maxEnergy=" << maxEnergy;
        return ss.str();
    }
};


/* EGSpringPoint */

template <typename VecType, typename NodePairType, typename ScalarType>
struct EGSpringPoint : EGGraphPoint<VecType>
{
    typedef EGSpring<VecType,NodePairType> SpringType;
    typedef std::vector<SpringType*> SpringList;
    
    VecType velocity;
    VecType acceleration;
    ScalarType mass;
    SpringList springs;
    
    EGSpringPoint(EGGraphNode *node, VecType position, ScalarType mass, VecType velocity, VecType acceleration)
        : EGGraphPoint<VecType>(node, position), velocity(velocity), acceleration(acceleration), mass(mass) {}
    
    ScalarType getMass() { return mass; }
    VecType getVelocity() { return velocity; }
    VecType getAcceleration() { return acceleration; }
    
    void setMass(ScalarType mass) { this->mass = mass; }
    void setPosition(VecType position) { this->position = position; }
    void setVelocity(VecType velocity) { this->velocity = velocity; }
    void setAcceleration(VecType acceleration) { this->acceleration = acceleration; }
    
    void applyForce(VecType &force) { acceleration += force * (1.0f / mass); } // uses reciprocal of mass so the vector op is a multiply
    
    void addSpring(SpringType *spring)
    {
        auto si = std::find(springs.begin(), springs.end(), spring);
        if (si == springs.end()) {
            springs.insert(springs.end(), spring);
        }
    }
    
    void removeSpring(SpringType *spring)
    {
        auto si = std::find(springs.begin(), springs.end(), spring);
        if (si != springs.end()) {
            springs.erase(si);
        }
    }
};


/* EGSpring */

template <typename VecType, typename NodePairType, typename ScalarType>
struct EGSpring
{
    typedef EGSpringPoint<VecType,NodePairType> PointType;
    typedef EGSpring<VecType,NodePairType> SpringType;
    
    NodePairType nodepair;
    ScalarType length;
    ScalarType stiffness;
    PointType *point1;
    PointType *point2;
    EGGraphEdgeList edges;
    
    EGSpring(NodePairType nodepair, ScalarType length, ScalarType stiffness)
        : nodepair(nodepair), length(length), stiffness(stiffness), point1(NULL), point2(NULL) {}
    
    NodePairType getNodePair() { return nodepair; }
    ScalarType getLength() { return length; }
    ScalarType getStiffness() { return stiffness; }
    EGGraphEdgeList* getEdges() { return &edges; }
    EGsize numEdges() { return edges.size(); }
    
    void setLength(ScalarType length) { this->length = length; }
    void setStiffness(ScalarType stiffness) { this->stiffness = stiffness; }
    
    void addEdge(EGGraphEdge *edge)
    {
        auto ei = std::find(edges.begin(), edges.end(), edge);
        if (ei == edges.end()) {
            edges.insert(edges.end(), edge);
        }
    }
    
    void removeEdge(EGGraphEdge *edge)
    {
        auto ei = std::find(edges.begin(), edges.end(), edge);
        if (ei != edges.end()) {
            edges.erase(ei);
        }
    }
};


/* EGSpringLayoutTreeNodeContainer */

template <typename VecType>
class EGSpringLayoutTreeNodeContainer : public EGPointRegionTreeNodeContainer<VecType>
{
public:
    VecType centerOfGravity;
    typename VecType::value_type mass;
    
    EGSpringLayoutTreeNodeContainer() : EGPointRegionTreeNodeContainer<VecType>(), centerOfGravity(), mass(0) {}
    EGSpringLayoutTreeNodeContainer(const EGSpringLayoutTreeNodeContainer<VecType> &o) : EGPointRegionTreeNodeContainer<VecType>(o) {}
};


/* EGSpringLayout */

template <typename VecType,
          typename NodePairType,
          typename ScalarType,
          EGbool buildTree,
          EGbool barnesHut,
          typename PointTreeType>
class EGSpringLayout : public EGGraphLayout<VecType>
{
public:
    typedef EGSpringLayoutSettings SettingsType;
    typedef EGSpringPoint<VecType,NodePairType> PointType;
    typedef EGSpring<VecType,NodePairType> SpringType;
    typedef std::map<EGGraphNode*,PointType*> PointMap;
    typedef std::map<NodePairType,SpringType*> SpringMap;
    typedef std::pair<EGGraphNode*,PointType*> PointMapValue;
    typedef std::pair<NodePairType,SpringType*> SpringMapValue;
    typedef PointTreeType TreeType;
    
protected:
    static const EGbool debug = false;
    
    // parameters
    ScalarType stiffness;
    ScalarType repulsion;
    ScalarType centerAttraction;
    ScalarType damping;
    ScalarType timeStep;
    ScalarType maxSpeed;
    ScalarType maxEnergy;
    ScalarType multiplier;

    // map of points and springs
    PointTreeType pointtree;
    PointMap pointmap;
    SpringMap springmap;
    
    // array indexed buffers
    EGsize numPoints;
    EGsize numSprings;
    PointType** points;
    SpringType** springs;
    
    // state variables
    EGbool arraysCreated;
    EGbool running;
    EGint iterCount;
    ScalarType energy;
    
public:
    EGSpringLayout(EGSpringLayoutSettings &s)
        : stiffness(s.stiffness), repulsion(s.repulsion), centerAttraction(s.centerAttraction),
          damping(s.damping), timeStep(s.timeStep), maxSpeed(s.maxSpeed), maxEnergy(s.maxEnergy), multiplier(1.0f),
          numPoints(0), numSprings(0), points(NULL), springs(NULL), arraysCreated(false), running(false), iterCount(0), energy((std::numeric_limits<ScalarType>::max)())
    {
        EGDebug("EGSpringLayout:%s barnesHut=%s %s\n", __func__, barnesHut ? "true" : "false", s.toString().c_str());
    }
    
    virtual ~EGSpringLayout()
    {
        deleteArrays();

        EGDebug("EGSpringLayout:%s deleted\n", __func__);
    }

    void updateSettings(EGSpringLayoutSettings &s)
    {
        stiffness = s.stiffness;
        repulsion = s.repulsion;
        centerAttraction = s.centerAttraction;
        damping = s.damping;
        timeStep = s.timeStep;
        maxSpeed = s.maxSpeed;
        maxEnergy = s.maxEnergy;
    }

    void updateBuffers(EGViewportPtr &viewport, EGGraphRendererParams<VecType> &params) {}

    PointTreeType* getTree() { return &pointtree; }
    
    void getBounds(VecType &minBound, VecType &maxBound)
    {
        EGGraphNodeList *nodes = EGGraphLayout<VecType>::graph->getNodeList();
        minBound = (std::numeric_limits<ScalarType>::max)();
        maxBound = -(std::numeric_limits<ScalarType>::max)();
        for (auto ni = nodes->begin(); ni != nodes->end(); ni++) {
            VecType pos = getPoint(*ni)->getPosition();
            for (EGsize i = 0; i < pos.dim(); i++) {
                if (minBound[i] > pos[i]) minBound[i] = pos[i];
                if (maxBound[i] < pos[i]) maxBound[i] = pos[i];
            }
        }
    }
    
    /* EGGraphLayout methods */
    
    void init()
    {
        EGGraphNodeList *nodes = EGGraphLayout<VecType>::graph->getNodeList();
        EGGraphEdgeList *edges = EGGraphLayout<VecType>::graph->getEdgeList();
        
        // add points and set initial positions for all nodes
        multiplier = (ScalarType)nodes->size() / (3.1415926f * 2.0f);
        for (auto ni = nodes->begin(); ni != nodes->end(); ni++) {
            addNode(*ni);
        }
        
        // add springs and set initial values for all edges
        for (auto ei = edges->begin(); ei != edges->end(); ei++) {
            addEdge(*ei);
        }

        // sort points by weight/centrality
        //EGGraphPointWeightCompare<PointType> weightCompare;
        //std::sort(points, points + numPoints, weightCompare);

        EGDebug("EGSpringLayout:%s numPoints=%d numSprings=%d\n", __func__, (EGint)pointmap.size(), (EGint)springmap.size());

        createArrays();
    }
    
    void clear()
    {
        deleteArrays();
        pointtree.clear();
        pointmap.clear();
        springmap.clear();
    }
    
    void refreshPositions(EGbool forceRefresh)
    {
        // no-op
    }
    
    void deleteArrays()
    {
        if (!arraysCreated) return;
        
        // delete points array (acceleration structure)
        delete [] points;
        points = NULL;

        // delete springs array (acceleration structure)
        delete [] springs;
        springs = NULL;
        
        arraysCreated = false;
    }
    
    void createArrays()
    {
        if (arraysCreated) return;
        
        // create points array
        numPoints = pointmap.size();
        PointType** pptr = points = new PointType*[numPoints];
        for (auto pi = pointmap.begin(); pi != pointmap.end(); pi++) {
            *(pptr++) = (*pi).second;
        }
        
        // create springs array
        numSprings = springmap.size();
        SpringType** sptr = springs = new SpringType*[numSprings];
        for (auto si = springmap.begin(); si != springmap.end(); si++) {
            *(sptr++) = (*si).second;
        }
        
        arraysCreated = true;
    }
    
    EGGraphPoint<VecType>* getPoint(EGGraphNode *node)
    {
        auto pi = pointmap.find(node);
        return pi != pointmap.end() ? static_cast<EGGraphPoint<VecType>*>((*pi).second) : NULL;
    }
    
    void addNode(EGGraphNode *node)
    {
        // we have one point per node
        auto pi = pointmap.find(node);
        if (pi == pointmap.end()) {
            VecType position = 0.0f;
            if (position.dim() == 1) {
                /* purely random coordinates */
                position[0] = multiplier * ((ScalarType)rand() / (ScalarType)RAND_MAX);
            } else if (position.dim() == 2) {
                /* random coordinates on a circle */
                ScalarType phi = 3.1415927f * 2.0f * ((ScalarType)rand() / (ScalarType)RAND_MAX);
                position[0] = cosf(phi);
                position[1] = sinf(phi);
            } else if (position.dim() == 3) {
                /* random coordinates on a sphere */
                ScalarType phi = 3.1415927f * 2.0f * ((ScalarType)rand() / (ScalarType)RAND_MAX);
                ScalarType theta = 3.1415927f * 2.0f * ((ScalarType)rand() / (ScalarType)RAND_MAX);
                position[0] = multiplier * sinf(theta) * cosf(phi);
                position[1] = multiplier * sinf(theta) * sinf(phi);
                position[2] = multiplier * cosf(theta);
            } else if (position.dim() == 4) {
                /* random coordinates on a 3-sphere */
                ScalarType phi = 3.1415927f * 2.0f * ((ScalarType)rand() / (ScalarType)RAND_MAX);
                ScalarType theta = 3.1415927f * 2.0f * ((ScalarType)rand() / (ScalarType)RAND_MAX);
                ScalarType kappa = 3.1415927f * 2.0f * ((ScalarType)rand() / (ScalarType)RAND_MAX);
                position[0] = multiplier * sinf(theta) * cosf(phi);
                position[1] = multiplier * sinf(theta) * sinf(phi) * cosf(kappa);
                position[2] = multiplier * cosf(theta);
                position[3] = multiplier * sinf(theta) * sinf(phi) * sinf(kappa);
            }
            PointType *point = new PointType(node, position, node->getWeight(), 0.0f, 0.0f);
            pointmap.insert(PointMapValue(node, point));
        }
        deleteArrays();
        running = true;
    }
    
    void addEdge(EGGraphEdge *edge)
    {
        NodePairType nodepair(edge);
        auto si = springmap.find(nodepair);
        if (si == springmap.end()) {
            SpringType *spring = new SpringType(nodepair, edge->getLength(), stiffness);
            spring->point1 = pointmap[edge->getSource()];
            spring->point2 = pointmap[edge->getTarget()];
            springmap.insert(SpringMapValue(nodepair, spring));
            spring->addEdge(edge);
        } else {
            SpringType *spring = (*si).second;
            EGsize numEdges = spring->numEdges();
            ScalarType r = 1.0f / (numEdges + 1);
            spring->length = spring->length * (1.0f - r) + edge->getLength() * r;
            spring->addEdge(edge);
        }
        deleteArrays();
        running = true;
    }

    void removeNode(EGGraphNode *node)
    {
        auto pi = pointmap.find(node);
        if (pi != pointmap.end()) {
            pointmap.erase(pi);
        }
        deleteArrays();
        running = true;
    }
    
    void removeEdge(EGGraphEdge *edge)
    {
        // find the spring for the node pair of this edge
        // (as there can be more than one edge between the same two nodes)
        NodePairType nodepair(edge);
        auto si = springmap.find(nodepair);
        if (si != springmap.end()) {
            SpringType *spring = (*si).second;
            spring->removeEdge(edge);
            // if this is the last edge, then remove the spring
            if (spring->numEdges() == 0) {
                springmap.erase(si);
            }
        }
        deleteArrays();
        running = true;
    }
    
    void updateNode(EGGraphNode *node)
    {
        auto pi = pointmap.find(node);
        if (pi != pointmap.end()) {
            PointType *point = (*pi).second;
            // update weight
            point->mass = node->getWeight();
        }
        deleteArrays();
        running = true;
    }
    
    void updateEdge(EGGraphEdge *edge)
    {
        NodePairType nodepair(edge);
        auto si = springmap.find(nodepair);
        if (si != springmap.end()) {
            SpringType *spring = (*si).second;
            assert(spring->numEdges() > 0);
            // update length (maximum of all edges common to this spring)
            spring->length = 0;
            EGGraphEdgeList *edges = spring->getEdges();
            for (auto ei = edges->begin(); ei != edges->end(); ei++) {
                EGGraphEdge *edge = *ei;
                spring->length = (std::max)(spring->length, edge->getLength());
            }
        }
        deleteArrays();
        running = true;
    }
    
    EGint getNumIterations() { return iterCount; }
    
    void step()
    {
        if (!running) return;
        
        // convert maps to arrays for faster iteration
        createArrays();
        
        // create the point region tree
        if (buildTree) {
            createTree();
        }

        // apply coulombs law
        if (barnesHut) {
            updateTreeCenterOfGravity();
            applyCoulombsLawBarnesHut();
        } else {
            applyCoulombsLaw();
        }

        applyHookesLaw();
        if (centerAttraction > 0) {
            attractToCenter();
        }
        updatePosition((ScalarType)timeStep);
        
        // update iteration count
        iterCount++;

        // stop simulation when energy of the system goes below a threshold
        ScalarType energy = totalEnergy();
        if (debug) {
            EGDebug("EGSpringLayout:%s totalEnergy=%f\n", __func__, energy);
        }
        if (energy < maxEnergy) {
            EGDebug("EGSpringLayout:%s stopped: totalEnergy=%f\n", __func__, energy);
            running = false;
        }
    }
    
    EGbool isRunning()
    {
        return running;
    }
    
    /* EGSpringLayout methods */
    
    void createTree()
    {
        pointtree.clear();
        for (EGsize i = 0; i < numPoints; i++) {
            PointType *point = points[i];
            pointtree.insert(point);
        }
    }
    
    void updateTreeCenterOfGravity()
    {
        struct CenterOfGravityVisitor : EGTreeNodeVisitor
        {
            void preDescend(EGTreeNode *node) {}
            
            EGbool shouldDescend(EGTreeNode *node) { return true; }

            void postDescend(EGTreeNode *node)
            {
                if (node->getNodeKind() == EGTreeNodeLeaf) {
                    // we've reached the bottom of the tree
                    // calculate center of gravity and mass on the points of this leaf
                    ScalarType mass = 0;
                    VecType centerOfGravity;
                    typename TreeType::EGLeafNodeType *leaf = static_cast<typename TreeType::EGLeafNodeType*>(node);
                    for (EGsize idx = 0; idx < leaf->storage.count(); idx++) {
                        PointType *point = leaf->storage.at(idx);
                        mass += point->mass;
                        centerOfGravity += point->position * point->mass;
                    }
                    centerOfGravity /= mass;
                    // update leaf center of gravity and mass
                    leaf->container.mass = mass;
                    leaf->container.centerOfGravity = centerOfGravity;
                }
                if (node->getNodeKind() == EGTreeNodeBranch) {
                    // we are popping back up the tree
                    // calculate center of gravity and mass on the child nodes of this branch
                    ScalarType mass = 0;
                    VecType centerOfGravity;
                    typename TreeType::EGBranchNodeType *branch = static_cast<typename TreeType::EGBranchNodeType*>(node);
                    for (EGsize idx = 0; idx < branch->childCount(); idx++) {
                        typename TreeType::EGTreeNodeType *child = static_cast<typename TreeType::EGTreeNodeType*>(branch->childAt(idx));
                        if (!child) continue;
                        mass += child->container.mass;
                        centerOfGravity += child->container.centerOfGravity * child->container.mass;
                    }
                    centerOfGravity /= mass;
                    // update branch center of gravity and mass
                    branch->container.mass = mass;
                    branch->container.centerOfGravity = centerOfGravity;
                }
            }
        };
        CenterOfGravityVisitor centerOfGravityVistor;
        pointtree.visit(centerOfGravityVistor);
    }

    void applyCoulombsLawBarnesHut()
    {
        struct BarnesHutInnerPass : EGTreeNodeVisitor
        {
            PointType *point1;
            ScalarType distanceLimit;
            ScalarType repulsion;
            EGint depth;
            EGbool descend;
            EGbool leafonly;
            
            BarnesHutInnerPass(PointType *point1, ScalarType distanceLimit, ScalarType repulsion)
                : point1(point1), distanceLimit(distanceLimit), repulsion(repulsion), descend(true) {}
            
            void preDescend(EGTreeNode *node)
            {                
                typename TreeType::EGTreeNodeType *treenode = static_cast<typename TreeType::EGTreeNodeType*>(node);
                
                VecType direction = point1->position - treenode->container.centerOfGravity;
                ScalarType distance = direction.magnitude();
                ScalarType mass = treenode->container.mass;
                
                // apply barnes hut optmization (force of node center of gravity)
                // if the node container center of gravity is over the distance limit
                //
                // NOTE: we also make sure the distance is greater than the node width to avoid applying the
                // center of gravity of the root node
                if (distance > distanceLimit && distance > treenode->container.half.magnitude() * 2.0f) {
                    direction *= 1.0f / (std::max)(distance, 0.1f);
                    
                    // apply force to each end point
                    ScalarType f = repulsion / (std::max)((distance * distance * 0.5f), 0.005f);
                    VecType force1 = direction * f * mass;
                    point1->applyForce(force1);
                    
                    descend = false;
                    return;
                }
                if (node->getNodeKind() == EGTreeNodeLeaf)
                {
                    typename TreeType::EGLeafNodeType *leaf = static_cast<typename TreeType::EGLeafNodeType*>(node);
                    
                    // apply force of individual points
                    for (EGsize idx = 0; idx < leaf->storage.count(); idx++) {
                        PointType *point2 = leaf->storage.at(idx);
                        
                        // get distance between the points
                        VecType direction = point1->position - point2->position;
                        ScalarType distance = direction.magnitude();
                        direction *= 1.0f / (std::max)(distance, 0.1f);
                        
                        // apply force to each end point
                        ScalarType f = repulsion / (std::max)((distance * distance * 0.5f), 0.005f);
                        VecType force1 = direction * f * point2->mass;
                        point1->applyForce(force1);
                    }
                }
            }
            
            EGbool shouldDescend(EGTreeNode *node) { return descend; }
            
            void postDescend(EGTreeNode *node) { descend = true; }
        };
        
        // get bounds
        VecType minBound, maxBound;
        getBounds(minBound, maxBound);
        VecType bound = (maxBound - minBound);
        ScalarType distanceLimit = bound.magnitude() / 8;

        // loop through all points
        for (EGsize i = 0; i < numPoints; i++) {
            BarnesHutInnerPass innerpass(points[i], distanceLimit, repulsion);
            pointtree.visit(innerpass);
        }
    }

    void applyCoulombsLaw()
    {
        // iterate through every pair of points applying repulsize forces
        for (EGsize i = 0; i < numPoints; i++) {
            for (EGsize j = i; j < numPoints; j++) {
                if (i == j) continue;
                PointType *point1 = points[i];
                PointType *point2 = points[j];
                
                // get distance between the points
                VecType direction = point1->position - point2->position;
                ScalarType distance = direction.magnitude();
                direction *= 1.0f / (std::max)(distance, 0.1f); // could call normalize() but we re-use the magnitude (avoid 2nd sqrt)
                
                // apply force to each end point
                ScalarType f = repulsion / (std::max)((distance * distance * 0.5f), 0.005f);
                VecType force1 = direction * f;
                VecType force2 = direction * -f;
                point1->applyForce(force1);
                point2->applyForce(force2);
            }
        }
    }
    
    void applyHookesLaw()
    {
        // iterate through every spring applying attractive forces
        for (EGsize i = 0; i < numSprings; i++) {
            SpringType *spring = springs[i];
            PointType *point1 = spring->point1;
            PointType *point2 = spring->point2;
            
            // get the direction of the spring
            VecType direction = point2->position - point1->position;
            ScalarType distance = direction.magnitude();
            ScalarType displacement = spring->length - distance;
            direction *= 1.0f / (std::max)(distance, 0.1f); // could call normalize() but we re-use the magnitude (avoid 2nd sqrt)
            
            // apply forces
            ScalarType f = spring->stiffness * displacement * 0.5f;
            VecType force1 = direction * -f;
            VecType force2 = direction * f;
            point1->applyForce(force1);
            point2->applyForce(force2);
        }
    }
    
    void attractToCenter()
    {
        // attract points to the center
        for (EGsize i = 0; i < numPoints; i++) {
            PointType *point = points[i];
            
            VecType direction = point->position * -1.0f;
            VecType force = direction * centerAttraction;
            point->applyForce(force);
        }
    }
    
    void updatePosition(EGfloat t)
    {
        // update position for all points
        for (EGsize i = 0; i < numPoints; i++) {
            PointType *point = points[i];

            // update velocity
            point->velocity = (point->velocity + point->acceleration * t) * damping;
            ScalarType speed = point->velocity.magnitude();
            if (speed > maxSpeed) {
                point->velocity *= maxSpeed / speed;
            }
            point->acceleration = 0.0f;

            // update position
            point->position += point->velocity * t;
        }
    }
        
    EGfloat totalEnergy()
    {
        // Calculate the total energy of the system
        energy = 0;
        for (EGsize i = 0; i < numPoints; i++) {
            PointType *point = points[i];
            
            // calculate this points energy
            ScalarType speed = point->velocity.magnitude();
            energy += 0.5f * point->mass * speed * speed;
        }
        return (EGfloat)energy;
    }

    void stop()
    {
        running = false;
    }
};

/* EGSpringLayout2D */

template <typename VecType,
          typename NodePairType = EGGraphNodePairOrdered,
          typename ScalarType = typename VecType::value_type,
          typename PointTreeType = EGPointRegionQuadtree<EGSpringPoint<VecType,NodePairType>*,EGGraphPointAdaptor<VecType>,VecType,EGSpringLayoutTreeNodeContainer<VecType>>>
class EGSpringLayout2D : public EGSpringLayout<VecType,NodePairType,ScalarType,false,false,PointTreeType>
{
public:
    EGSpringLayout2D(EGSpringLayoutSettings &s) : EGSpringLayout<VecType,NodePairType,ScalarType,false,false,PointTreeType>(s) {}
};


/* EGSpringLayout3D */

template <typename VecType,
          typename NodePairType = EGGraphNodePairOrdered,
          typename ScalarType = typename VecType::value_type,
          typename PointTreeType = EGPointRegionOctree<EGSpringPoint<VecType,NodePairType>*,EGGraphPointAdaptor<VecType>,VecType,EGSpringLayoutTreeNodeContainer<VecType>>>
class EGSpringLayout3D : public EGSpringLayout<VecType,NodePairType,ScalarType,false,false,PointTreeType>
{
public:
    EGSpringLayout3D(EGSpringLayoutSettings &s) : EGSpringLayout<VecType,NodePairType,ScalarType,false,false,PointTreeType>(s) {}
};


/* EGSpringLayoutBarnesHut2D */

template <typename VecType,
          typename NodePairType = EGGraphNodePairOrdered,
          typename ScalarType = typename VecType::value_type,
          typename PointTreeType = EGPointRegionQuadtree<EGSpringPoint<VecType,NodePairType>*,EGGraphPointAdaptor<VecType>,VecType,EGSpringLayoutTreeNodeContainer<VecType>>>
class EGSpringLayoutBarnesHut2D : public EGSpringLayout<VecType,NodePairType,ScalarType,true,true,PointTreeType>
{
public:
    EGSpringLayoutBarnesHut2D(EGSpringLayoutSettings &s) : EGSpringLayout<VecType,NodePairType,ScalarType,true,true,PointTreeType>(s) {}
};


/* EGSpringLayoutBarnesHut3D */

template <typename VecType,
          typename NodePairType = EGGraphNodePairOrdered,
          typename ScalarType = typename VecType::value_type,
          typename PointTreeType = EGPointRegionOctree<EGSpringPoint<VecType,NodePairType>*,EGGraphPointAdaptor<VecType>,VecType,EGSpringLayoutTreeNodeContainer<VecType>>>
class EGSpringLayoutBarnesHut3D : public EGSpringLayout<VecType,NodePairType,ScalarType,true,true,PointTreeType>
{
public:
    EGSpringLayoutBarnesHut3D(EGSpringLayoutSettings &s) : EGSpringLayout<VecType,NodePairType,ScalarType,true,true,PointTreeType>(s) {}
};


#endif
