#include "../clientlib.h"

#include "rendertarget.h"
#include "renderwindow.h"

namespace Engine {
namespace Render {

    RenderTarget::RenderTarget(RenderWindow *window, Scene::Camera *camera, const Vector2 &size)
        : mWindow(window)
        , mCamera(camera)
        , mSize(size)
    {
        mWindow->addRenderTarget(this);
    }

    RenderTarget::~RenderTarget()
    {
        mWindow->removeRenderTarget(this);
    }

    Scene::Camera *RenderTarget::camera() const
    {
        return mCamera;
    }

    void RenderTarget::resize(const Vector2 &size)
    {
        mSize = size;
    }

    const Vector2 &RenderTarget::getSize()
    {
        return mSize;
    }

    void RenderTarget::addPreRenderPass(std::unique_ptr<RenderPass> &&pass)
    {
        mPreRenderPasses.emplace_back(std::move(pass));
    }

    void RenderTarget::addPostRenderPass(std::unique_ptr<RenderPass> &&pass)
    {
        mPostRenderPasses.emplace_back(std::move(pass));
    }

    const std::vector<std::unique_ptr<RenderPass>> &RenderTarget::preRenderPasses()
    {
        return mPreRenderPasses;
    }

    const std::vector<std::unique_ptr<RenderPass>> &RenderTarget::postRenderPasses()
    {
        return mPostRenderPasses;
    }

}
}