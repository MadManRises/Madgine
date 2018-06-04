#pragma once

#include "VSCommands.cs"
#include <string>

struct VSMsg {
	VSMsg(VSCommands::VSCommand cmd = (VSCommands::VSCommand)0, const std::string &arg1 = "", int64_t arg2 = 0) :
		mCmd(cmd),
		mArg2(arg2) {
		strcpy_s(mArg1, arg1.c_str());
	}

	VSCommands::VSCommand mCmd;
	char mArg1[512];
	int64_t mArg2;
};