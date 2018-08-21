#pragma once

#include "Madgine/gui/widgets/toplevelwindow.h"

namespace Engine
{
	namespace GUI
	{
		
		class MyGUITopLevelWindow : public TopLevelWindow, public Ogre::WindowEventListener
		{
		public:
			MyGUITopLevelWindow(MyGUISystem &system);
			~MyGUITopLevelWindow();

			void injectKeyPress(const Input::KeyEventArgs& arg) override;
			void injectKeyRelease(const Input::KeyEventArgs& arg) override;
			void injectMousePress(const Input::MouseEventArgs& arg) override;
			void injectMouseRelease(const Input::MouseEventArgs& arg) override;
			void injectMouseMove(const Input::MouseEventArgs& arg) override;

			Vector2 relativeMoveDelta(MyGUI::Widget* w) const;
			Vector2 widgetRelative(MyGUI::Widget* w, int left = -1, int top = -1) const;

			bool isCursorVisible() override;
			void setCursorVisibility(bool v) override;
			void setCursorPosition(const Vector2& pos) override;
			Vector2 getCursorPosition() override;
			Vector3 getScreenSize() override;

			std::unique_ptr<Widget> createWidget(const std::string &name) override;
			std::unique_ptr<Bar> createBar(const std::string &name) override;
			std::unique_ptr<Button> createButton(const std::string &name) override;
			std::unique_ptr<Checkbox> createCheckbox(const std::string &name) override;
			std::unique_ptr<Combobox> createCombobox(const std::string &name) override;
			std::unique_ptr<Label> createLabel(const std::string &name) override;
			std::unique_ptr<SceneWindow> createSceneWindow(const std::string &name) override;
			std::unique_ptr<TabWidget> createTabWidget(const std::string &name) override;
			std::unique_ptr<Textbox> createTextbox(const std::string &name) override;
			std::unique_ptr<Image> createImage(const std::string &name) override;

			MyGUI::Widget *rootWidget();

			bool singleFrame();

		private:
			Ogre::RenderWindow *mWindow;
			void *mHwnd;

			std::unique_ptr<Input::InputHandler> mInputHolder;

			float mScrollWheel;

			Vector2 mMousePosition;
			Vector2 mMoveDelta;

			Ogre::Camera *mCamera;
			Ogre::Viewport *mViewport;




			MyGUI::OgrePlatform *mPlatform;
			MyGUI::Gui *mGUI;
			MyGUI::InputManager* mInputManager;
			MyGUI::PointerManager *mPointerManager;
			MyGUI::OgreRenderManager *mRenderManager;
			MyGUI::LayoutManager* mLayoutManager;
			MyGUI::ResourceManager* mResourceManager;
			MyGUI::Widget *mRootWindow;

		};

	}
}