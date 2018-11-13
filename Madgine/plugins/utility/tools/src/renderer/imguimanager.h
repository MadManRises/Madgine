#pragma once

#include "Madgine/input/inputlistener.h"

namespace Engine {
	namespace Tools {

		class ImGuiManager : public Engine::Input::InputListener {
		public:
			ImGuiManager(Engine::App::ClientApplication &app);
			virtual ~ImGuiManager();

			virtual void init() = 0;
			virtual void finalize() = 0;

			virtual void newFrame(float timeSinceLastFrame) = 0;


			void injectKeyPress(const Engine::Input::KeyEventArgs& arg) override;
			void injectKeyRelease(const Engine::Input::KeyEventArgs& arg) override;
			void injectMousePress(const Engine::Input::MouseEventArgs& arg) override;
			void injectMouseRelease(const Engine::Input::MouseEventArgs& arg) override;
			void injectMouseMove(const Engine::Input::MouseEventArgs& arg) override;

		protected:
			Engine::App::ClientApplication &mApp;

		private:

			Engine::Input::InputListener *mListener;
		};

	}
}