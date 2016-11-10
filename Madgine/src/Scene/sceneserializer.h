#pragma once


namespace Engine {
namespace Scene {

class MADGINE_EXPORT SceneSerializer
{
public:
    SceneSerializer() = delete;

	static void storeCurrentScene(Scripting::Serialize::SerializeOutStream &out, bool storeComponents, const std::set<Scripting::Scope*> &ignoreSet = {});
    static void restoreScene(Scripting::Serialize::SerializeInStream &in, bool callInit = false);

};

} // namespace Scripting
} // namespace Core


