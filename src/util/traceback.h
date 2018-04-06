#pragma once

namespace Engine
{
	namespace Util
	{
		struct TraceBack
		{
			TraceBack(const std::string& file = "<unknown>", int line = -1, const std::string& func = "") :
				mFile(file),
				mLineNr(line),
				mFunction(func)
			{
			}

			std::string mFile;
			int mLineNr;
			std::string mFunction;
		};
	}
}
