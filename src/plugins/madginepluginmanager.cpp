#include "../baselib.h"

#include "madginepluginmanager.h"

#include <iostream>

namespace Engine {
	namespace Plugins {

		MadginePluginManager::MadginePluginManager()
		{
			Plugin base(MADGINE_BASE_BINARY_NAME);
			base.load();
			std::experimental::filesystem::path binDir = base.fullPath().parent_path();

			std::experimental::filesystem::path fileName = "plugins" CONFIG_SUFFIX ".txt";

			std::ifstream pluginsFile(binDir / fileName);

			std::string line;
			while (std::getline(pluginsFile, line))
			{
				int i = line.find_first_of('=');
				std::string name = line.substr(0, i);
				std::string fileName = line.substr(i + 1);

				addPlugin(name, fileName);
			}

		}

	}
}
