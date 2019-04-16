#pragma once

#include "Interfaces/reflection/classname.h"
#include "Madgine/core/madgineobject.h"
#include "Interfaces/scripting/types/scope.h"

namespace Engine {
	namespace Render {

		class MADGINE_CLIENT_EXPORT RendererBase : public Core::MadgineObject, public Scripting::Scope<RendererBase> {
		public:			
			RendererBase(GUI::GUISystem *gui);
			RendererBase(const RendererBase&) = delete;
			virtual ~RendererBase();

			void operator=(const RendererBase&) = delete;

			virtual std::unique_ptr<RenderWindow> createWindow(GUI::TopLevelWindow *w) = 0;

			virtual App::Application &app(bool = true) override;
			virtual const Core::MadgineObject *parent() const override;

		private:
			GUI::GUISystem *mGui;

		};

	}
}

RegisterType(Engine::Render::RendererBase);