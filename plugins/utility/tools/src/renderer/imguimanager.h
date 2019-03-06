#pragma once

#include "client/gui/windowoverlay.h"

namespace Engine {
	namespace Tools {

		class ImGuiManager : public Engine::GUI::WindowOverlay {
		public:
			ImGuiManager(Engine::App::Application &app);
			virtual ~ImGuiManager();

			virtual void init() = 0;
			virtual void finalize() = 0;

			virtual void newFrame(float timeSinceLastFrame) = 0;
			virtual void render() override;


			bool injectKeyPress(const Engine::Input::KeyEventArgs& arg) override;
			bool injectKeyRelease(const Engine::Input::KeyEventArgs& arg) override;
			bool injectMousePress(const Engine::Input::MouseEventArgs& arg) override;
			bool injectMouseRelease(const Engine::Input::MouseEventArgs& arg) override;
			bool injectMouseMove(const Engine::Input::MouseEventArgs& arg) override;

			void calculateAvailableScreenSpace(Vector3 &pos, Vector3 &size) override;

			void setMenuHeight(float h);

		protected:
			Engine::App::Application &mApp;

			float mMenuHeight = 0.0f;

		};

	}
}