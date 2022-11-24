#pragma once

namespace Engine {

namespace UniqueComponent {
    struct IndexHolder;

    struct CollectorManager;
    template <typename T, typename _Collector, typename _Base>
    struct Component;
    template <typename Base>
    struct NamedComponent;
    template <typename Registry, typename _Base, typename... _Ty>
    struct Selector;
    template <typename C, typename Registry, typename __dont_remove_Base, typename... _Ty>
    struct Container;
    template <typename T, typename _Collector, typename Base>
    struct VirtualComponentBase;
    template <typename T, typename Base, typename _VBase = Base>
    struct VirtualComponentImpl;
    template <typename _Base, typename... _Ty>
    struct Registry;
    template <typename _Base, typename... _Ty>
    struct NamedRegistry;
    template <typename Registry, typename __Base, typename... _Ty>
    struct Collector;

    struct RegistryBase;
}

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

    struct BinaryInfo;
}

namespace Threading {
    struct ConnectionBase;
    template <typename T, typename... _Ty>
    struct ConnectionInstance;
    struct TaskQueue;
    struct TaskHandle;
    template <typename T, bool Immediate = false>
    struct Task;
    struct TaskPromiseTypeBase;
    struct TaskSuspendablePromiseTypeBase;

    struct WorkGroup;
    struct WorkGroupHandle;
    struct Scheduler;

    struct DataMutex;
}

namespace Ini {
    struct IniFile;
    struct IniSection;
}

}
