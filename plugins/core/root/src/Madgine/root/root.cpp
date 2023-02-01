#include "../rootlib.h"

#include "root.h"

#include "Interfaces/debug/memory/memory.h"
#include "Modules/plugins/pluginmanager.h"
#include "keyvalueregistry.h"

#include "Madgine/cli/cli.h"
#include "Modules/uniquecomponent/uniquecomponentcollectormanager.h"

#include "Madgine/cli/parameter.h"

#include "Modules/ini/inifile.h"

#include "Madgine/codegen/codegen_cpp.h"

#include "Modules/plugins/binaryinfo.h"

#include "Modules/uniquecomponent/uniquecomponentregistry.h"

#include "rootcomponentbase.h"

#include "Interfaces/util/standardlog.h"

#include "Interfaces/filesystem/fsapi.h"

namespace Engine {
namespace Root {

    static Root *sSingleton = nullptr;

    CLI::Parameter<bool> noPluginCache { { "--no-plugin-cache", "-npc" }, false, "Disables the loading of the cached plugin selection at startup." };
    CLI::Parameter<std::string> loadPlugins { { "--load-plugins", "-lp" }, "", "If set the pluginmanager will load the specified config file after loading the cached plugin-file." };
    CLI::Parameter<std::string> exportPlugins { { "--export-plugins", "-ep" }, "", "If set the pluginmanager will save the current plugin selection after the boot to the specified config file and will export a corresponding uniquecomponent configuration source file." };

    CLI::Parameter<bool> toolModeParameter { { "--toolMode", "-t" }, false, "If set, no application will be started. Only the root will be initialized and then immediately shutdown again." };
    CLI::Parameter<Engine::Util::MessageType> logLevel { { "--logLevel", "-l" }, Engine::Util::MessageType::INFO_TYPE, "Specify log-level." };

    Root::Root(int argc, char **argv)
        : Root(std::make_unique<CLI::CLICore>(argc, argv))
    {
    }

    Root::Root(std::unique_ptr<CLI::CLICore> cli)
        : mCLI(std::move(cli))
        , mTaskQueue("Root")
    {

        assert(!sSingleton);
        sSingleton = this;

        Util::StandardLog::setLogLevel(logLevel);

#if ENABLE_PLUGINS
        mPluginManager = std::make_unique<Plugins::PluginManager>();
        mErrorCode = mPluginManager->setup(!noPluginCache, mCLI->mProgramPath.stem(), loadPlugins);
        mCollectorManager = std::make_unique<UniqueComponent::CollectorManager>(*mPluginManager);

        if (mErrorCode == 0 && !exportPlugins->empty()) {

            auto helper = [this](const Filesystem::Path &path, bool hasTools) {
                Filesystem::createDirectories(path);
                Ini::IniFile file;
                LOG("Saving Plugins to '" << path << "'");
                mPluginManager->saveSelection(file, hasTools);
                if (!file.saveToDisk(path / "plugins.cfg"))
                    mErrorCode = -1;
                else
                    exportStaticComponentHeader(path / "components.cpp", hasTools);
            };

            Filesystem::Path p = *exportPlugins;
            helper(p, false);
            Filesystem::Path p_tools = p.parentPath() / (std::string { p.stem() } + "_tools");
            helper(p_tools, true);
        }
#endif

#if ENABLE_MEMTRACKING
        mMemTracker = std::make_unique<Debug::Memory::MemoryTracker>();
#endif

        mComponents = std::make_unique<RootComponentContainer<std::vector<Placeholder<0>>>>(*this);

        for (std::unique_ptr<RootComponentBase> &component : *mComponents) {
            KeyValueRegistry::registerGlobal(component->key().data(), component.get());

            if (mErrorCode == 0)
                mErrorCode = component->mErrorCode;
        }
    }

    Root::~Root()
    {
        assert(sSingleton == this);
        sSingleton = nullptr;
    }

    Root &Root::getSingleton()
    {
        assert(sSingleton);
        return *sSingleton;
    }

    RootComponentBase &Root::getComponent(size_t i)
    {
        return mComponents->get(i);
    }

    bool Root::toolMode() const
    {
        return toolModeParameter;
    }

    int Root::errorCode()
    {
        return mErrorCode;
    }

    Threading::TaskQueue *Root::taskQueue()
    {
        return &mTaskQueue;
    }

#if ENABLE_PLUGINS
    std::string fixInclude(const char *pStr, const Plugins::BinaryInfo *binInfo)
    {
        Filesystem::Path p = pStr;
        if (p.isRelative())
            p = Filesystem::Path { BINARY_DIR } / p;
        return p.relative(binInfo->mSourceRoot).str();
    };

    static std::vector<const TypeInfo *> &sSkip()
    {
        static std::vector<const TypeInfo *> dummy;
        return dummy;
    }

    void skipUniqueComponentOnExport(const TypeInfo *t)
    {
        sSkip().push_back(t);
    }

    void exportStaticComponentHeader(const Filesystem::Path &outFile, bool hasTools)
    {
        LOG("Exporting uniquecomponent configuration source file '" << outFile << "'");
        LOG_WARNING("TODO: Export in alphabetical order to prevent changes in the version control.");

        std::set<const Plugins::BinaryInfo *> binaries;

        auto notInSkip = [&](const TypeInfo *v) {
            return std::ranges::find_if(sSkip(), [=](const TypeInfo *v2) {
                return strcmp(v->mFullName, v2->mFullName) == 0;
            }) == sSkip().end();
        };

        auto skipBinary = [&](const Plugins::BinaryInfo *bin) {
            return !hasTools && StringUtil::endsWith(bin->mName, "Tools");
        };

        for (UniqueComponent::RegistryBase *reg : UniqueComponent::registryRegistry()) {
            if (skipBinary(reg->mBinary))
                continue;
            binaries.insert(reg->mBinary);

            for (UniqueComponent::CollectorInfoBase *collector : *reg) {
                if (!skipBinary(collector->mBinary))
                    binaries.insert(collector->mBinary);
            }
        }

        CodeGen::CppFile file;

        file.include(0, "Modules/moduleslib.h");
        file.include(0, "Modules/uniquecomponent/uniquecomponentregistry.h");
        file.include(0, "Modules/uniquecomponent/uniquecomponent.h");

        for (const Plugins::BinaryInfo *bin : binaries) {
            if (strlen(bin->mPrecompiledHeaderPath)) {
                file.beginCondition("BUILD_"s + bin->mName);
                file.include(0, bin->mPrecompiledHeaderPath);
                file.endCondition("BUILD_"s + bin->mName);
            }
        }

        for (UniqueComponent::RegistryBase *reg : UniqueComponent::registryRegistry()) {
            const Plugins::BinaryInfo *bin = reg->mBinary;
            if (skipBinary(bin))
                continue;
            file.beginCondition("BUILD_"s + bin->mName);
            file.include(1, fixInclude(reg->named_type_info()->mHeaderPath, bin));

            for (UniqueComponent::CollectorInfoBase *collector : *reg) {
                if (skipBinary(collector->mBinary))
                    continue;
                for (const std::vector<const TypeInfo *> &typeInfos : collector->mElementInfos) {
                    const TypeInfo *typeInfo = typeInfos.front();
                    if (notInSkip(typeInfo)) {
                        file.beginCondition("BUILD_"s + collector->mBinary->mName);
                        file.include(1, fixInclude(typeInfo->mHeaderPath, collector->mBinary));
                        file.endCondition("BUILD_"s + collector->mBinary->mName);
                    }
                }
            }
            file.endCondition("BUILD_"s + bin->mName);
        }

        file.beginNamespace("Engine");

        for (UniqueComponent::RegistryBase *reg : UniqueComponent::registryRegistry()) {
            if (skipBinary(reg->mBinary))
                continue;
            file.beginCondition("BUILD_"s + reg->mBinary->mName);

            file << R"(template <>
std::vector<)"
                 << reg->type_info()->mFullName << "::F> " << reg->type_info()->mFullName
                 << R"(::sComponents()
{
	return {
)";

            for (UniqueComponent::CollectorInfoBase *collector : *reg) {
                if (skipBinary(collector->mBinary))
                    continue;
                file.beginCondition("BUILD_"s + collector->mBinary->mName);
                for (const std::vector<const TypeInfo *> &typeInfos : collector->mElementInfos) {
                    const TypeInfo *typeInfo = typeInfos.front();
                    if (notInSkip(typeInfo))
                        file << "		UniqueComponent::createComponent<"
                             << typeInfo->mFullName << ">,\n";
                }
                file.endCondition("BUILD_"s + collector->mBinary->mName);
            }

            file << R"(
	}; 
}
)";

            file << R"(
#    define ACC 0

)";

            for (UniqueComponent::CollectorInfoBase *collector : *reg) {
                if (skipBinary(collector->mBinary))
                    continue;
                file.beginCondition("BUILD_"s + collector->mBinary->mName);
                file << "constexpr size_t CollectorBaseIndex_"
                     << collector->mBaseInfo->mTypeName << "_"
                     << collector->mBinary->mName << " = ACC;\n";
                size_t i = 0;
                for (const std::vector<const TypeInfo *> &typeInfos : collector->mElementInfos) {
                    if (notInSkip(typeInfos.front())) {
                        for (const TypeInfo *typeInfo : typeInfos) {
                            while (typeInfo) {
                                file << R"(template <>
size_t UniqueComponent::component_index<)"
                                     << typeInfo->mFullName
                                     << ">() { return CollectorBaseIndex_"
                                     << collector->mBaseInfo->mTypeName << "_"
                                     << collector->mBinary->mName << " + " << i
                                     << "; }\n";
                                typeInfo = typeInfo->mDecayType ? typeInfo->mDecayType : nullptr;
                            }
                        }
                        ++i;
                    }
                }
                file << "#        undef ACC\n"
                     << "#        define ACC CollectorBaseIndex_"
                     << collector->mBaseInfo->mTypeName << "_"
                     << collector->mBinary->mName << " + " << i << "\n";
                file.endCondition("BUILD_"s + collector->mBinary->mName);
            }

            file << "\n#    undef ACC\n\n";

            if (reg->mIsNamed) {
                file << R"(template <>
const std::map<std::string_view, IndexType<uint32_t>> &)"
                     << reg->named_type_info()->mFullName
                     << R"(::sComponentsByName()
{
    static std::map<std::string_view, IndexType<uint32_t>> mapping {
)";

                for (UniqueComponent::CollectorInfoBase *collector : *reg) {
                    if (skipBinary(collector->mBinary))
                        continue;
                    file.beginCondition("BUILD_"s + collector->mBinary->mName);
                    size_t i = 0;
                    for (const std::vector<const TypeInfo *> &typeInfos : collector->mElementInfos) {
                        const TypeInfo *typeInfo = typeInfos.front();
                        if (notInSkip(typeInfo))
                            file << R"(		{")" << collector->mComponentNames[i] << R"(", CollectorBaseIndex_)"
                                 << collector->mBaseInfo->mTypeName << "_"
                                 << collector->mBinary->mName << " + " << i++ << "},\n";
                    }
                    file.endCondition("BUILD_"s + collector->mBinary->mName);
                }

                file << R"(
	}; 
    return mapping;
}
)";
            }

            file.endCondition("BUILD_"s + reg->mBinary->mName);
        }

        file.endNamespace();

        std::ofstream stream(outFile.str());
        assert(stream);

        file.generate(stream);
    }
#endif

}
}
