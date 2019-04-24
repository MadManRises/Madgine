#pragma once

namespace Engine
{
	namespace Serialize
	{
		
		struct CompareStreamId
		{
			bool operator()(SerializeInStream* first, SerializeInStream* second) const;
			bool operator()(SerializeOutStream* first, SerializeOutStream* second) const;
			bool operator()(BufferedInStream* first, BufferedInStream* second) const;
			bool operator()(BufferedOutStream* first, BufferedOutStream* second) const;
			bool operator()(const BufferedInOutStream& first, const BufferedOutStream& second) const;
		};
	}
}