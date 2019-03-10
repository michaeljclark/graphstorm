// See LICENSE for license details.

#pragma once

typedef std::map<EGGraphEdge*,EGTextPtr> EGGraphEdgeTextMap;

/* EGGraphEdgeLabelRenderer */

template <typename VecType, typename GraphLayoutType>
class EGGraphEdgeLabelRenderer : public EGGraphEdgeRenderer<VecType>
{
public:
    typedef EGGraphPoint<VecType> EGGraphPointType;
    typedef EGGraphLayout<VecType> EGGraphLayoutType;
    typedef typename EGGraphLayoutType::LayoutPtr EGGraphLayoutPtr;
    
protected:
    EGGraphPtr graph;
    EGGraphLayoutPtr layout;
    EGGraphEdgeTextMap textMap;
    
public:
    EGGraphEdgeLabelRenderer(EGGraphPtr graph, EGGraphLayoutPtr layout) : graph(graph), layout(layout)
    {
        EGDebug("%s initialized\n", __func__);
    }
    
    void update(EGViewportPtr &viewport, EGGraphRendererParams<VecType> &params)
    {
        // get graph bounds
        VecType bound = params.maxBound - params.minBound;

        // fill dimension array
        EGint d[3];
        for (EGint i = 0; i < params.viewDimensions; i++) {
            d[i] = params.visibleDimensions[i];
        }

        // create text objects
        EGGraphEdgeList *edges = graph->getEdgeList();
        for (EGGraphEdgeList::iterator ei = edges->begin(); ei != edges->end(); ei++) {
            EGGraphEdge *edge = *ei;
            if (edge->getName().size() == 0) continue;
            EGGraphNode *node1 = edge->getSource();
            EGGraphNode *node2 = edge->getTarget();
            EGGraphPoint<VecType> *point1 = layout->getPoint(node1);
            EGGraphPoint<VecType> *point2 = layout->getPoint(node2);
            VecType pos1 = point1->getPosition();
            VecType pos2 = point2->getPosition();
            VecType pos = (pos1 + pos2) * 0.5f;
            VecType proj = (pos - params.minBound) / bound * params.viewBound + params.viewOffset;

            EGfloat wproj[4];
            if (params.viewDimensions == 2) {
                EGfloat wvec[4] = { proj[d[0]], proj[d[1]], 0, 1 };
                EGMath::multMat4fVec4fHomogenousDivide(wproj, wvec, viewport->modelViewProjectionMatrix);
            } else if (params.viewDimensions == 3) {
                EGfloat wvec[4] = { proj[d[0]], proj[d[1]], proj[d[2]], 1 };
                EGMath::multMat4fVec4fHomogenousDivide(wproj, wvec, viewport->modelViewProjectionMatrix);
            }
            EGfloat x = roundf((wproj[0] + 1.0f) * 0.5f * viewport->screenWidth);
            EGfloat y = roundf((-wproj[1] + 1.0f) * 0.5f * viewport->screenHeight);

            EGGraphEdgeTextMap::iterator ti = textMap.find(edge);
            EGTextPtr text;
            if (ti == textMap.end()) {
                text = EGText::create(EGText::defaultFontFace, 12, EGFontStyleNormal, EGTextHAlignCenter | EGTextVAlignCenter | EGTextEmbossed);
                textMap.insert(std::pair<EGGraphEdge*,EGTextPtr>(edge, text));
            } else {
                text = ti->second;
            }
            text->setText(edge->getName());
            text->setPosition(x, y);
        }
    }
    
    void draw(EGViewportPtr &, EGGraphRendererParams<VecType> &)
    {
        for (EGGraphEdgeTextMap::iterator ti = textMap.begin(); ti != textMap.end(); ti++) {
            EGTextPtr &text = ti->second;
            text->draw();
        }
    }
};
