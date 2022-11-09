#include "../clientlib.h"

#include "renderpass.h"

#include "renderdata.h"

namespace Engine {
namespace Render {

    void RenderPass::preRender()
    {
        for (RenderData *dep : mDependencies)
            dep->update();
    }

    void RenderPass::addDependency(RenderData *dep)
    {
        mDependencies.push_back(dep);
    }

    void RenderPass::removeDependency(RenderData *dep)
    {
        std::erase(mDependencies, dep);
    }

}
}