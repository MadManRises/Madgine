#pragma once

namespace Engine {
	class ValueType;

	class InvScopePtr;

	namespace Serialize {

		class Stream;
		class SerializeInStream;
		class SerializeOutStream;
		class SerializableUnitBase;
		class BufferedInOutStream;
		class BufferedInStream;
		class BufferedOutStream;
		class TopLevelSerializableUnitBase;
		class Observable;
		class Serializable;
		class FileBuffer;
		struct MessageHeader;
		class SerializeManager;


		typedef size_t ParticipantId;

		class buffered_streambuf;

		using SerializableUnitMap = std::map<size_t, SerializableUnitBase*>;

		enum StreamError {
			NO_ERROR = 0,
			WOULD_BLOCK,
			ALREADY_CONNECTED,
			TIMEOUT,
			NO_SERVER,
			NO_CONNECTION,
			UNKNOWN_ERROR
		};

		enum MessageType {
			STATE,
			ACTION,
			REQUEST
		};

	}
	namespace Scripting {

		class ScriptingManager;

		class ScopeBase;
		class GlobalScopeBase;

		class KeyValueIterator;

		class LuaTable;

		class ArgumentList;
		typedef std::map<std::string, ValueType> Stack;

		typedef ValueType(*ApiMethod)(ScopeBase *, const ArgumentList &);

		namespace Parsing
		{
			class ScriptParser;
		}

	}


	namespace Network {
		class NetworkManager;
		class NetworkStream;

	}


	namespace Hierarchy {
		class any_ptr;
	}


	namespace Util {
		struct TraceBack;

		class LogListener;

		class Process;
		class ProcessListener;
		class StandardLog;
	}

	namespace SignalSlot {

		class ConnectionBase;
		template <class... _Ty>
		class ConnectionInstance;
		class ConnectionManager;
	}


}

struct lua_State;
struct luaL_Reg;

