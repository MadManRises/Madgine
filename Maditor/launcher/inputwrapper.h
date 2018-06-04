#pragma once



#include "Madgine/Input\InputHandler.h"


namespace Maditor {
	namespace Launcher {
		
		class InputWrapper : public Engine::Input::InputHandler {

		public:
			InputWrapper(Shared::InputShared &shared);


			virtual void update() override;

		protected:
			static Engine::GUI::MouseButton::MouseButton convertButton(Shared::MouseButton button);

		private:

			Shared::InputShared &mShared;

		};

	}
}