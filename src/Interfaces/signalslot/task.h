#pragma once

#include "../generic/tupleunpacker.h"

namespace Engine
{
	namespace SignalSlot
	{

		enum TaskState {
			STARTING,
			YIELD,
			SUCCESS,
			FAILURE
		};

		/*template <typename R, typename Y = void>
		struct TaskResult {
			TaskState mState;
			union {
				R mResult;
				Y mYield;
			};
		};*/

		struct Task {
			virtual TaskState execute() = 0;
			virtual ~Task() = default;
		};

		template <typename F>
		struct TaskImpl : Task {

			TaskImpl(F &&f) :
				mF(std::forward<F>(f))
			{
			}

			TaskState execute() override
			{
				return TupleUnpacker::invokeDefaultResult(SUCCESS, mF);
			}

		private:
			F mF;
		};

		struct TaskHandle {

			template <typename F>
			TaskHandle(F &&f) :
				mTask(std::make_unique<TaskImpl<std::remove_reference_t<F>>>(std::forward<F>(f))) {}

			TaskHandle(TaskHandle &&) = default;

			TaskHandle(const TaskHandle &) = delete;
			TaskHandle& operator=(const TaskHandle &) = delete;

			TaskState operator()() const {
				return mTask->execute();
			}

		private:
			std::unique_ptr<Task> mTask;
		};

	}
}