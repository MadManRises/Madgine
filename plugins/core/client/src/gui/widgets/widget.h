#pragma once

#include "Interfaces/math/matrix3.h"

#include "../../input/inputevents.h"

#include "widgetclass.h"
#include "Interfaces/scripting/types/scope.h"
#include "Interfaces/signalslot/signal.h"

#include "Interfaces/generic/transformIt.h"

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

			Matrix3 getAbsoluteSize() const;
			Matrix3 getAbsolutePosition() const;

			void updateGeometry(const Vector3 &screenSize, const Matrix3 &parentSize, const Matrix3 &parentPos = Matrix3::ZERO);
			void screenSizeChanged(const Vector3 &screenSize);

			virtual Class getClass();

			void destroy();

			virtual void releaseInput();
			virtual void captureInput();
			virtual void activate();
			//virtual void moveToFront();


			virtual bool isVisible() const;
			virtual void showModal();
			virtual void hideModal();
			void show();
			void hide();
			virtual void setVisible(bool b);

			virtual void setEnabled(bool b);

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
			
			Widget *getChildRecursive(const std::string& name);
			Widget *getParent() const;

			virtual bool injectPointerPress(const Input::PointerEventArgs& arg);
			virtual bool injectPointerRelease(const Input::PointerEventArgs& arg);
			virtual bool injectPointerMove(const Input::PointerEventArgs& arg);
			virtual bool injectPointerEnter(const Input::PointerEventArgs& arg);
			virtual bool injectPointerLeave(const Input::PointerEventArgs& arg);

			SignalSlot::SignalStub<const Input::PointerEventArgs&> &pointerMoveEvent();
			SignalSlot::SignalStub<const Input::PointerEventArgs&> &pointerDownEvent();
			SignalSlot::SignalStub<const Input::PointerEventArgs&> &pointerUpEvent();
			SignalSlot::SignalStub<const Input::PointerEventArgs&> &pointerEnterEvent();
			SignalSlot::SignalStub<const Input::PointerEventArgs&> &pointerLeaveEvent();

			decltype(auto) children() const 
			{
				return uniquePtrToPtr(mChildren);
			}

			bool containsPoint(const Vector2 &point, const Vector3 &screenSize) const;

			virtual std::vector<Vertex> vertices(const Vector3 &screenSize);

		protected:

			std::unique_ptr<Widget> createWidgetClass(const std::string& name, Class _class);

			virtual std::unique_ptr<Widget> createWidget(const std::string &name);
			virtual std::unique_ptr<Bar> createBar(const std::string& name);
			virtual std::unique_ptr<Button> createButton(const std::string& name);
			virtual std::unique_ptr<Checkbox> createCheckbox(const std::string& name);
			virtual std::unique_ptr<Combobox> createCombobox(const std::string& name);
			virtual std::unique_ptr<Image> createImage(const std::string& name);
			virtual std::unique_ptr<Label> createLabel(const std::string& name);
			virtual std::unique_ptr<SceneWindow> createSceneWindow(const std::string& name);
			virtual std::unique_ptr<TabWidget> createTabWidget(const std::string& name);
			virtual std::unique_ptr<Textbox> createTextbox(const std::string& name);

			virtual void sizeChanged(const Vector3 &pixelSize);

		protected:

			size_t depth();

			GUISystem &gui();
			TopLevelWindow &window();

			void destroyChild(Widget *w);

			KeyValueMapList maps() override;

			SignalSlot::Signal<const Input::PointerEventArgs&> mPointerMoveSignal, mPointerDownSignal, mPointerUpSignal, mPointerEnterSignal, mPointerLeaveSignal;

		private:
			std::string mName;
			Widget *mParent;

			TopLevelWindow &mWindow;

			std::vector<std::unique_ptr<Widget>> mChildren;

			
			Matrix3 mPos = Matrix3::ZERO;
			Matrix3 mSize = Matrix3::IDENTITY;

			Matrix3 mAbsolutePos, mAbsoluteSize;

			bool mVisible = true;

		};
	}
}

RegisterType(Engine::GUI::Widget);