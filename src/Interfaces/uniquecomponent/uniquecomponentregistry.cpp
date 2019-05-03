#include "../interfaceslib.h"

#ifndef STATIC_BUILD

#include "uniquecomponentregistry.h"

#include "../plugins/binaryinfo.h"

#include "../util/stringutil.h"

#include "../filesystem/api.h"

namespace Engine {

struct CompareTypeInfo {
    bool operator()(const TypeInfo *t1, const TypeInfo *t2) const {
        return strcmp(t1->mFullName, t2->mFullName) == -1;
    }
};

struct GuardGuard {
    GuardGuard(std::ostream &o, const Plugins::BinaryInfo *b) : out(o), bin(b) {
        if (bin)
            out << "#ifdef BUILD_" << bin->mName << "\n";
    }

    ~GuardGuard() {
        if (bin)
            out << "#endif\n";
    }

    std::ostream &out;
    const Plugins::BinaryInfo *bin;
};

std::string fixInclude(const char *pStr, const Plugins::BinaryInfo *binInfo) {
    Filesystem::Path p = Filesystem::makeNormalized(pStr);
    return p.relative(binInfo->mSourceRoot).str();
};

void include(std::ostream &out, std::string header,
             const Plugins::BinaryInfo *bin = nullptr) {
    GuardGuard g(out, bin);
    out << "#include \"" << header << "\"\n";
}

void exportStaticComponentHeader(const Filesystem::Path &outFile,
                                 std::vector<const TypeInfo *> skip) {
    std::set<const Plugins::BinaryInfo *> binaries;

    auto notInSkip = [&](const TypeInfo *v) {
        return std::find_if(skip.begin(), skip.end(), [=](const TypeInfo *v2) {
                   return strcmp(v->mFullName, v2->mFullName) == 0;
               }) == skip.end();
    };

    for (auto &[name, reg] : registryRegistry()) {
        binaries.insert(reg->mBinary);

        for (CollectorInfo *collector : *reg) {
            binaries.insert(collector->mBinary);
        }
    }

    std::ofstream file(outFile.str());
    assert(file);

	include(file, "Interfaces/interfaceslib.h");

    for (const Plugins::BinaryInfo *bin : binaries) {
        if (strlen(bin->mPrecompiledHeaderPath)) {
            include(file, bin->mPrecompiledHeaderPath, bin);
        }
    }

    for (auto &[name, reg] : registryRegistry()) {
        const Plugins::BinaryInfo *bin = reg->mBinary;
        include(file, fixInclude(reg->type_info()->mHeaderPath, bin), bin);

        for (CollectorInfo *collector : *reg) {
            for (const TypeInfo *typeInfo : collector->mElementInfos) {
                if (notInSkip(typeInfo))
                    include(
                        file,
                        fixInclude(typeInfo->mHeaderPath, collector->mBinary),
                        collector->mBinary);
            }
        }
    }

    file << R"(

namespace Engine{

)";

    for (auto &[name, reg] : registryRegistry()) {
        GuardGuard g2(file, reg->mBinary);

        file << "	template <> std::vector<" << name << "::F> " << name
             << R"(::sComponents() {
		return {
)";

        for (CollectorInfo *collector : *reg) {
            GuardGuard g(file, collector->mBinary);
            for (const TypeInfo *typeInfo : collector->mElementInfos) {
                if (notInSkip(typeInfo))
                    file << "			createComponent<"
                         << typeInfo->mFullName << ">,\n";
            }
        }

        file << R"(
		}; 
	}

	#define ACC 0

)";

        for (CollectorInfo *collector : *reg) {
            GuardGuard g(file, collector->mBinary);
            file << "constexpr size_t CollectorBaseIndex_"
                 << collector->mBaseInfo->mTypeName << "_"
                 << collector->mBinary->mName << " = ACC;\n";
            size_t i = 0;
            for (const TypeInfo *typeInfo : collector->mElementInfos) {
                if (notInSkip(typeInfo)) {
                    while (typeInfo) {
                        file << "    template<> size_t component_index<"
                             << typeInfo->mFullName
                             << ">(){ return CollectorBaseIndex_"
                             << collector->mBaseInfo->mTypeName << "_"
                             << collector->mBinary->mName << " + " << i
                             << "; }\n";
                        typeInfo = typeInfo->mDecayType;
                    }
                    ++i;
                }
            }
            file << "#undef ACC\n" 
				 << "#define ACC CollectorBaseIndex_"
                 << collector->mBaseInfo->mTypeName << "_"
                 << collector->mBinary->mName << " + " << i << "\n";
        }

        file << "\n#undef ACC\n\n";
    }

    file << "}\n";
}

INTERFACES_EXPORT std::map<std::string, ComponentRegistryBase *> &
registryRegistry() {
    static std::map<std::string, ComponentRegistryBase *> dummy;
    return dummy;
}

} // namespace Engine

#endif