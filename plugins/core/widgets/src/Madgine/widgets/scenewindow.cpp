#include "../widgetslib.h"

#include "scenewindow.h"

#include "widgetmanager.h"

#include "Meta/math/vector4.h"

#include "vertex.h"

#include "Madgine/render/camera.h"

#include "Madgine/window/mainwindow.h"

#include "Madgine/render/rendercontext.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "textureloader.h"

METATABLE_BEGIN_BASE(Engine::Widgets::SceneWindow, Engine::Widgets::WidgetBase)
METATABLE_END(Engine::Widgets::SceneWindow)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::SceneWindow, Engine::Widgets::WidgetBase)
SERIALIZETABLE_END(Engine::Widgets::SceneWindow)

namespace Engine {
namespace Widgets {

    SceneWindow::SceneWindow(const std::string &name, WidgetManager &manager, WidgetBase *parent)
        : Widget(name, manager, parent)
    {
        mTarget = manager.window().getRenderer()->createRenderTexture();
    }

    SceneWindow::~SceneWindow()
    {
    }

    std::vector<std::pair<std::vector<Vertex>, Render::TextureDescriptor>> SceneWindow::vertices(const Vector3 &screenSize)
    {
        std::vector<Vertex> result;

        Vector3 pos = (getAbsolutePosition() * screenSize) / screenSize;
        Vector3 size = (getAbsoluteSize() * screenSize) / screenSize;

        Vector4 color { 1, 1, 1, 1 };

        Render::TextureHandle tex = mTarget ? mTarget->texture()->mTextureHandle : 0;

        Vector3 v = pos;
        v.z = static_cast<float>(depth());
        result.push_back({ v, color, { 0, 0 } });
        v.x += size.x;
        result.push_back({ v, color, { 1, 0 } });
        v.y += size.y;
        result.push_back({ v, color, { 1, 1 } });
        result.push_back({ v, color, { 1, 1 } });
        v.x -= size.x;
        result.push_back({ v, color, { 0, 1 } });
        v.y -= size.y;
        result.push_back({ v, color, { 0, 0 } });
        return { { result, { tex } } };
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
        return WidgetClass::SCENEWINDOW_CLASS;
    }

}
}
