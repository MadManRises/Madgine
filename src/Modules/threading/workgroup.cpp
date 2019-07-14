#include "../moduleslib.h"

#include "workgroup.h"

namespace Engine {
namespace Threading {
	
    static THREADLOCAL(WorkGroup *) sSelf = nullptr;

    static std::vector<std::function<Any()>> &sWorkgroupLocalBssConstructors()
    {
        static std::vector<std::function<Any()>> dummy;
        return dummy;
    }

	static std::vector<std::function<Any()>> &sWorkgroupLocalObjectConstructors()
    {
        static std::vector<std::function<Any()>> dummy;
        return dummy;
    }

	static std::vector<SignalSlot::TaskHandle> &sThreadInitializers()
    {
        static std::vector<SignalSlot::TaskHandle> dummy;
        return dummy;
    }

    static size_t sWorkgroupInstanceCounter = 0;

    WorkGroup::WorkGroup(const std::string &name)
        : mName(name.empty() ? "Workgroup_" + std::to_string(++sWorkgroupInstanceCounter) : name)
    {
        ThreadLocalStorage::init(true);
        ThreadLocalStorage::init(false);

        assert(!sSelf);
        sSelf = this;

		setCurrentThreadName(mName + "_Main");  
		
        for (const std::function<Any()> &ctor : sWorkgroupLocalBssConstructors()) {
            mBssVariables.emplace_back(ctor());
        }
        for (const std::function<Any()> &ctor : sWorkgroupLocalObjectConstructors()) {
            mObjectVariables.emplace_back(ctor());
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
        assert(sSelf == this);
        sSelf = nullptr;
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

    DefaultTaskQueue &WorkGroup::taskQueue()
    {
        return mTaskQueue;
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

    int WorkGroup::registerLocalBssVariable(std::function<Any()> ctor)
    {
        sWorkgroupLocalBssConstructors().emplace_back(std::move(ctor));
        return -static_cast<int>(sWorkgroupLocalBssConstructors().size());
    }

	    int WorkGroup::registerLocalObjectVariable(std::function<Any()> ctor)
    {
        sWorkgroupLocalObjectConstructors().emplace_back(std::move(ctor));
        return sWorkgroupLocalObjectConstructors().size() - 1;
    }

    const Any &WorkGroup::localVariable(int index)
    {
        std::vector<Any> &variables = index < 0 ? self().mBssVariables : self().mObjectVariables;
        if (index < 0)
            index = -(index + 1);
        return variables.at(index);
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
