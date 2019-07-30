#pragma once

#include "task.h"

namespace Engine
{
	namespace SignalSlot
	{

		struct MODULES_EXPORT TaskTracker 
		{
			TaskTracker(TaskHandle &&task, std::atomic<size_t> &tracker);
			TaskTracker(const TaskTracker &) = delete;
			TaskTracker(TaskTracker &&other);
			~TaskTracker();

			TaskHandle mTask;

		private:
			std::atomic<size_t> *mTracker;
		};

		struct MODULES_EXPORT TaskQueue
		{
			TaskQueue(const std::string &name);
			virtual ~TaskQueue() = default;

			void queue(TaskHandle &&task, const std::vector<Threading::DataMutex*> &dependencies = {});
			void queue_after(TaskHandle &&task, std::chrono::steady_clock::duration duration, const std::vector<Threading::DataMutex*> &dependencies = {});
			void queue_for(TaskHandle &&task, std::chrono::steady_clock::time_point time_point, const std::vector<Threading::DataMutex*> &dependencies = {});

			void addRepeatedTask(TaskHandle &&task, std::chrono::steady_clock::duration interval = std::chrono::steady_clock::duration::zero());

			virtual std::optional<TaskTracker> fetch(std::chrono::steady_clock::time_point &nextTask, int &idleCount);

			virtual bool idle() const;

			const std::string &name() const;
			
			void update(int idleCount = -1);
			void update(std::chrono::steady_clock::time_point &nextAvailableTaskTime, int idleCount = -1);
			void waitForTasks(std::chrono::steady_clock::time_point until = std::chrono::steady_clock::time_point::max());

			bool running() const;
			void stop();

		protected:

			struct ScheduledTask {
				TaskTracker mTask;
				std::chrono::steady_clock::time_point mScheduledFor = std::chrono::steady_clock::time_point::min();
			};


			void queueInternal(ScheduledTask &&tracker);

			virtual std::optional<TaskTracker> fetch_on_idle();

			TaskTracker wrapTask(TaskHandle &&task);
			
		private:
			std::string mName;

			std::atomic<bool> mRunning = true;
			std::atomic<size_t> mTaskCount = 0;

			struct RepeatedTask {
				SignalSlot::TaskHandle mTask;
				std::chrono::steady_clock::duration mInterval = std::chrono::steady_clock::duration::zero();
				std::chrono::steady_clock::time_point mNextExecuted = std::chrono::steady_clock::time_point::min();				
			};
			
			std::list<ScheduledTask> mQueue;
			std::list<RepeatedTask> mRepeatedTasks;
			mutable std::mutex mMutex;
			std::condition_variable mCv;
			
		};

	}
}