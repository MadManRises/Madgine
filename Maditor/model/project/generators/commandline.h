#pragma once


namespace Maditor {
	namespace Model {
		namespace Generators {

			class CommandLine {
			public:
				static std::pair<int, std::string> exec(std::string cmd);

				//static void setLog(Watcher::OgreLogWatcher *log);

			private:
				//static Watcher::OgreLogWatcher *sLog;
			};
		}
	}
}