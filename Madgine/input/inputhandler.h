#pragma once

namespace Engine
{
	namespace Input
	{
		class MADGINE_CLIENT_EXPORT InputHandler
		{
		public:
			InputHandler();
			virtual ~InputHandler() = default;

			virtual void update() = 0;
			
			void setSystem(GUI::GUISystem *gui);

		protected:
			void injectKeyPress(const GUI::KeyEventArgs& arg);
			void injectKeyRelease(const GUI::KeyEventArgs& arg);
			void injectMousePress(const GUI::MouseEventArgs& arg);
			void injectMouseRelease(const GUI::MouseEventArgs& arg);
			void injectMouseMove(const GUI::MouseEventArgs& arg);

		private:
			GUI::GUISystem* mGUI;
		};
	}
}
