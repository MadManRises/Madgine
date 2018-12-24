#include "../baselib.h"

#ifndef STATIC_BUILD

#include "uniquecomponentregistry.h"

#include "Interfaces/plugins/pluginmanager.h"

#include "Interfaces/plugins/binaryinfo.h"

#include "Interfaces/util/pathutil.h"
#include "Interfaces/util/stringutil.h"


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
		std::experimental::filesystem::path p = PathUtil::make_normalized(pStr);
		return StringUtil::replace(PathUtil::relative(p, binInfo->mSourceRoot).generic_string(), '\\', '/');
	};

	void include(std::ostream &out, std::string header, const Plugins::BinaryInfo *bin = nullptr)
	{
		GuardGuard g(out, bin);
		out << "#include \"" << header << "\"\n";
	}

	void exportStaticComponentHeader(const std::experimental::filesystem::path &outFile, std::vector<const TypeInfo*> skip) {
		std::map<const TypeInfo *, std::vector<const TypeInfo*>, CompareTypeInfo> collectorData;
		std::set<const Plugins::BinaryInfo *> binaries;

		auto notInSkip = [&](const TypeInfo *v) {
			return std::find_if(skip.begin(), skip.end(), [=](const TypeInfo *v2) {return strcmp(v->mFullName, v2->mFullName) == 0; }) == skip.end();
		};

		for (const std::pair<const std::string, ComponentRegistryBase*> &registry : registryRegistry()) {
			collectorData[registry.second->type_info()];			
		}

		for (const std::pair<const std::string, Plugins::PluginSection> &sec : Plugins::PluginManager::getSingleton()) {
			for (const std::pair<const std::string, Plugins::Plugin> &p : sec.second) {
				if (p.second.isLoaded()) {
					const Plugins::BinaryInfo *binInfo = static_cast<const Plugins::BinaryInfo*>(p.second.getSymbol("binaryInfo"));
					binaries.insert(binInfo);
					auto f = (CollectorRegistry*(*)())p.second.getSymbol("collectorRegistry");
					if (f) {
						for (CollectorInfo *info : f()->mInfos) {
							if (notInSkip(info->mBaseInfo)) {
								auto &v = collectorData[info->mRegistryInfo];
								std::copy_if(info->mElementInfos.begin(), info->mElementInfos.end(), std::back_inserter(v), notInSkip);
							}
						}
					}
				}
			}
		}

		std::ofstream file(outFile);
		assert(file);

		for (const Plugins::BinaryInfo *bin : binaries)
		{
			if (strlen(bin->mPrecompiledHeaderPath))
			{
				include(file, bin->mPrecompiledHeaderPath, bin);
			}
		}
	

		for (const std::pair<const TypeInfo* const, std::vector<const TypeInfo*>> &p : collectorData) {
			const Plugins::BinaryInfo *bin = p.first->mBinary;
			std::experimental::filesystem::path path = PathUtil::make_normalized(p.first->mHeaderPath);
			include(file, fixInclude(p.first->mHeaderPath, bin), bin);
			
			for (const TypeInfo *typeInfo : p.second) {
				include(file, fixInclude(typeInfo->mHeaderPath, typeInfo->mBinary), typeInfo->mBinary);
			}
		}

		file << R"(

namespace Engine{

)";

		for (const std::pair<const TypeInfo* const, std::vector<const TypeInfo*>> &p : collectorData) {
			{
				GuardGuard g2(file, p.first->mBinary);
				file << "	template<> std::vector<" << p.first->mFullName << "::F> " << p.first->mFullName << "::sComponents() { return {\n";

				for (const TypeInfo *typeInfo : p.second) {
					GuardGuard g(file, typeInfo->mBinary);
					file << "		createComponent<" << typeInfo->mFullName << ">,\n";
				}

				file << "\n	}; }\n\n";
			}

			size_t i = 0;
			for (const TypeInfo *typeInfo : p.second) {
				GuardGuard g(file, typeInfo->mBinary);
				while (typeInfo) {
					file << "    template<> size_t component_index<" << typeInfo->mFullName << ">(){ return -1; }\n";
					typeInfo = typeInfo->mDecayType;
				}
			}


			file << "\n";
		}

		file << "}";
	}

	MADGINE_BASE_EXPORT std::map<std::string, ComponentRegistryBase*>& registryRegistry()
	{
		static std::map<std::string, ComponentRegistryBase *>  dummy;
		return dummy;
	}


}

#endif