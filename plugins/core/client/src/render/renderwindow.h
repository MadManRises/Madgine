#pragma once

#include "Modules/threading/framelistener.h"

namespace Engine {
	namespace Render {

		class MADGINE_CLIENT_EXPORT RenderWindow : public Threading::FrameListener {
		public:
			RenderWindow(GUI::TopLevelWindow *w);
			virtual ~RenderWindow();

			virtual void render() = 0;

			virtual bool frameStarted(std::chrono::microseconds) override;
			virtual bool frameRenderingQueued(std::chrono::microseconds, Scene::ContextMask) override;
			virtual bool frameEnded(std::chrono::microseconds) override;

			virtual std::unique_ptr<RenderTarget> createRenderTarget(Scene::Camera *camera, const Vector2 &size) = 0;

			void addRenderTarget(RenderTarget *target);
			void removeRenderTarget(RenderTarget *target);

		protected:
			virtual void renderOverlays();

			void updateRenderTargets();

			GUI::TopLevelWindow *window();

		private:
			GUI::TopLevelWindow *mWindow;
			std::vector<RenderTarget *> mRenderTargets;
		};

	}
}