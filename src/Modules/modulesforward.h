#pragma once

namespace Engine {

namespace UniqueComponent {
    struct IndexHolder;

    struct CollectorManager;
    template <typename T, typename _Collector, typename _Base>
    struct Component;
    template <typename Base>
    struct NamedComponent;
    template <typename Registry>
    struct Selector;
    template <typename C, typename Registry>
    struct Container;
    template <typename T, typename _Collector, typename Base>
    struct VirtualComponentBase;
    template <typename T, typename Base, typename _VBase = Base>
    struct VirtualComponentImpl;
    template <typename _Base, typename... Annotations>
    struct Registry;
    template <typename _Base, typename... Annotations>
    struct NamedRegistry;
    template <typename Registry>
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

    namespace Tasks {
        struct TaskTracker;
    }

    template <typename T, size_t S>
    struct History;
    template <typename T>
    struct HistoryData;
}

namespace Plugins {
    struct PluginManager;
    struct Plugin;
    struct PluginSection;

    struct BinaryInfo;
}

namespace Threading {
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

    template <typename... _Ty>
    struct SignalStub;
    struct ConnectionStore;

    struct CustomClock;
    struct CustomTimepoint;
}


namespace Ini {
    struct IniFile;
    struct IniSection;
}

}
