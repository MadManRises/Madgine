#pragma once

namespace Engine {

	class MadgineObject;

	namespace Scene {
		class SceneManagerBase;
		enum class ContextMask : unsigned int;

		namespace Entity {
			class Entity;
			class Transform;
		}

		class Light;

	}

	namespace App {
		
		struct AppSettings;
		class Application;
		class FrameListener;
	}

	namespace UI {
		class GuiHandlerBase;
		class GameHandlerBase;
	}

	namespace Scripting {
		class GlobalScope;
		class GlobalAPIComponentBase;
	}
}