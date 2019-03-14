#include "../clientlib.h"

#include "rendertarget.h"
#include "renderwindow.h"

namespace Engine {
	namespace Render {

		RenderTarget::RenderTarget(RenderWindow *window, Camera * camera, const Vector2 & size) :
			mWindow(window),
			mCamera(camera),
			mSize(size)
		{
			mWindow->addRenderTarget(this);
		}

		RenderTarget::~RenderTarget()
		{
			mWindow->removeRenderTarget(this);
		}

		Camera * RenderTarget::camera() const
		{
			return mCamera;
		}

		void RenderTarget::resize(const Vector2 & size)
		{
			mSize = size;
		}

		const Vector2 & RenderTarget::getSize()
		{
			return mSize;
		}

	}
}