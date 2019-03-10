// See LICENSE for license details.

#pragma once

struct EGGraphModifier;
typedef std::shared_ptr<EGGraphModifier> EGGraphModifierPtr;
typedef std::vector<EGGraphModifierPtr> EGGraphModifierList;

/* EGGraphNodeMassType */

enum EGGraphNodeMassType {
    EGGraphNodeMassTypeUserDefined,
    EGGraphNodeMassTypeConstant,
    EGGraphNodeMassTypeDegree,
};


/* EGGraphNodeSizeType */

enum EGGraphNodeSizeType {
    EGGraphNodeSizeTypeUserDefined,
    EGGraphNodeSizeTypeConstant,
    EGGraphNodeSizeTypeLogDegree,
};


/* EGGraphNodeColorType */

enum EGGraphNodeColorType {
    EGGraphNodeColorTypeUserDefined,
    EGGraphNodeColorTypeConstant,
    EGGraphNodeColorTypeDegree,
};


/* EGGraphEdgeColorType */

enum EGGraphEdgeColorType {
    EGGraphEdgeColorTypeUserDefined,
    EGGraphEdgeColorTypeConstant,
    EGGraphEdgeColorTypeLength,
};


/* EGGraphModifier */

struct EGGraphModifier
{
    virtual ~EGGraphModifier() {}
    
    virtual void modify(EGGraphPtr graph) = 0;
    
    static void modify(EGGraphPtr graph, EGGraphModifierList &modifierList)
    {
        for (auto mli = modifierList.begin(); mli != modifierList.end(); mli++) {
            EGGraphModifierPtr &modifier = *mli;
            modifier->modify(graph);
        }
    }
};


/* EGGraphNodeMassModifier */

struct EGGraphNodeMassModifier : EGGraphModifier
{
    EGGraphNodeMassType nodeMassType;
    EGfloat nodeMassConstant;
    
    EGGraphNodeMassModifier(EGGraphNodeMassType nodeMassType, EGfloat nodeMassConstant)
        : nodeMassType(nodeMassType), nodeMassConstant(nodeMassConstant) {}
    
    void modify(EGGraphPtr graph)
    {
        switch (nodeMassType) {
            case EGGraphNodeMassTypeUserDefined:
                break;
            case EGGraphNodeMassTypeConstant:
            {
                EGGraphNodeList *nodes = graph->getNodeList();
                for (auto ni = nodes->begin(); ni != nodes->end(); ni++) {
                    EGGraphNode *node = *ni;
                    node->setWeight(nodeMassConstant);
                }
                break;
            }
            case EGGraphNodeMassTypeDegree:
            {
                EGGraphNodeList *nodes = graph->getNodeList();
                for (auto ni = nodes->begin(); ni != nodes->end(); ni++) {
                    EGGraphNode *node = *ni;
                    EGint degreeCentrality = (EGint)node->getEdges()->size();
                    node->setWeight(degreeCentrality * nodeMassConstant);
                }
                break;
            }
        }
    }
};


/* EGGraphNodeSizeModifier */

struct EGGraphNodeSizeModifier : EGGraphModifier
{
    EGGraphNodeSizeType nodeSizeType;
    EGfloat nodeSizeDefault;
    
    EGGraphNodeSizeModifier(EGGraphNodeSizeType nodeSizeType, EGfloat nodeSizeDefault) : nodeSizeType(nodeSizeType), nodeSizeDefault(nodeSizeDefault) {}
    
    void modify(EGGraphPtr graph)
    {
        switch (nodeSizeType) {
            case EGGraphNodeSizeTypeUserDefined:
                break;
            case EGGraphNodeSizeTypeConstant:
            {
                EGGraphNodeList *nodes = graph->getNodeList();
                for (auto ni = nodes->begin(); ni != nodes->end(); ni++) {
                    EGGraphNode *node = *ni;
                    node->setData(EGGraphDataSize::create(nodeSizeDefault));
                }
                break;
            }
            case EGGraphNodeSizeTypeLogDegree:
            {
                EGGraphNodeList *nodes = graph->getNodeList();
                for (auto ni = nodes->begin(); ni != nodes->end(); ni++) {
                    EGGraphNode *node = *ni;
                    EGint degreeCentrality = (EGint)node->getEdges()->size();
                    node->setData(EGGraphDataSize::create(std::log(degreeCentrality) + 1.0f));
                }
                break;
            }
        }
    }
};


/* EGGraphColorModifier */

struct EGGraphColorModifier : EGGraphModifier
{
    void static HSVtoRGB(int &r, int &g, int &b, int h, int s, int v)
    {
        EGint f, p, q, t;
        
        if(s == 0) {
            r = g = b = v;
            return;
        }
        
        f = ((h%60)*255)/60;
        h /= 60;
        
        p = (v * (256 - s))/256;
        q = (v * (256 - (s * f) / 256)) /256;
        t = (v * (256 - (s * (256-f))/256))/256;
        
        switch( h ) {
            case 0: r = v; g = t; b = p; break;
            case 1: r = q; g = v; b = p; break;
            case 2: r = p; g = v; b = t; break;
            case 3: r = p; g = q; b = v; break;
            case 4: r = t; g = p; b = v; break;
            default: r = v; g = p; b = q; break;
        }
    }
    
    static EGColor colorForValue(EGfloat t)
    {
        EGint r, g, b;
        EGint hue = (EGint)(360.0f * (1.0f - t));
        HSVtoRGB(r, g, b, hue, 192, 192);
        return EGColor(r/255.0f, g/255.0f, b/255.0f, 1.0f);
    }
};


/* EGGraphNodeColorModifier */

struct EGGraphNodeColorModifier : EGGraphColorModifier
{
    EGGraphNodeColorType nodeColorType;
    EGColor nodeColorDefault;
    
    EGGraphNodeColorModifier(EGGraphNodeColorType nodeColorType, EGColor nodeColorDefault) : nodeColorType(nodeColorType), nodeColorDefault(nodeColorDefault) {}
 
    void modify(EGGraphPtr graph)
    {
        switch (nodeColorType) {
            case EGGraphNodeColorTypeUserDefined:
                break;
            case EGGraphNodeColorTypeConstant:
            {
                EGGraphNodeList *nodes = graph->getNodeList();
                for (auto ni = nodes->begin(); ni != nodes->end(); ni++) {
                    EGGraphNode *node = *ni;
                    node->setData(EGGraphDataColor::create(nodeColorDefault));
                }
                break;
            }
            case EGGraphNodeColorTypeDegree:
            {
                EGGraphNodeList *nodes = graph->getNodeList();
                EGint maxDegreeCentrality = 0;
                for (auto ni = nodes->begin(); ni != nodes->end(); ni++) {
                    EGGraphNode *node = *ni;
                    EGint degreeCentrality = (EGint)node->getEdges()->size();
                    maxDegreeCentrality = (std::max)(maxDegreeCentrality, degreeCentrality);
                }
                for (auto ni = nodes->begin(); ni != nodes->end(); ni++) {
                    EGGraphNode *node = *ni;
                    EGint degreeCentrality = (EGint)node->getEdges()->size();
                    EGfloat t = (maxDegreeCentrality > 0) ? (EGfloat)degreeCentrality / (EGfloat)maxDegreeCentrality : 1.0f;
                    EGColor color = colorForValue(t);
                    node->setData(EGGraphDataColor::create(color));
                }
                break;
            }
        }
    }
};


/* EGGraphEdgeColorModifier */

struct EGGraphEdgeColorModifier : EGGraphColorModifier
{
    EGGraphEdgeColorType edgeColorType;
    EGColor edgeColorDefault;
    
    EGGraphEdgeColorModifier(EGGraphEdgeColorType edgeColorType, EGColor edgeColorDefault) : edgeColorType(edgeColorType), edgeColorDefault(edgeColorDefault) {}
    
    void modify(EGGraphPtr graph)
    {
        switch (edgeColorType) {
            case EGGraphEdgeColorTypeUserDefined:
                break;
            case EGGraphEdgeColorTypeConstant:
            {
                EGGraphEdgeList *edges = graph->getEdgeList();
                for (auto ei = edges->begin(); ei != edges->end(); ei++) {
                    EGGraphEdge *edge = *ei;
                    edge->setData(EGGraphDataColor::create(edgeColorDefault));
                }
                break;
            }
            case EGGraphEdgeColorTypeLength:
            {
                EGGraphEdgeList *edges = graph->getEdgeList();
                EGfloat maxLength = 0;
                for (auto ei = edges->begin(); ei != edges->end(); ei++) {
                    EGGraphEdge *edge = *ei;
                    EGfloat length = edge->getLength();
                    maxLength = (std::max)(maxLength, length);
                }
                for (auto ei = edges->begin(); ei != edges->end(); ei++) {
                    EGGraphEdge *edge = *ei;
                    EGfloat length = edge->getLength();
                    EGfloat t = (maxLength > 0) ? length / maxLength : 1.0f;
                    EGColor color = colorForValue(t);
                    edge->setData(EGGraphDataColor::create(color));
                }
                break;
            }
        }
    }
};
