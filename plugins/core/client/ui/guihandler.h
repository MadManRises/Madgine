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

			bool init() override;
			void finalize() override;

			void setContext(Scene::ContextMask context);


		private:

			const WindowType mType;


			Scene::ContextMask mContext;
		};
		/*
		using GuiHandlerCollector = UniqueComponentCollector<GuiHandlerBase, UIManager &>;
		template <class T>
		using GuiHandler = Scripting::Scope<T, UniqueComponent<T, GuiHandlerCollector>>;


		using GuiHandlerContainer = UniqueComponentContainer<GuiHandlerBase, UIManager &>;*/
	} // namespace GuiHandler
/*
#ifndef STATIC_BUILD
	MADGINE_CLIENT_TEMPLATE_INSTANTIATION struct UniqueComponentRegistry<UI::GuiHandlerBase, UI::UIManager &>;
#endif
*/
} // namespace Cegui

DEFINE_UNIQUE_COMPONENT(Engine::UI, GuiHandlerBase, UIManager&, GuiHandler, MADGINE_CLIENT);

RegisterType(Engine::UI::GuiHandlerBase);
//RegisterCollector(Engine::UI::GuiHandlerCollector);