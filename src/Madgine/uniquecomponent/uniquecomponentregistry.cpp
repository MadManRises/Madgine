#include "../baselib.h"

#ifndef STATIC_BUILD

#include "uniquecomponentregistry.h"

#include "Interfaces/plugins/pluginmanager.h"

#include "Interfaces/plugins/binaryinfo.h"

#include "Interfaces/util/pathutil.h"


namespace Engine {

	struct CompareTypeInfo {
		bool operator()(const TypeInfo *t1, const TypeInfo *t2) const {
			return strcmp(t1->mFullName, t2->mFullName) == -1;
		}
	};

	void exportStaticComponentHeader(const std::experimental::filesystem::path &outFile, std::vector<const TypeInfo*> skip) {
		std::map<const TypeInfo *, std::vector<const TypeInfo*>, CompareTypeInfo> collectorData;
		std::set<const Plugins::BinaryInfo *> binaries{ &Plugins::binaryInfo_Base };

		auto notInSkip = [&](const TypeInfo *v) {
			return std::find_if(skip.begin(), skip.end(), [=](const TypeInfo *v2) {return strcmp(v->mFullName, v2->mFullName) == 0; }) == skip.end();
		};

		for (const std::pair<const std::string, ComponentRegistryBase*> &registry : registryRegistry()) {
			collectorData[registry.second->type_info()];			
		}

		for (CollectorInfo *info : collectorRegistry_Base()->mInfos) {
			auto &v = collectorData[info->mRegistryInfo];
			v.insert(v.end(), info->mElementInfos.begin(), info->mElementInfos.end());			
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
			if (strlen(bin->mPrecompiledHeaderPath))
				file << "#include \"" << bin->mPrecompiledHeaderPath << "\"\n";

		auto fixInclude = [&](const char *pStr) {
			std::experimental::filesystem::path p = PathUtil::make_case_sensitive(pStr);
			for (const Plugins::BinaryInfo *binInfo : binaries)
				if (!PathUtil::relative(p, binInfo->mSourceRoot).empty())
					return PathUtil::relative(p, binInfo->mSourceRoot);
			return p;
		};

		for (const std::pair<const TypeInfo* const, std::vector<const TypeInfo*>> &p : collectorData) {
			file << "#include \"" << fixInclude(p.first->mHeaderPath) << "\"\n";

			for (const TypeInfo *info : p.second) {
				file << "#include \"" << fixInclude(info->mHeaderPath) << "\"\n";
			}
		}

		file << R"(

namespace Engine{

)";

		for (const std::pair<const TypeInfo* const, std::vector<const TypeInfo*>> &p : collectorData) {
			file << "	template<> std::vector<" << p.first->mFullName << "::F> " << p.first->mFullName << "::sComponents() { return {\n";

			bool first = true;

			for (const TypeInfo *info : p.second) {
				if (first) {
					first = false;
				}
				else {
					file << ",\n";
				}
				file << "		createComponent<" << info->mFullName << ">";
			}

			file << "\n	}; }\n\n";

			size_t i = 0;
			for (const TypeInfo *info : p.second) {
				while (info) {
					file << "    template<> size_t component_index<" << info->mFullName << ">(){ return " << i << "; }\n";
					info = info->mDecayType;
				}
				++i;
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