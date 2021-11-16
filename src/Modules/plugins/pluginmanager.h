#pragma once

#if ENABLE_PLUGINS

#include "../threading/signal.h"

namespace Engine {
namespace Plugins {

    struct MODULES_EXPORT PluginManager {
        static PluginManager &getSingleton();

        PluginManager();
        ~PluginManager();		

        bool setup(bool loadCache = true, bool loadExe = true);

        PluginSection &section(const std::string &name);
        PluginSection &operator[](const std::string &name);
        const PluginSection &at(const std::string &name) const;

        Plugin *getPlugin(const std::string &name);

        std::map<std::string, PluginSection>::const_iterator begin() const;
        std::map<std::string, PluginSection>::const_iterator end() const;
        std::map<std::string, PluginSection>::iterator begin();
        std::map<std::string, PluginSection>::iterator end();

        Threading::TaskFuture<bool> loadFromFile(const Filesystem::Path &p);
        void saveToFile(const Filesystem::Path &p, bool withTools);

        std::mutex mDependenciesMutex;

        Threading::SignalStub<Filesystem::Path, bool> &exportSignal();

    protected:
        void setupSection(const std::string &name, bool exclusive, bool atleastOne);

        void saveSelection(Threading::Barrier &barrier, Ini::IniFile &file, bool withTools);
        Threading::TaskFuture<bool> loadSelection(Threading::Barrier &barrier, Ini::IniFile file);

        void onUpdate(Threading::Barrier &barrier);

        friend struct PluginSection;

    private:
        std::map<std::string, PluginSection> mSections;

        bool mLoadingSelectionFile = false;

        Threading::Signal<Filesystem::Path, bool> mExportSignal;
    };

}

}

#define IF_PLUGIN(p) if (Engine::Plugins::PluginManager::getSingleton().isLoaded(#p))
#define THROW_PLUGIN(errorMsg) throw errorMsg

#else

#define IF_PLUGIN(p) if constexpr (!Engine::streq("BUILD_" #p, STRINGIFY2(BUILD_##p)))
#define THROW_PLUGIN(errorMsg) throw errorMsg

#endif
