#pragma once

namespace Engine
{
	namespace Util
	{
		struct TraceBack
		{
			TraceBack(std::string func = "????????", std::string file = "", int line = -1) :
				mFile(std::move(file)),
				mLineNr(line),
				mFunction(std::move(func))
			{
			}

			std::string mFile;
			int mLineNr;
			std::string mFunction;
		};

		std::ostream &operator <<(std::ostream &out, const TraceBack &trace);
	}
}
