#pragma once

#if ENABLE_PLUGINS

#include "../threading/signal.h"

namespace Engine {
namespace Plugins {

    struct MODULES_EXPORT PluginManager {
        static PluginManager &getSingleton();

        PluginManager();
        PluginManager(const PluginManager &) = delete;
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

        bool loadFromFile(const Filesystem::Path &p, bool withTools);
        void saveToFile(const Filesystem::Path &p, bool withTools);

        Threading::SignalStub<Filesystem::Path, bool> &exportSignal();

    protected:
        void setupSection(const std::string &name, bool exclusive, bool atleastOne);

        void saveSelection(Ini::IniFile &file, bool withTools);
        bool loadSelection(const Ini::IniFile &file, bool withTools);

        void onUpdate();

        friend struct PluginSection;

    private:
        std::map<std::string, PluginSection> mSections;        

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
