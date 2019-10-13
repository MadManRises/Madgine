#include "../../clientlib.h"

#include "scenewindow.h"

#include "Modules/math/vector4.h"

#include "../vertex.h"

#include "Madgine/scene/camera.h"

#include "toplevelwindow.h"

#include "../../render/renderwindow.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"


METATABLE_BEGIN(Engine::GUI::SceneWindow)
PROPERTY(Camera, camera, setCamera)
METATABLE_END(Engine::GUI::SceneWindow)

SERIALIZETABLE_INHERIT_BEGIN(Engine::GUI::SceneWindow, Engine::GUI::WidgetBase)
SERIALIZETABLE_END(Engine::GUI::SceneWindow)

namespace Engine {
namespace GUI {
    SceneWindow::~SceneWindow()
    {
    }

    void SceneWindow::setCamera(Scene::Camera *camera)
    {
        if (camera) {
            const Engine::Vector3 screenSize = window().getScreenSize();
            mTarget = window().getRenderer()->createRenderTarget(camera, (getAbsoluteSize() * screenSize).xy());
        } else {
            mTarget.reset();
		}
    }

    Scene::Camera *SceneWindow::camera()
    {
        return mTarget ? mTarget->camera() : nullptr;
    }

    std::vector<std::pair<std::vector<Vertex>, Render::TextureDescriptor>> SceneWindow::vertices(const Vector3 &screenSize)
    {
        std::vector<Vertex> result;

        Vector3 pos = (getAbsolutePosition() * screenSize) / screenSize;
        Vector3 size = (getAbsoluteSize() * screenSize) / screenSize;

        Vector4 color { 1, 1, 1, 1 };

        uint32_t texId = mTarget ? mTarget->textureId() : 0;

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
        return { { result, { texId } } };
    }

    Render::RenderTarget *SceneWindow::getRenderTarget()
    {
        return mTarget.get();
    }

    void SceneWindow::sizeChanged(const Vector3 &pixelSize)
    {
        if (mTarget) {
            mTarget->resize(pixelSize.xy());
        }
    }

	    WidgetClass SceneWindow::getClass() const
    {
        return WidgetClass::SCENEWINDOW_CLASS;
    }

}
}
