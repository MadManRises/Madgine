#include "../moduleslib.h"

#if ENABLE_PLUGINS

#    include "uniquecomponentregistry.h"

#    include "../plugins/binaryinfo.h"

#    include "Interfaces/filesystem/api.h"

#    include "codegen/codegen_cpp.h"

namespace Engine {

std::string fixInclude(const char *pStr, const Plugins::BinaryInfo *binInfo)
{
    Filesystem::Path p = pStr;
    if (p.isRelative())
        p = Filesystem::Path { BINARY_OUT_DIR } / p;
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

    for (UniqueComponentRegistryBase *reg : registryRegistry()) {
        if (skipBinary(reg->mBinary))
            continue;
        binaries.insert(reg->mBinary);

        for (CollectorInfoBase *collector : *reg) {
            if (!skipBinary(collector->mBinary))
                binaries.insert(collector->mBinary);
        }
    }

    CodeGen::CppFile file;

    file.include(0, "Modules/moduleslib.h");
    file.include(0, "Modules/uniquecomponent/uniquecomponentregistry.h");

    for (const Plugins::BinaryInfo *bin : binaries) {
        if (strlen(bin->mPrecompiledHeaderPath)) {
            file.beginCondition("BUILD_"s + bin->mName);
            file.include(0, bin->mPrecompiledHeaderPath);
            file.endCondition("BUILD_"s + bin->mName);
        }
    }

    for (UniqueComponentRegistryBase *reg : registryRegistry()) {
        const Plugins::BinaryInfo *bin = reg->mBinary;
        if (skipBinary(bin))
            continue;
        file.beginCondition("BUILD_"s + bin->mName);
        file.include(1, fixInclude(reg->named_type_info()->mHeaderPath, bin));

        for (CollectorInfoBase *collector : *reg) {
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

    for (UniqueComponentRegistryBase *reg : registryRegistry()) {
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

        for (CollectorInfoBase *collector : *reg) {
            if (skipBinary(collector->mBinary))
                continue;
            file.beginCondition("BUILD_"s + collector->mBinary->mName);
            for (const std::vector<const TypeInfo *> &typeInfos : collector->mElementInfos) {
                const TypeInfo *typeInfo = typeInfos.front();
                if (notInSkip(typeInfo))
                    file << "		createComponent<"
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

        for (CollectorInfoBase *collector : *reg) {
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
size_t component_index<)"
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
std::map<std::string_view, size_t> )"
                 << reg->named_type_info()->mFullName
                 << R"(::sComponentsByName()
{
	return {
)";

            for (CollectorInfoBase *collector : *reg) {
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

MODULES_EXPORT std::vector<UniqueComponentRegistryBase *> &
registryRegistry()
{
    static std::vector<UniqueComponentRegistryBase *> dummy;
    return dummy;
}

} // namespace Engine

#endif