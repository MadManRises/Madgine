#pragma once

#include "Modules/madgineobject/madgineobject.h"

#include "globalapicollector.h"

namespace Engine {
namespace App {

    class MADGINE_BASE_EXPORT Application : public ScopeBase,
                                            public MadgineObject {
    public:
        Application(const AppSettings &settings);

        virtual ~Application();

        template <class T>
        T &getGlobalAPIComponent()
        {
            return static_cast<T &>(getGlobalAPIComponent(component_index<T>()));
        }

        GlobalAPIBase &getGlobalAPIComponent(size_t, bool = true);


        Application &getSelf(bool = true);

        
        const AppSettings &settings();

        virtual const MadgineObject *parent() const override;
        
		static Application &getSingleton();

    protected:

        bool init() override;

        void finalize() override;

    private:
        const AppSettings &mSettings;

        int mGlobalAPIInitCounter;

    public:
        GlobalAPIContainer<std::vector<Placeholder<0>>> mGlobalAPIs;
    };
}
}
