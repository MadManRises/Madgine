#pragma once

#include <string>


namespace Maditor {
	namespace Model {
		namespace Generator {
			class CommandLine {
			public:
				static int exec(const char *cmd);
			};
		}
	}
}