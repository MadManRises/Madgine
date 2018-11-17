#pragma once

#include "Madgine/gui/widgets/toplevelwindow.h"
#include "Interfaces/math/vector2.h"

namespace Engine
{
	namespace GUI
	{
		
		class MyGUITopLevelWindow : public TopLevelWindow
		{
		public:
			MyGUITopLevelWindow(MyGUISystem &system);
			~MyGUITopLevelWindow();

			bool handleKeyPress(const Input::KeyEventArgs& arg) override;
			bool handleKeyRelease(const Input::KeyEventArgs& arg) override;
			bool handleMousePress(const Input::MouseEventArgs& arg) override;
			bool handleMouseRelease(const Input::MouseEventArgs& arg) override;
			bool handleMouseMove(const Input::MouseEventArgs& arg) override;

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

		protected:
			void onResize(size_t width, size_t height) override;

		private:
			Ogre::RenderWindow *mWindow;

			float mScrollWheel;

			Vector2 mMousePosition;
			Vector2 mMoveDelta;

			Ogre::Camera *mCamera;
			Ogre::Viewport *mViewport;


			std::unique_ptr<MyGUI::OgrePlatform> mPlatform;
			std::unique_ptr<MyGUI::Gui> mGUI;
			MyGUI::InputManager* mInputManager;
			MyGUI::PointerManager *mPointerManager;
			MyGUI::OgreRenderManager *mRenderManager;
			MyGUI::LayoutManager* mLayoutManager;
			MyGUI::ResourceManager* mResourceManager;
			MyGUI::Widget *mRootWindow;

		};

	}
}