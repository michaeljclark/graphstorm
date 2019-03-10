// See LICENSE for license details.

#include "EG.h"
#include "EGGL.h"
#include "EGMath.h"
#include "EGColor.h"
#include "EGText.h"
#include "EGImage.h"
#include "EGViewport.h"
#include "EGVector.h"
#include "EGResource.h"
#include "EGRenderApi.h"
#include "EGRenderApiES2.h"
#include "EGIndexArray.h"
#include "EGVertexArray.h"
#include "EGRenderBatch.h"
#include "EGRenderBatchES2.h"
#include "EGPointRegionOctree.h"
#include "EGPointRegionQuadtree.h"
#include "EGGraph.h"
#include "EGGraphData.h"
#include "EGGraphLayout.h"
#include "EGGraphRendererParams.h"
#include "EGGraphEdgeRenderer.h"
#include "EGGraphNodeRenderer.h"
#include "EGGraphEdgeLabelRenderer.h"
#include "EGGraphNodeLabelRenderer.h"
#include "EGBoundsRenderer.h"
#include "EGTreeRenderer.h"
#include "EGSpringLayout.h"

#if USE_OPENCL
#include "EGOpenCL.h"
#include "EGSpringLayoutCL.h"
#include "EGGraphEdgeRendererES2CL.h"
#include "EGGraphNodeRendererES2CL.h"
#endif

#include "EGGraphLayoutFactory.h"


/* EGGraphLayoutImpl */

EGuint EGGraphLayoutImpl::nextLayoutId = 1;


/* EGGraphLayoutFactoryImpl */

EGSpringLayout2DFactory<>               EGGraphLayoutFactoryImpl::springLayout2D;
EGSpringLayout3DFactory<>               EGGraphLayoutFactoryImpl::springLayout3D;
EGSpringLayout4DFactory<>               EGGraphLayoutFactoryImpl::springLayout4D;
EGSpringLayoutBarnesHut2DFactory<>      EGGraphLayoutFactoryImpl::springLayoutBarnesHut2D;
EGSpringLayoutBarnesHut3DFactory<>      EGGraphLayoutFactoryImpl::springLayoutBarnesHut3D;
EGSpringLayoutBarnesHut4DFactory<>      EGGraphLayoutFactoryImpl::springLayoutBarnesHut4D;

#if USE_OPENCL
EGSpringLayout2DCLFactory<>             EGGraphLayoutFactoryImpl::springLayout2DCL;
EGSpringLayout3DCLFactory<>             EGGraphLayoutFactoryImpl::springLayout3DCL;
EGSpringLayout4DCLFactory<>             EGGraphLayoutFactoryImpl::springLayout4DCL;
EGSpringLayoutBarnesHut2DCLFactory<>    EGGraphLayoutFactoryImpl::springLayoutBarnesHut2DCL;
EGSpringLayoutBarnesHut3DCLFactory<>    EGGraphLayoutFactoryImpl::springLayoutBarnesHut3DCL;
EGSpringLayoutBarnesHut4DCLFactory<>    EGGraphLayoutFactoryImpl::springLayoutBarnesHut4DCL;
#endif


/* EGGraphLayoutFactory */

std::vector<std::string> EGGraphLayoutFactory::factoryNames;
EGGraphLayoutFactoryList EGGraphLayoutFactory::factories;

void EGGraphLayoutFactory::addFactory(EGGraphLayoutFactory *factory)
{
    if (std::find(factoryNames.begin(), factoryNames.end(), factory->name()) == factoryNames.end()) {
        factoryNames.push_back(factory->name());
    }
    factories.push_back(factory);
}

EGGraphLayoutFactory* EGGraphLayoutFactory::getFactory(std::string name, int numDimensions, EGbool requiresOpenCL)
{
    for (EGGraphLayoutFactoryList::iterator fi = factories.begin(); fi != factories.end(); fi++) {
        EGGraphLayoutFactory *factory = *fi;
        if (factory->name() == name && factory->numDimensions() == numDimensions && factory->requiresOpenCL() == requiresOpenCL) {
            return factory;
        }
    }
    return NULL;
}

void EGGraphLayoutFactory::init()
{
    if (factories.size() > 0) return;
    
    addFactory(&EGGraphLayoutFactoryImpl::springLayout2D);
    addFactory(&EGGraphLayoutFactoryImpl::springLayout3D);
    addFactory(&EGGraphLayoutFactoryImpl::springLayout4D);
    addFactory(&EGGraphLayoutFactoryImpl::springLayoutBarnesHut2D);
    addFactory(&EGGraphLayoutFactoryImpl::springLayoutBarnesHut3D);
    addFactory(&EGGraphLayoutFactoryImpl::springLayoutBarnesHut4D);
    
#if USE_OPENCL
    addFactory(&EGGraphLayoutFactoryImpl::springLayout2DCL);
    addFactory(&EGGraphLayoutFactoryImpl::springLayout3DCL);
    addFactory(&EGGraphLayoutFactoryImpl::springLayout4DCL);
    addFactory(&EGGraphLayoutFactoryImpl::springLayoutBarnesHut2DCL);
    addFactory(&EGGraphLayoutFactoryImpl::springLayoutBarnesHut3DCL);
    addFactory(&EGGraphLayoutFactoryImpl::springLayoutBarnesHut4DCL);
#endif
}
