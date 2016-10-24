#pragma once

#include <string>


namespace Maditor {
	namespace Model {

		class ProjectLog;

		namespace Generator {
			class CommandLine {
			public:
				static int exec(const char *cmd);

				static void setLog(ProjectLog *log);

			private:
				static ProjectLog *sLog;
			};
		}
	}
}