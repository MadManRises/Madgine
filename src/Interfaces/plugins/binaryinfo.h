namespace Engine {
	namespace Plugins {

		struct BinaryInfo {

			size_t mMajorVersion;
			size_t mMinorVersion;
			size_t mPathNumber;

			const char *mSourceRoot;
			const char *mBinaryDir;
			const char *mBinaryName;

		};

		//extern "C" extern BinaryInfo binaryInfo;

	}
}