#include "../interfaceslib.h"

#include "workgroup.h"

namespace Engine
{
	namespace Threading
	{
		static THREADLOCAL(WorkGroup*) sSelf = nullptr;

		static std::vector<Any> &sWorkgroupLocalVariables()
		{
			static std::vector<Any> dummy;
			return dummy;
		}

		static size_t sWorkgroupInstanceCounter = 0;		

		WorkGroup::WorkGroup(const std::string & name) :
			mName(name.empty() ? "Workgroup_" + std::to_string(++sWorkgroupInstanceCounter) : name)		
		{
			initThread("Main");
			for (const Any &v : sWorkgroupLocalVariables())
			{
				mLocalVariables.emplace_back(v);
			}
		}

		WorkGroup::~WorkGroup()
		{
			assert(singleThreaded());
			assert(sSelf == this);
			sSelf = nullptr;
		}

		void WorkGroup::addThreadInitializer(SignalSlot::TaskHandle && task)
		{
			assert(mSubThreads.empty());
			mThreadInitializers.emplace_back(std::move(task));
		}

		bool WorkGroup::singleThreaded()
		{
			return mSubThreads.empty();
		}

		void WorkGroup::checkThreadStates()
		{
			auto pivot =
				std::remove_if(mSubThreads.begin(), mSubThreads.end(),
					[](std::future<int> &f)
				{
					return f.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
				}
			);

			for (auto it = pivot; it != mSubThreads.end(); ++it)
			{
				it->get();
			}

			mSubThreads.erase(pivot, mSubThreads.end());
		}

		const std::string & WorkGroup::name() const
		{
			return mName;
		}

		SignalSlot::DefaultTaskQueue & WorkGroup::taskQueue()
		{
			return mTaskQueue;
		}

		void WorkGroup::initThread(const std::string &name)
		{

			ThreadLocalStorage::init();

			assert(!sSelf);
			sSelf = this;

			setCurrentThreadName(mName + "_" + name);

			mProfiler.registerCurrentThread();

			for (const SignalSlot::TaskHandle &task : mThreadInitializers)
			{
				task();
			}

		}

		size_t WorkGroup::registerLocalVariable(Any &&ctor)
		{
			sWorkgroupLocalVariables().emplace_back(std::move(ctor));
			return sWorkgroupLocalVariables().size() - 1;
		}

		const Any& WorkGroup::localVariable(size_t index)
		{
			return self().mLocalVariables.at(index);
		}

		WorkGroup & WorkGroup::self()
		{
			assert(sSelf);
			return *sSelf;
		}

	}
}
