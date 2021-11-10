#pragma once

namespace Engine {

struct IndexHolder;

struct UniqueComponentCollectorManager;
template <typename T, typename _Collector, typename _Base>
struct UniqueComponent;
template <typename Base>
struct NamedUniqueComponent;
template <typename Observer, typename Registry, typename _Base, typename... _Ty>
struct UniqueComponentSelector;
template <typename C, typename Registry, typename __dont_remove_Base, typename... _Ty>
struct UniqueComponentContainer;
template <typename T, typename _Collector, typename Base>
struct VirtualUniqueComponentBase;
template <typename T, typename Base, typename _VBase = Base>
struct VirtualUniqueComponentImpl;
template <typename _Base, typename... _Ty>
struct UniqueComponentRegistry;
template <typename _Base, typename... _Ty>
struct NamedUniqueComponentRegistry;
template <typename Registry, typename __Base, typename... _Ty>
struct UniqueComponentCollector;



struct UniqueComponentRegistryBase;


namespace Debug {

    struct TraceBack;
    struct StackTraceIterator;

    namespace Memory {
        struct StatsMemoryResource;
        struct MemoryTracker;
    }

    namespace Profiler {
        struct Profiler;
        struct ProfilerThread;
    }
}


namespace Plugins {
    struct PluginManager;
    struct Plugin;
    struct PluginSection;
    struct PluginListener;

    struct BinaryInfo;
}

namespace Threading {
    struct ConnectionBase;
    template <typename T, typename... _Ty>
    struct ConnectionInstance;
    struct TaskQueue;    
    struct TaskHandle;
    template <typename T, typename Immediate = std::false_type>
    struct Task;
    struct Barrier;
    struct TaskPromiseTypeBase;
}

namespace Threading {
    struct WorkGroup;
    struct WorkGroupHandle;
    struct Scheduler;

    struct FrameListener;
    struct FrameLoop;

    struct DataMutex;
}

}
