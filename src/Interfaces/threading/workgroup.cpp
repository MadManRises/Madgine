#include "../interfaceslib.h"

#include "workgroup.h"

namespace Engine
{
	namespace Threading
	{
		static std::vector<std::pair<void(*)(WorkGroup &group), void(*)(WorkGroup &group)>> &sWorkgroupLocalVariables()
		{
			static std::vector<std::pair<void(*)(WorkGroup &), void(*)(WorkGroup &)>> data;
			return data;
		}

		static size_t sWorkgroupInstanceCounter = 0;		

		WorkGroup::WorkGroup(const std::string & name) :
			mProfiler(*this),
			mName(name.empty() ? "Workgroup_" + std::to_string(++sWorkgroupInstanceCounter) : name)
		{
			setCurrentThreadName(mName + "_Main");
			for (auto p : sWorkgroupLocalVariables())
			{
				p.first(*this);
			}
		}

		WorkGroup::~WorkGroup()
		{
			for (auto p : sWorkgroupLocalVariables())
			{
				p.second(*this);
			}
			assert(singleThreaded());
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

		SignalSlot::TaskQueue & WorkGroup::taskQueue()
		{
			return mTaskQueue;
		}

		void WorkGroup::registerWorkgroupLocalVariable(void(*init)(WorkGroup &), void(*finalize)(WorkGroup&))
		{
			sWorkgroupLocalVariables().emplace_back(init, finalize);
		}

	}
}
