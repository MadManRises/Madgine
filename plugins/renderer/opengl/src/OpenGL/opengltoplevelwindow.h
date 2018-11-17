#pragma once

#include "Madgine/gui/widgets/toplevelwindow.h"

#include "Interfaces/math/vector2.h"
#include "Interfaces/math/vector3.h"

namespace Engine {
	namespace GUI {

		class MADGINE_OPENGL_EXPORT OpenGLTopLevelWindow : public TopLevelWindow
		{
		public:
			OpenGLTopLevelWindow(OpenGLSystem &system);

			bool update() override;

			// Geerbt über TopLevelWindow
			bool handleKeyPress(const Input::KeyEventArgs & arg) override;
			bool handleKeyRelease(const Input::KeyEventArgs & arg) override;
			bool handleMousePress(const Input::MouseEventArgs & arg) override;
			bool handleMouseRelease(const Input::MouseEventArgs & arg) override;
			bool handleMouseMove(const Input::MouseEventArgs & arg) override;
			bool isCursorVisible() override;
			void setCursorVisibility(bool v) override;
			void setCursorPosition(const Vector2 & pos) override;
			Vector2 getCursorPosition() override;
			Vector3 getScreenSize() override;
			bool singleFrame() override;
			std::unique_ptr<Widget> createWidget(const std::string & name) override;
			std::unique_ptr<Bar> createBar(const std::string & name) override;
			std::unique_ptr<Button> createButton(const std::string & name) override;
			std::unique_ptr<Checkbox> createCheckbox(const std::string & name) override;
			std::unique_ptr<Combobox> createCombobox(const std::string & name) override;
			std::unique_ptr<Image> createImage(const std::string & name) override;
			std::unique_ptr<Label> createLabel(const std::string & name) override;
			std::unique_ptr<SceneWindow> createSceneWindow(const std::string & name) override;
			std::unique_ptr<TabWidget> createTabWidget(const std::string & name) override;
			std::unique_ptr<Textbox> createTextbox(const std::string & name) override;

		};

	}
}