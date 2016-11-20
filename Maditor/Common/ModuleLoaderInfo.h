#pragma once



		enum ModuleLoaderCmd {
			Load,
			Unload,
			Init
		};

		struct ModuleLoaderMsg {
			ModuleLoaderCmd mCmd;
			char mArg[256];
			bool mCallInit;
		};

