#pragma once

#if ENABLE_THREADING

#    include "Madgine/app/application.h"
#    include "Madgine/app/appsettings.h"
#    include "Modules/threading/workgrouphandle.h"

namespace Engine {
namespace Server {
    class MADGINE_SERVER_EXPORT ServerAppInstance {
    public:
        template <class T>
        ServerAppInstance(T &&initCallback, const App::AppSettings &settings = {})
            : mApplication(nullptr)
            , mSettings(settings)
            , mName("thread_"s + std::to_string(++sInstanceCounter))
            , mWorkGroup(&ServerAppInstance::go<T>, this, std::forward<T>(initCallback))
        {
        }

        virtual ~ServerAppInstance();

        const char *key() const;

        //ValueType toValueType() const;

    protected:
        template <class T>
        int go(T initCallback, Threading::WorkGroup &workgroup)
        {
            mSettings.mRunMain = false;
            App::Application app(mSettings);
            mApplication = &app;
            return TupleUnpacker::invokeDefaultResult(0, std::move(initCallback), workgroup, app);
        }

    private:
        App::Application *mApplication;
        App::AppSettings mSettings;

        std::string mName;
        static size_t sInstanceCounter;

        Threading::WorkGroupHandle mWorkGroup;
    };
}
}

#endif