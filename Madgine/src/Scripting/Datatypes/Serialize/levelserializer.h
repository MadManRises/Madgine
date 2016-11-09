#pragma once


namespace Engine {
namespace Scripting {
namespace Serialize {

class MADGINE_EXPORT LevelSerializer
{
public:
    LevelSerializer() = delete;

	static void storeCurrentLevel(SerializeOutStream &out, bool storeComponents, const std::set<Scope*> &ignoreSet = {});
    static void restoreLevel(SerializeInStream &in, bool callInit = false);

};

}
} // namespace Scripting
} // namespace Core


