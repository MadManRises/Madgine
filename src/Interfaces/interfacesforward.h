#pragma once

namespace Engine
{
	class ValueType;

	class InvScopePtr;

	enum KeyValueValueFlags : uint8_t;

	class KeyValueIterator;
	class KeyValueMapList;


	struct IndexHolder;

	struct UniqueComponentCollectorManager;
	struct ComponentRegistryBase;

	struct InStream;
	struct OutStream;

	namespace Debug {

		struct TraceBack;
		struct StackTraceIterator;

		namespace Memory {
			struct StatsMemoryResource;
			struct MemoryTracker;
		}

		namespace Profiler {
			class Profiler;
		}

	}

	namespace Ini {
		struct IniFile;
	}

	namespace Serialize
	{
		struct SerializeInStream;
		struct SerializeOutStream;
		class SerializableUnitBase;
		struct BufferedInOutStream;
		struct BufferedInStream;
		struct BufferedOutStream;
		class TopLevelSerializableUnitBase;
		class Observable;
		class Serializable;
		class FileBuffer;
		struct MessageHeader;
		struct SerializeManager;
		struct SerializeStreambuf;

		class EOLType;

		struct CompareStreamId;

		typedef size_t ParticipantId;

		struct buffered_streambuf;

		struct noparent_deleter;

		using SerializableUnitMap = std::map<size_t, SerializableUnitBase*>;

		enum StreamError
		{
			NO_ERROR = 0,
			WOULD_BLOCK,
			ALREADY_CONNECTED,
			TIMEOUT,
			NO_SERVER,
			NO_CONNECTION,
			CONNECTION_REFUSED,
			UNKNOWN_ERROR
		};

		enum MessageType
		{
			STATE,
			ACTION,
			REQUEST
		};

		enum Command
		{
			INITIAL_STATE_DONE,
			STREAM_EOF
		};
	}

	namespace Scripting
	{
		class ScriptingManager;

		class ScopeBase;
		class GlobalScopeBase;

		class LuaState;
		class LuaTable;
		struct LuaThread;
		class LuaTableIterator;

		struct Mapper;

		class ArgumentList;
		typedef std::map<std::string, ValueType> Stack;

		typedef ValueType (*ApiMethod)(ScopeBase*, const ArgumentList&);

		namespace Parsing
		{
			class ScriptParser;
		}
	}


	namespace Network
	{
		class NetworkManager;
	}

	namespace Plugins
	{
		class PluginManager;
		class Plugin;
		class PluginSection;
		struct PluginListener;

		struct BinaryInfo;
	}


	namespace Util
	{
		
		class Log;		

		class LogListener;

		class Process;
		class StandardLog;

		
	}

	namespace SignalSlot
	{
		class ConnectionBase;
		template <class T, class... _Ty>
		class ConnectionInstance;
		struct TaskQueue;
	}

	namespace Window {
		struct WindowEventListener;
		struct Window;
	}

	namespace Filesystem {
		struct Path;
		struct FileQuery;
		struct FileQueryState;

		struct SharedLibraryQuery;
		struct SharedLibraryQueryState;		
	}

	namespace Threading {
		struct WorkGroup;
		struct WorkGroupHandle;
		struct Scheduler;

		class FrameListener;
		struct FrameLoop;

		struct DataMutex;
	}

	class Vector2;
	class Vector3;

	class Matrix3;
	class Matrix4;

	struct TypeInfo;

}

struct lua_State;
struct luaL_Reg;
