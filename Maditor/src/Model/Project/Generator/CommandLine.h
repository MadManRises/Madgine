#pragma once


namespace Maditor {
	namespace Model {
		namespace Generator {

			class CommandLine {
			public:
				static int exec(const char *cmd);

				static void setLog(Watcher::OgreLogWatcher *log);

			private:
				static Watcher::OgreLogWatcher *sLog;
			};
		}
	}
}