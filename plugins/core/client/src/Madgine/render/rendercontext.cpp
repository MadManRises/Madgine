#include "../clientlib.h"
#include "../gui/widgets/toplevelwindow.h"
#include "Interfaces/window/windowapi.h"
#include "Madgine/app/application.h"
#include "rendertarget.h"

#include "Modules/debug/profiler/profiler.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Modules/generic/reverseIt.h"

#include "Modules/generic/safeiterator.h"

RegisterType(Engine::Render::RenderContext)

    namespace Engine
{
    namespace Render {

        RenderContext::RenderContext()
        {
        }

        RenderContext::~RenderContext()
        {
            assert(mRenderTargets.empty());
        }

        void RenderContext::addRenderTarget(RenderTarget *target)
        {
            mRenderTargets.push_back(target);
        }

        void RenderContext::removeRenderTarget(RenderTarget *target)
        {
            mRenderTargets.erase(std::find(mRenderTargets.begin(), mRenderTargets.end(), target));
        }

        void RenderContext::render()
        {
            for (RenderTarget *target : safeIterate(reverseIt(mRenderTargets)))
                target->render();
        }

    }
}