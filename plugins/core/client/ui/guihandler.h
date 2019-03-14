#pragma once

#include "handler.h"
#include "../gui/widgetnames.h"
#include "Interfaces/uniquecomponent/uniquecomponentdefine.h"

#include "Interfaces/scripting/types/scope.h"

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

			GuiHandlerBase(UIManager &ui, const std::string& windowName, WindowType type);



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

DEFINE_UNIQUE_COMPONENT(Engine::UI, GuiHandlerBase, UIManager&, GuiHandler, MADGINE_CLIENT);

RegisterType(Engine::UI::GuiHandlerBase);

