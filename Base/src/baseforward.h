#pragma once

namespace Engine {

	class BaseMadgineObject;

	namespace Scene {
		class SceneManager;

		namespace Entity {
			class Entity;
		}
	}

	namespace App {
		enum class ContextMask : unsigned int;
		struct AppSettings;
		class Application;
	}

	namespace UI {
		class GuiHandlerBase;
		class GameHandlerBase;
	}

}