#pragma once

#include "handler.h"
#include "../gui/widgetnames.h"
#include "Modules/uniquecomponent/uniquecomponentdefine.h"

namespace Engine
{
	namespace UI
	{
		class MADGINE_CLIENT_EXPORT GuiHandlerBase : public Handler
		{
		public:
			enum class WindowType
			{
				MODAL_OVERLAY,
				NONMODAL_OVERLAY,
				ROOT_WINDOW
			};

			GuiHandlerBase(UIManager &ui, WindowType type);


			virtual void open();
			virtual void close();
			bool isOpen() const;

			bool isRootWindow() const;

			Scene::ContextMask context() const;

			GuiHandlerBase &getSelf(bool = true);

		protected:

			void setContext(Scene::ContextMask context);


		private:

			const WindowType mType;


			Scene::ContextMask mContext;
		};
	}
}

DECLARE_UNIQUE_COMPONENT(Engine::UI, GuiHandler, GuiHandlerBase, UIManager &);

namespace Engine 
{
	namespace UI
	{

		template <typename T>
		using GuiHandler = GuiHandlerComponent<T>;

	}
}



