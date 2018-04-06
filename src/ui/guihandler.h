#pragma once

#include "handler.h"
#include "../gui/windownames.h"
#include "../ogreuniquecomponentcollector.h"
#include "../uniquecomponent.h"

#include "../scripting/types/scope.h"

namespace Engine
{
	namespace UI
	{
		class OGREMADGINE_EXPORT GuiHandlerBase : public Handler
		{
		public:
			enum class WindowType
			{
				MODAL_OVERLAY,
				NONMODAL_OVERLAY,
				ROOT_WINDOW
			};

			GuiHandlerBase(const std::string& windowName, WindowType type, const std::string& layoutFile = "",
			               const std::string& parentName = WindowNames::rootWindow);


			bool init(int order);
			bool init() override;
			void finalize(int order);
			void finalize() override;

			virtual void open();
			virtual void close();
			bool isOpen() const;

			bool isRootWindow() const;

			Scene::ContextMask context() const;

		protected:
			void setInitialisationOrder(int order);
			void setContext(Scene::ContextMask context);


		private:


			//std::map<CEGUI::Window *, std::string> mTranslationKeys;

			const std::string mLayoutFile;
			const std::string mParentName;
			const WindowType mType;


			int mOrder;
			Scene::ContextMask mContext;
		};

		using GuiHandlerCollector = OgreUniqueComponentCollector<GuiHandlerBase>;
		template <class T>
		using GuiHandler = Scripting::Scope<T, UniqueComponent<T, GuiHandlerCollector>>;
	} // namespace GuiHandler
} // namespace Cegui
