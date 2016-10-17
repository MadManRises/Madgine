#pragma once

namespace Engine {
	namespace App {
		class ConfigSet;
		struct AppSettings;
		enum class ContextMask : size_t;
		class Application;
	}
	namespace Scripting {

		enum class ScopeClass;
		class BaseAPI;
		class BaseGlobalAPIComponent;

		class Scope;
		class ValueType;
		class VarSet;
		class List;
		class Struct;

		class GlobalScope;
		class Level;

		namespace Serialize {

			class SerializeInStream;
			class SerializeOutStream;
			class Serializable;

		}

		namespace Parsing {
			class MethodNode;
			class MethodNodePtr;
			class EntityNode;
			class EntityNodePtr;
			class ScriptParser;
		}

		namespace Statements {
			class Statement;
		}


	}

	namespace UI {

		class Handler;
		class GameHandlerBase;
		class GuiHandlerBase;
		class UIManager;

		class Process;
		class ProcessListener;

	}

	namespace GUI {

		class GUISystem;
		struct WindowSizeRelVector;
		class WindowContainer;
		class Window;
		class Bar;
		class Button;
		class TextureDrawer;
		class Checkbox;
		class Combobox;
		class TabWindow;
		class Textbox;
		class Layout;
		class Label;

		namespace MouseButton {
			enum MouseButton;
		}

	}

	namespace OGRE {

		class SceneManager;
		class BaseSceneComponent;
		class SceneListener;
		class TextureListener;
		class TextureComponent;

		namespace Entity {
			class Entity;
			class BaseEntityComponent;
		}


	}

	namespace Math {
		class Bounds;
	}

	namespace Resources {
		class ResourceLoader;
		namespace Shading {
			class ShaderFactoryBase;
		}
	}

	namespace Util {
		class Profiler;
		class ProcessStats;
	}

	namespace Input {
		class InputHandler;
	}

}



