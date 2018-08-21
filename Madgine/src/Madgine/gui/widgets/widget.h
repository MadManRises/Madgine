#pragma once

#include "../../math/matrix3.h"

#include "../../input/inputevents.h"

#include "widgetclass.h"
#include "../../scripting/types/scope.h"
#include "../../signalslot/signal.h"

namespace Engine
{
	namespace GUI
	{
		class MADGINE_CLIENT_EXPORT Widget : public Scripting::Scope<Widget>
		{
		public:
			Widget(const std::string& name, Widget* parent);

			Widget(const std::string& name, TopLevelWindow& window);

			Widget(const Widget &) = delete;

			virtual ~Widget();

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

			Widget* createChild(const std::string& name, Class _class);
			Widget *createChildWidget(const std::string &name);
			Bar *createChildBar(const std::string& name);
			Button *createChildButton(const std::string& name);
			Checkbox *createChildCheckbox(const std::string& name);
			Combobox *createChildCombobox(const std::string& name);
			Label *createChildLabel(const std::string& name);
			SceneWindow *createChildSceneWindow(const std::string& name);
			TabWidget *createChildTabWidget(const std::string& name);
			Textbox *createChildTextbox(const std::string& name);
			Image *createChildImage(const std::string& name);
			
			Widget* getChildRecursive(const std::string& name);

			SignalSlot::SignalStub<Input::MouseEventArgs&> &mouseMoveEvent();
			SignalSlot::SignalStub<Input::MouseEventArgs&> &mouseDownEvent();
			SignalSlot::SignalStub<Input::MouseEventArgs&> &mouseUpEvent();

		protected:

			std::unique_ptr<Widget> createWidgetClass(const std::string& name, Class _class);

			virtual std::unique_ptr<Widget> createWidget(const std::string &name) = 0;
			virtual std::unique_ptr<Bar> createBar(const std::string& name) = 0;
			virtual std::unique_ptr<Button> createButton(const std::string& name) = 0;
			virtual std::unique_ptr<Checkbox> createCheckbox(const std::string& name) = 0;
			virtual std::unique_ptr<Combobox> createCombobox(const std::string& name) = 0;
			virtual std::unique_ptr<Image> createImage(const std::string& name) = 0;
			virtual std::unique_ptr<Label> createLabel(const std::string& name) = 0;
			virtual std::unique_ptr<SceneWindow> createSceneWindow(const std::string& name) = 0;
			virtual std::unique_ptr<TabWidget> createTabWidget(const std::string& name) = 0;
			virtual std::unique_ptr<Textbox> createTextbox(const std::string& name) = 0;

		protected:
			virtual void setAbsoluteSize(const Vector3 &size) = 0;
			virtual void setAbsolutePosition(const Vector2 &pos) = 0;

			GUISystem &gui();
			TopLevelWindow &window();

			void destroyChild(Widget *w);

			KeyValueMapList maps() override;

			SignalSlot::Signal<Input::MouseEventArgs&> mMouseMoveSignal, mMouseDownSignal, mMouseUpSignal;

		private:
			std::string mName;
			Widget *mParent;

			TopLevelWindow &mWindow;

			std::vector<std::unique_ptr<Widget>> mChildren;

			
			Matrix3 mPos, mSize;

			

		};
	}
}
