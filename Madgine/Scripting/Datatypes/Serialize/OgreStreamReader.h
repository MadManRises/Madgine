#pragma once


namespace Engine {
	namespace Scripting {
		namespace Serialize {
			class MADGINE_EXPORT OgreStreamReader : public std::basic_streambuf<char>{
			public:
				OgreStreamReader(Ogre::DataStreamPtr stream);

			protected:
				virtual std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override;
				virtual std::streampos seekpos(std::streampos pos, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override;
			private:
				Ogre::MemoryDataStream *mPtr;
			};
		}
	}
}