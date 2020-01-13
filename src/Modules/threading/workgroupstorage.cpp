#include "../moduleslib.h"


#if ENABLE_THREADING

#include "workgroupstorage.h"
#include "workgroup.h"
#include "globalvariablemanager.h"


namespace Engine {
namespace Threading {

    static std::vector<GlobalVariableManager> &sWorkgroupLocalBssConstructors()
    {
        static std::vector<GlobalVariableManager> dummy;
        return dummy;
    }

    static std::vector<GlobalVariableManager> &sWorkgroupLocalObjectConstructors()
    {
        static std::vector<GlobalVariableManager> dummy;
        return dummy;
    }

    int WorkGroupStorage::registerLocalBssVariable(std::function<Any()> ctor)
    {
        sWorkgroupLocalBssConstructors().emplace_back(std::move(ctor));
        return -static_cast<int>(sWorkgroupLocalBssConstructors().size());
    }

    void WorkGroupStorage::unregisterLocalBssVariable(int index)
    {
        sWorkgroupLocalBssConstructors()[-(index + 1)].reset();
    }

    int WorkGroupStorage::registerLocalObjectVariable(std::function<Any()> ctor)
    {
        sWorkgroupLocalObjectConstructors().emplace_back(std::move(ctor));
        return sWorkgroupLocalObjectConstructors().size() - 1;
    }

    void WorkGroupStorage::unregisterLocalObjectVariable(int index)
    {
        sWorkgroupLocalObjectConstructors()[index].reset();
    }

    const Any &WorkGroupStorage::localVariable(int index)
    {
        size_t selfIndex = WorkGroup::self().mInstanceCounter;
        std::vector<GlobalVariableManager> &constructors = index < 0 ? sWorkgroupLocalBssConstructors() : sWorkgroupLocalObjectConstructors();
        if (index < 0)
            index = -(index + 1);
        GlobalVariableManager &m = constructors[index];
        return m[selfIndex];
    }

    void WorkGroupStorage::init(bool bss)
    {
        size_t selfIndex = WorkGroup::self().mInstanceCounter;
        if (bss) {
            for (GlobalVariableManager &m : sWorkgroupLocalBssConstructors()) {
                m[selfIndex];
            }
        } else {
            for (GlobalVariableManager &m : sWorkgroupLocalObjectConstructors()) {
                m[selfIndex];
            }
        }
    }

	void WorkGroupStorage::finalize(bool bss)
    {
        size_t selfIndex = WorkGroup::self().mInstanceCounter;
        if (bss) {
            for (GlobalVariableManager &m : sWorkgroupLocalBssConstructors()) {
                m.remove(selfIndex);
            }
        } else {
            for (GlobalVariableManager &m : sWorkgroupLocalObjectConstructors()) {
                m.remove(selfIndex);
            }
        }
    }

}
}

#endif