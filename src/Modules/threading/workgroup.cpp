#include "../moduleslib.h"

#if ENABLE_THREADING

#    include "workgroup.h"

namespace Engine {
namespace Threading {

    static THREADLOCAL(WorkGroup *) sSelf = nullptr;

    static std::vector<std::pair<std::function<Any()>, std::vector<Any>>> &sWorkgroupLocalBssConstructors()
    {
        static std::vector<std::pair<std::function<Any()>, std::vector<Any>>> dummy;
        return dummy;
    }

    static std::vector<std::pair<std::function<Any()>, std::vector<Any>>> &sWorkgroupLocalObjectConstructors()
    {
        static std::vector<std::pair<std::function<Any()>, std::vector<Any>>> dummy;
        return dummy;
    }

    static std::vector<SignalSlot::TaskHandle> &sThreadInitializers()
    {
        static std::vector<SignalSlot::TaskHandle> dummy;
        return dummy;
    }

    static std::atomic<size_t> sWorkgroupInstanceCounter = 0;

    WorkGroup::WorkGroup(const std::string &name)
        : mInstanceCounter(sWorkgroupInstanceCounter.fetch_add(1))
        , mName(name.empty() ? "Workgroup_" + std::to_string(mInstanceCounter) : name)
    {
        ThreadLocalStorage::init(true);
        ThreadLocalStorage::init(false);

        assert(!sSelf);
        sSelf = this;

        setCurrentThreadName(mName + "_Main");

        for (std::pair<std::function<Any()>, std::vector<Any>> &p : sWorkgroupLocalBssConstructors()) {
            if (p.second.size() <= mInstanceCounter)
                p.second.resize(mInstanceCounter + 1);
            if (!p.second[mInstanceCounter])
                p.second[mInstanceCounter] = p.first();
        }

        for (std::pair<std::function<Any()>, std::vector<Any>> &p : sWorkgroupLocalObjectConstructors()) {
            if (p.second.size() <= mInstanceCounter)
                p.second.resize(mInstanceCounter + 1);
            if (!p.second[mInstanceCounter])
                p.second[mInstanceCounter] = p.first();
        }

        for (const SignalSlot::TaskHandle &task : sThreadInitializers()) {
            task();
        }

        for (const SignalSlot::TaskHandle &task : mThreadInitializers) {
            task();
        }
    }

    WorkGroup::~WorkGroup()
    {
        assert(singleThreaded());
        finalizeThread();
    }

    void WorkGroup::addThreadInitializer(SignalSlot::TaskHandle &&task)
    {
        assert(mSubThreads.empty());
        mThreadInitializers.emplace_back(std::move(task));
    }

    void WorkGroup::addStaticThreadInitializer(SignalSlot::TaskHandle &&task)
    {
        sThreadInitializers().emplace_back(std::move(task));
    }

    bool WorkGroup::singleThreaded()
    {
        return mSubThreads.empty();
    }

    void WorkGroup::checkThreadStates()
    {
        auto pivot = std::remove_if(mSubThreads.begin(), mSubThreads.end(),
            [](std::future<int> &f) {
                return f.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
            });

        for (auto it = pivot; it != mSubThreads.end(); ++it) {
            it->get();
        }

        mSubThreads.erase(pivot, mSubThreads.end());
    }

    const std::string &WorkGroup::name() const
    {
        return mName;
    }

    void WorkGroup::initThread(const std::string &name)
    {

        ThreadLocalStorage::init(true);
        ThreadLocalStorage::init(false);

        assert(!sSelf);
        sSelf = this;

        setCurrentThreadName(mName + "_" + name);

        for (const SignalSlot::TaskHandle &task : sThreadInitializers()) {
            task();
        }

        for (const SignalSlot::TaskHandle &task : mThreadInitializers) {
            task();
        }
    }

    void WorkGroup::finalizeThread()
    {

        assert(sSelf == this);
        sSelf = nullptr;

        ThreadLocalStorage::finalize(false);
        ThreadLocalStorage::finalize(true);
    }

    int WorkGroup::registerLocalBssVariable(std::function<Any()> ctor)
    {
        sWorkgroupLocalBssConstructors().emplace_back(std::move(ctor), std::vector<Any> {});
        return -static_cast<int>(sWorkgroupLocalBssConstructors().size());
    }

    void WorkGroup::unregisterLocalBssVariable(int index)
    {
        sWorkgroupLocalBssConstructors()[-(index + 1)] = {};
    }

    int WorkGroup::registerLocalObjectVariable(std::function<Any()> ctor)
    {
        sWorkgroupLocalObjectConstructors().emplace_back(std::move(ctor), std::vector<Any> {});
        return sWorkgroupLocalObjectConstructors().size() - 1;
    }

    void WorkGroup::unregisterLocalObjectVariable(int index)
    {
        sWorkgroupLocalObjectConstructors()[index] = {};
    }

    const Any &WorkGroup::localVariable(int index)
    {
        size_t selfIndex = self().mInstanceCounter;
        std::vector<std::pair<std::function<Any()>, std::vector<Any>>> &constructors = index < 0 ? sWorkgroupLocalBssConstructors() : sWorkgroupLocalObjectConstructors();
        if (index < 0)
            index = -(index + 1);
        std::pair<std::function<Any()>, std::vector<Any>> &p = constructors[index];
        if (p.second.size() <= selfIndex)
            p.second.resize(selfIndex + 1);
        if (!p.second[selfIndex])
            p.second[selfIndex] = p.first();
        return p.second.at(selfIndex);
    }

    WorkGroup &WorkGroup::self()
    {
        assert(isInitialized());
        return *sSelf;
    }

    bool WorkGroup::isInitialized()
    {
        return sSelf != nullptr;
    }

}
}

#endif
