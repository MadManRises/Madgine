#pragma once

namespace Ogre {
	template <class T>
	class Singleton;
}

namespace Engine {
	class ValueType;

	enum class InvPtr : uintptr_t {};

	namespace App {
		class ConfigSet;
		struct AppSettings;
		enum class ContextMask : unsigned int;
		class Application;
	}
	namespace Serialize {

		class SerializeInStream;
		class SerializeOutStream;
		class SerializableUnit; 
		class TopLevelSerializableUnit;
		class Observable;
		class Serializable;
		class FileBuffer;
		struct MessageHeader;
		class SerializeManager;

		template <class T, class... Args>
		class ObservableMap;

		typedef size_t ParticipantId;

		enum TopLevelMadgineObject;
	}
	namespace Scripting {

		class ScriptingManager;

		class BaseAPI;
		class BaseGlobalAPIComponent;

		class Scope;

		class RefScopeTopLevelSerializableUnit;
		class RefScope;
		
		class Prototype;
		class List;
		class Vector;
		class Struct;

		class GlobalScope;
		class Scene;

		typedef std::vector<ValueType> ArgumentList;
		typedef std::map<std::string, ValueType> Stack;
		typedef Serialize::ObservableMap<ValueType> VarSet;



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
		class Label;

		namespace MouseButton {
			enum MouseButton : unsigned char;
		}

	}

	namespace Scene {

		class SceneManager;
		class BaseSceneComponent;
		class SceneListener;

		namespace Entity {
			class Entity;
			class BaseEntityComponent;
		}


	}

	namespace Math {
		class Bounds;
	}

	namespace Resources {
		class TextureListener;
		class TextureComponent;

		class ResourceLoader;
		namespace Shading {
			class ShaderFactoryBase;
		}
	}

	namespace Util {
		class Profiler;
		class ProcessStats;
		struct TraceBack;
		class BaseMadgineObject;
	}

	namespace Input {
		class InputHandler;
	}

	namespace Network {
		class NetworkManager;
		class NetworkStream;
		
	}

}



