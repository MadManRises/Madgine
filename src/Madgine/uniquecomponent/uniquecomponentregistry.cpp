#include "../baselib.h"

#ifndef STATIC_BUILD

#include "uniquecomponentregistry.h"

#include "Interfaces/plugins/pluginmanager.h"


namespace Engine {

	CollectorRegistry collectorRegistry{};

	struct CompareTypeInfo {
		bool operator()(const TypeInfo *t1, const TypeInfo *t2) const {
			return strcmp(t1->mFullName, t2->mFullName) == -1;
		}
	};

	void exportStaticComponentHeader(const std::experimental::filesystem::path &outFile, std::vector<const TypeInfo*> skip) {
		std::map<const TypeInfo *, std::vector<const TypeInfo*>, CompareTypeInfo> collectorData;

		auto notInSkip = [&](const TypeInfo *v) {
			return std::find_if(skip.begin(), skip.end(), [=](const TypeInfo *v2) {return strcmp(v->mFullName, v2->mFullName) == 0; }) == skip.end();
		};

		for (const std::pair<const std::string, ComponentRegistryBase*> &registry : registryRegistry()) {
			collectorData[registry.second->type_info()];
		}

		for (CollectorInfo *info : collectorRegistry.mInfos) {
			std::vector<const TypeInfo*> &v = collectorData[info->mRegistryInfo];
			v.insert(v.end(), info->mElementInfos.begin(), info->mElementInfos.end());
		}

		for (const std::pair<const std::string, Plugins::PluginSection> &sec : Plugins::PluginManager::getSingleton()) {
			for (const std::pair<const std::string, Plugins::Plugin> &p : sec.second) {
				if (p.second.isLoaded()) {
					auto reg = (CollectorRegistry*)p.second.getSymbol("collectorRegistry");
					if (reg) {
						for (CollectorInfo *info : reg->mInfos) {
							if (notInSkip(info->mBaseInfo)) {
								std::vector<const TypeInfo*> &v = collectorData[info->mRegistryInfo];
								std::copy_if(info->mElementInfos.begin(), info->mElementInfos.end(), std::back_inserter(v), notInSkip);
							}
						}
					}
				}
			}
		}

		std::ofstream file(outFile);
		assert(file);
		file << R"(
#include "Madgine/baselib.h"

#include "toolslib.h"
#include "oislib.h"
#include "OpenGL/opengllib.h"

)";

		for (const std::pair<const TypeInfo* const, std::vector<const TypeInfo*>> &p : collectorData) {
			file << "#include \"" << p.first->mHeaderPath << "\"\n";

			for (const TypeInfo *info : p.second) {
				file << "#include \"" << info->mHeaderPath << "\"\n";
			}
		}

		file << R"(

namespace Engine{

)";

		for (const std::pair<const TypeInfo* const, std::vector<const TypeInfo*>> &p : collectorData) {
			file << "	std::vector<" << p.first->mFullName << "::F> " << p.first->mFullName << "::sComponents() { return {\n";

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


}

#endif