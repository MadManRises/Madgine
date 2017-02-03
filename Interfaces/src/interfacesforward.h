#pragma once

namespace Engine {
	class ValueType;

	enum class InvPtr : uintptr_t {};

	namespace Serialize {

		class SerializeInStream;
		class SerializeOutStream;
		class SerializableUnit;
		class BufferedInOutStream;
		class BufferedInStream;
		class BufferedOutStream;
		class TopLevelSerializableUnit;
		class Observable;
		class Serializable;
		class FileBuffer;
		struct MessageHeader;
		class SerializeManager;


		typedef size_t ParticipantId;

		class buffered_streambuf;

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
		//typedef Serialize::ObservableMap<ValueType> VarSet;



		namespace Parsing {
			class MethodNode;
			//typedef Ogre::SharedPtr<MethodNode> MethodNodePtr;
			class EntityNode;
			//typedef Ogre::SharedPtr<EntityNode> EntityNodePtr;
			class ScriptParser;
		}

		namespace Statements {
			class Statement;
		}

	}

	namespace Util {
		struct TraceBack;

		class Process;
		class ProcessListener;
	}

	namespace Network {
		class NetworkManager;
		class NetworkStream;

	}

}



