// See LICENSE for license details.

#pragma once

typedef std::map<EGGraphNode*,EGTextPtr> EGGraphNodeTextMap;

template <typename VecType, typename GraphLayoutType>
class EGGraphNodeLabelRenderer : public EGGraphNodeRenderer<VecType>
{
public:
    typedef EGGraphPoint<VecType> EGGraphPointType;
    typedef EGGraphLayout<VecType> EGGraphLayoutType;
    typedef typename EGGraphLayoutType::LayoutPtr EGGraphLayoutPtr;

protected:
    EGGraphPtr graph;
    EGGraphLayoutPtr layout;
    EGGraphNodeTextMap textMap;
    
public:
    EGGraphNodeLabelRenderer(EGGraphPtr graph, EGGraphLayoutPtr layout) : graph(graph), layout(layout)
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
        EGGraphNodeList *nodes = graph->getNodeList();
        for (EGGraphNodeList::iterator ni = nodes->begin(); ni != nodes->end(); ni++) {
            EGGraphNode *node = *ni;
            if (node->getName().size() == 0) continue;
            EGGraphPoint<VecType> *point = layout->getPoint(node);
            VecType pos = point->getPosition();
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

            EGGraphNodeTextMap::iterator ti = textMap.find(node);
            EGTextPtr text;
            if (ti == textMap.end()) {
                text = EGText::create(EGText::defaultFontFace, 12, EGFontStyleNormal, EGTextHAlignCenter | EGTextVAlignCenter | EGTextEmbossed);
                textMap.insert(std::pair<EGGraphNode*,EGTextPtr>(node, text));
            } else {
                text = ti->second;
            }
            text->setText(node->getName());
            text->setPosition(x, y);
        }
    }
    
    void draw(EGViewportPtr &, EGGraphRendererParams<VecType> &)
    {
        for (EGGraphNodeTextMap::iterator ti = textMap.begin(); ti != textMap.end(); ti++) {
            EGTextPtr &text = ti->second;
            text->draw();
        }
    }
};
