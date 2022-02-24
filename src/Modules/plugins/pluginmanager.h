#pragma once

#if ENABLE_PLUGINS

namespace Engine {
namespace Plugins {

    struct MODULES_EXPORT PluginManager {
        static PluginManager &getSingleton();

        PluginManager();
        PluginManager(const PluginManager &) = delete;
        ~PluginManager();		

        PluginManager &operator=(const PluginManager &) = delete;

        bool setup(bool loadCache, std::string_view programName, std::string_view configFile);

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

        void saveSelection(Ini::IniFile &file, bool withTools);
        bool loadSelection(const Ini::IniFile &file, bool withTools);

    protected:
        void setupSection(const std::string &name, bool exclusive, bool atleastOne);

        void onUpdate();

        friend struct PluginSection;

    private:
        std::map<std::string, PluginSection> mSections;        

        bool mUseCache = false;
    };

}

}

#define IF_PLUGIN(p) if (Engine::Plugins::PluginManager::getSingleton().isLoaded(#p))
#define THROW_PLUGIN(errorMsg) throw errorMsg

#else

#define IF_PLUGIN(p) if constexpr (!Engine::streq("BUILD_" #p, STRINGIFY2(BUILD_##p)))
#define THROW_PLUGIN(errorMsg) throw errorMsg

#endif
