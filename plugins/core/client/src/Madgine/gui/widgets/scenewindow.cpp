#include "../../clientlib.h"

#include "scenewindow.h"

#include "widgetmanager.h"

#include "Modules/math/vector4.h"

#include "../vertex.h"

#include "../../render/camera.h"

#include "toplevelwindow.h"

#include "../../render/rendercontext.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

METATABLE_BEGIN(Engine::Widgets::SceneWindow)
METATABLE_END(Engine::Widgets::SceneWindow)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::SceneWindow, Engine::Widgets::WidgetBase)
SERIALIZETABLE_END(Engine::Widgets::SceneWindow)

namespace Engine {
namespace Widgets {

    SceneWindow::~SceneWindow()
    {
    }

    std::vector<std::pair<std::vector<GUI::Vertex>, Render::TextureDescriptor>> SceneWindow::vertices(const Vector3 &screenSize)
    {
        std::vector<GUI::Vertex> result;

        Vector3 pos = (getAbsolutePosition() * screenSize) / screenSize;
        Vector3 size = (getAbsoluteSize() * screenSize) / screenSize;

        Vector4 color { 1, 1, 1, 1 };

        const Render::Texture *tex = mTarget ? mTarget->texture() : nullptr;

        Vector3 v = pos;
        v.z = static_cast<float>(depth());
        result.push_back({ v, color, { 0, 1 } });
        v.x += size.x;
        result.push_back({ v, color, { 1, 1 } });
        v.y += size.y;
        result.push_back({ v, color, { 1, 0 } });
        result.push_back({ v, color, { 1, 0 } });
        v.x -= size.x;
        result.push_back({ v, color, { 0, 0 } });
        v.y -= size.y;
        result.push_back({ v, color, { 0, 1 } });
        return { { result, { tex } } };
    }

    Render::RenderTarget *SceneWindow::getRenderTarget()
    {
        return mTarget.get();
    }

    void SceneWindow::sizeChanged(const Vector3i &pixelSize)
    {
        if (!mTarget)
            mTarget = manager().window().getRenderer()->createRenderTexture(pixelSize.xy());
        else
            mTarget->resize(pixelSize.xy());
    }

    WidgetClass SceneWindow::getClass() const
    {
        return WidgetClass::SCENEWINDOW_CLASS;
    }

}
}
