#pragma once

#include "traceback.h"

namespace Engine{
    namespace Debug {

		using FullStackTrace = std::pmr::vector<TraceBack>;

		INTERFACES_EXPORT void getStackTrace(size_t skip, void **buffer, size_t size);
                INTERFACES_EXPORT FullStackTrace resolveSymbols(void *const *data, size_t size);
        
        template <size_t S>
        struct StackTrace{

            StackTrace() = default;

            StackTrace(const StackTrace<S> &other) :
                mTrace(other.mTrace){}

            FullStackTrace calculateReadable(size_t count = S) const {
                size_t itemCount;
                for (itemCount = count; itemCount > 0 && mTrace[itemCount - 1] == nullptr; --itemCount);
                return resolveSymbols(mTrace.data(), itemCount);                
            }

            static StackTrace<S> getCurrent(size_t skip){
                StackTrace<S> result;
                getStackTrace(skip + 1, result.mTrace.data(), S);
                return result;
            }

            bool operator==(const StackTrace<S> &other) const {
                for (size_t i = 0; i < S; ++i){
                    if (mTrace[i] != other.mTrace[i])
                        return false;
                }
                return true;
            }

        private:
            friend struct std::hash<StackTrace<S>>;

            std::array<void*, S> mTrace;
        };



	}
}

namespace std{
    template <size_t S>
    struct hash<Engine::Debug::StackTrace<S>> {
        size_t operator()(const Engine::Debug::StackTrace<S> &stacktrace) const {
            size_t hash = 0;
            for (int i = 0; i < S && stacktrace.mTrace[i]; ++i){
                hash = reinterpret_cast<size_t>(stacktrace.mTrace[i]) + (hash << 6) + (hash << 16) - hash;
            }
            return hash;
        }
    };

	template <>
	struct hash<Engine::Debug::FullStackTrace> {
		size_t operator()(const Engine::Debug::FullStackTrace &stacktrace) const {
			std::hash<Engine::Debug::TraceBack> helper;
			size_t hash = 0;
			for (const Engine::Debug::TraceBack& t : stacktrace) {
				hash = helper(t) + (hash << 6) + (hash << 16) - hash;
			}
			return hash;
		}
	};
}