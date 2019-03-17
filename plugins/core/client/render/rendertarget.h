#pragma once

#include "Interfaces/math/vector2.h"

namespace Engine {
	namespace Render {

		struct MADGINE_CLIENT_EXPORT RenderTarget {
			RenderTarget(RenderWindow *window, Scene::Camera *camera, const Vector2 &size);
			virtual ~RenderTarget();

			Scene::Camera *camera() const;

			virtual void render() = 0;

			virtual uint32_t textureId() const = 0;

			virtual void resize(const Vector2 &size);

			const Vector2 &getSize();

		private:
			RenderWindow *mWindow;
			Scene::Camera *mCamera;
			Vector2 mSize;
		};

	}
}