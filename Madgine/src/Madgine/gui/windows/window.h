#pragma once

#include "../../math/matrix3.h"

#include "../guievents.h"

#include "windowclass.h"
#include "../../scripting/types/scope.h"
#include "../../signalslot/signal.h"

namespace Engine
{
	namespace GUI
	{
		class MADGINE_CLIENT_EXPORT Window : public Scripting::Scope<Window>
		{
		public:
			Window(const std::string& name, Window* parent);

			Window(const std::string& name, GUISystem& gui);

			Window(const Window &) = delete;

			virtual ~Window();

			void setSize(const Matrix3& size);
			const Matrix3& getSize();
			void setPos(const Matrix3& pos);
			const Matrix3& getPos() const;

			virtual Vector3 getAbsoluteSize() const = 0;
			virtual Vector2 getAbsolutePosition() const = 0;

			void updateGeometry(const Vector3 &parentSize, const Vector2 &parentPos);

			virtual Class getClass() = 0;

			void destroy();

			virtual void releaseInput() = 0;
			virtual void captureInput() = 0;
			virtual void activate() = 0;
			virtual void moveToFront() = 0;


			virtual bool isVisible() const = 0;
			virtual void showModal() = 0;
			virtual void hideModal() = 0;
			void show();
			void hide();
			virtual void setVisible(bool b) = 0;

			virtual void setEnabled(bool b) = 0;

			const std::string &getName() const;

			const char * key() const override;

			Window* createChild(const std::string& name, Class _class);
			Window *createChildWindow(const std::string &name);
			Bar *createChildBar(const std::string& name);
			Button *createChildButton(const std::string& name);
			Checkbox *createChildCheckbox(const std::string& name);
			Combobox *createChildCombobox(const std::string& name);
			Label *createChildLabel(const std::string& name);
			SceneWindow *createChildSceneWindow(const std::string& name);
			TabWindow *createChildTabWindow(const std::string& name);
			Textbox *createChildTextbox(const std::string& name);
			Image *createChildImage(const std::string& name);
			
			Window* getChildRecursive(const std::string& name);

			SignalSlot::SignalStub<MouseEventArgs&> &mouseMoveEvent();
			SignalSlot::SignalStub<MouseEventArgs&> &mouseDownEvent();
			SignalSlot::SignalStub<MouseEventArgs&> &mouseUpEvent();

		protected:

			std::unique_ptr<Window> createWindowClass(const std::string& name, Class _class);

			virtual std::unique_ptr<Window> createWindow(const std::string &name) = 0;
			virtual std::unique_ptr<Bar> createBar(const std::string& name) = 0;
			virtual std::unique_ptr<Button> createButton(const std::string& name) = 0;
			virtual std::unique_ptr<Checkbox> createCheckbox(const std::string& name) = 0;
			virtual std::unique_ptr<Combobox> createCombobox(const std::string& name) = 0;
			virtual std::unique_ptr<Image> createImage(const std::string& name) = 0;
			virtual std::unique_ptr<Label> createLabel(const std::string& name) = 0;
			virtual std::unique_ptr<SceneWindow> createSceneWindow(const std::string& name) = 0;
			virtual std::unique_ptr<TabWindow> createTabWindow(const std::string& name) = 0;
			virtual std::unique_ptr<Textbox> createTextbox(const std::string& name) = 0;

		protected:
			virtual void setAbsoluteSize(const Vector3 &size) = 0;
			virtual void setAbsolutePosition(const Vector2 &pos) = 0;

			GUISystem &gui();

			void destroyChild(Window *w);

			KeyValueMapList maps() override;

			SignalSlot::Signal<MouseEventArgs&> mMouseMoveSignal, mMouseDownSignal, mMouseUpSignal;

		private:
			std::string mName;
			Window *mParent;

			GUISystem &mGUI;

			std::vector<std::unique_ptr<Window>> mChildren;

			
			Matrix3 mPos, mSize;

			

		};
	}
}
