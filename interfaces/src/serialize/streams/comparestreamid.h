#pragma once

namespace Engine
{
	namespace Serialize
	{
		
		struct CompareStreamId
		{
			bool operator()(Stream* first, Stream* second) const;
			bool operator()(BufferedOutStream* first, BufferedOutStream* second) const;
		};
	}
}