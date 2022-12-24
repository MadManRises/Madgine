#include "../widgetslib.h"

#include "scenewindow.h"

#include "widgetmanager.h"

#include "Meta/math/vector4.h"

#include "Madgine/window/mainwindow.h"

#include "Madgine/render/rendercontext.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine/render/rendertarget.h"

#include "util/renderdata.h"

METATABLE_BEGIN_BASE(Engine::Widgets::SceneWindow, Engine::Widgets::WidgetBase)
PROPERTY(Sampled, sampled, setSampled)
METATABLE_END(Engine::Widgets::SceneWindow)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::SceneWindow, Engine::Widgets::WidgetBase)
ENCAPSULATED_FIELD(Sampled, sampled, setSampled)
SERIALIZETABLE_END(Engine::Widgets::SceneWindow)

namespace Engine {
namespace Widgets {

    SceneWindow::SceneWindow(WidgetManager &manager, WidgetBase *parent)
        : Widget(manager, parent)
    {
        mTarget = manager.window().getRenderer()->createRenderTexture({ 1, 1 }, { .mSamples = mSampled ? 4u : 1u });
        manager.addDependency(mTarget.get());
    }

    SceneWindow::~SceneWindow()
    {
        manager().removeDependency(mTarget.get());
    }

    void SceneWindow::vertices(WidgetsRenderData &renderData, size_t layer)
    {
        Vector3 pos { getAbsolutePosition(), static_cast<float>(depth(layer)) };
        Vector3 size = getAbsoluteSize();

        renderData.mVertexData[{mTarget->texture()}].renderQuad(pos, size.xy());
    }

    Render::RenderTarget *SceneWindow::getRenderTarget()
    {
        return mTarget.get();
    }

    void SceneWindow::sizeChanged(const Vector3i &pixelSize)
    {
        mTarget->resize(pixelSize.xy());
    }

    WidgetClass SceneWindow::getClass() const
    {
        return WidgetClass::SCENEWINDOW;
    }

    bool SceneWindow::sampled() const
    {
        return mSampled;
    }

    void SceneWindow::setSampled(bool sampled)
    {
        if (sampled != mSampled) {
            mSampled = sampled;

            std::vector<Render::RenderPass *> passes = mTarget->renderPasses();

            manager().removeDependency(mTarget.get());
            mTarget = mTarget->context()->createRenderTexture(mTarget->size(), { .mSamples = mSampled ? 4u : 1u });
            manager().addDependency(mTarget.get());

            for (Render::RenderPass *pass : passes)
                mTarget->addRenderPass(pass);
        }
    }

}
}
