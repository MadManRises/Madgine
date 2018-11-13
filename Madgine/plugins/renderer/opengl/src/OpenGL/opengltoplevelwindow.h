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

			void setCustomRenderCall(void(*call)() = nullptr);

			bool update() override;

			// Geerbt über TopLevelWindow
			virtual void injectKeyPress(const Input::KeyEventArgs & arg) override;
			virtual void injectKeyRelease(const Input::KeyEventArgs & arg) override;
			virtual void injectMousePress(const Input::MouseEventArgs & arg) override;
			virtual void injectMouseRelease(const Input::MouseEventArgs & arg) override;
			virtual void injectMouseMove(const Input::MouseEventArgs & arg) override;
			virtual bool isCursorVisible() override;
			virtual void setCursorVisibility(bool v) override;
			virtual void setCursorPosition(const Vector2 & pos) override;
			virtual Vector2 getCursorPosition() override;
			virtual Vector3 getScreenSize() override;
			virtual bool singleFrame() override;
			virtual std::unique_ptr<Widget> createWidget(const std::string & name) override;
			virtual std::unique_ptr<Bar> createBar(const std::string & name) override;
			virtual std::unique_ptr<Button> createButton(const std::string & name) override;
			virtual std::unique_ptr<Checkbox> createCheckbox(const std::string & name) override;
			virtual std::unique_ptr<Combobox> createCombobox(const std::string & name) override;
			virtual std::unique_ptr<Image> createImage(const std::string & name) override;
			virtual std::unique_ptr<Label> createLabel(const std::string & name) override;
			virtual std::unique_ptr<SceneWindow> createSceneWindow(const std::string & name) override;
			virtual std::unique_ptr<TabWidget> createTabWidget(const std::string & name) override;
			virtual std::unique_ptr<Textbox> createTextbox(const std::string & name) override;

		private:
			void(*mCustomRenderCall)() = nullptr;

		};

	}
}