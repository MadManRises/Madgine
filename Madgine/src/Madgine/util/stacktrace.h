#pragma once

namespace Engine{
    namespace Util{

        struct StackTraceIterator{
        public:
            StackTraceIterator(const std::unique_ptr<TraceBack> *ptr) : mPtr(ptr) {}

            TraceBack &operator *(){
                return **mPtr;
            }

            TraceBack *operator->(){
                return mPtr->get();
            }

            void operator++(){
                ++mPtr;
            }

            bool operator==(const StackTraceIterator &other) const{
                return other.mPtr == mPtr;
            }

            bool operator!=(const StackTraceIterator &other) const{
                return other.mPtr != mPtr;
            }

        private:
            const std::unique_ptr<TraceBack> *mPtr;
        };


        void getStackTrace(size_t skip, void **buffer, size_t size);
        void resolveSymbols(void *const *data, std::unique_ptr<TraceBack> *buffer, size_t size);
        
        template <size_t S>
        struct StackTrace{

            using iterator = StackTraceIterator;

            StackTrace() = default;

            StackTrace(const StackTrace<S> &other) :
                mTrace(other.mTrace){}
            
            iterator begin() const {
                return iterator(mReadableTrace.data());
            }

            iterator end() const {
                size_t itemCount = calculateReadable();
                return iterator(mReadableTrace.data() + itemCount);
            }

            size_t calculateReadable() const {
                size_t itemCount;
                for (itemCount = S; itemCount > 0 && mTrace[itemCount - 1] == nullptr; --itemCount);
                resolveSymbols(mTrace.data(), mReadableTrace.data(), itemCount);
                return itemCount;
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
            mutable std::array<std::unique_ptr<TraceBack>, S> mReadableTrace;
        };

    }
}

namespace std{
    template <size_t S>
    struct hash<Engine::Util::StackTrace<S>> {
        size_t operator()(const Engine::Util::StackTrace<S> &stacktrace) const {
            size_t hash = 0;
            for (int i = 0; i < S && stacktrace.mTrace[i]; ++i){
                hash = reinterpret_cast<size_t>(stacktrace.mTrace[i]) + (hash << 6) + (hash << 16) - hash;
            }
            return hash;
        }
    };
}