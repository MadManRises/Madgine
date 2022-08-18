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
METATABLE_END(Engine::Widgets::SceneWindow)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::SceneWindow, Engine::Widgets::WidgetBase)
SERIALIZETABLE_END(Engine::Widgets::SceneWindow)

namespace Engine {
namespace Widgets {

    SceneWindow::SceneWindow(WidgetManager &manager, WidgetBase *parent)
        : Widget(manager, parent)
    {
        mTarget = manager.window().getRenderer()->createRenderTexture({ 1, 1 }, { .mSamples = 1 });
    }

    SceneWindow::~SceneWindow()
    {
    }

    std::vector<std::pair<std::vector<Vertex>, TextureSettings>> SceneWindow::vertices(const Vector3 &screenSize)
    {
        Vector3 pos = (getEffectivePosition() * screenSize) / screenSize;
        Vector3 size = (getEffectiveSize() * screenSize) / screenSize;
        pos.z = depth();

        std::vector<Vertex> result;

        RenderData::renderQuad(result, pos, size.xy());

        return { { result, { mTarget->texture() } } };
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

    void SceneWindow::preRender()
    {
        mTarget->render();
    }

}
}
