#include "../moduleslib.h"

#include "workgroupstorage.h"
#include "workgroup.h"

namespace Engine {
namespace Threading {

    static std::vector<std::pair<std::function<Any()>, std::vector<Any>>> &sWorkgroupLocalBssConstructors()
    {
        static std::vector<std::pair<std::function<Any()>, std::vector<Any>>> dummy;
        return dummy;
    }

    static std::vector<std::pair<std::function<Any()>, std::vector<Any>>> &sWorkgroupLocalObjectConstructors()
    {
        static std::vector<std::pair<std::function<Any()>, std::vector<Any>>> dummy;
        return dummy;
    }

    int WorkGroupStorage::registerLocalBssVariable(std::function<Any()> ctor)
    {
        sWorkgroupLocalBssConstructors().emplace_back(std::move(ctor), std::vector<Any> {});
        return -static_cast<int>(sWorkgroupLocalBssConstructors().size());
    }

    void WorkGroupStorage::unregisterLocalBssVariable(int index)
    {
        sWorkgroupLocalBssConstructors()[-(index + 1)] = {};
    }

    int WorkGroupStorage::registerLocalObjectVariable(std::function<Any()> ctor)
    {
        sWorkgroupLocalObjectConstructors().emplace_back(std::move(ctor), std::vector<Any> {});
        return sWorkgroupLocalObjectConstructors().size() - 1;
    }

    void WorkGroupStorage::unregisterLocalObjectVariable(int index)
    {
        sWorkgroupLocalObjectConstructors()[index] = {};
    }

    const Any &WorkGroupStorage::localVariable(int index)
    {
        size_t selfIndex = WorkGroup::self().mInstanceCounter;
        std::vector<std::pair<std::function<Any()>, std::vector<Any>>> &constructors = index < 0 ? sWorkgroupLocalBssConstructors() : sWorkgroupLocalObjectConstructors();
        if (index < 0)
            index = -(index + 1);
        std::pair<std::function<Any()>, std::vector<Any>> &p = constructors[index];
        if (p.second.size() <= selfIndex)
            p.second.resize(selfIndex + 1);
        if (!p.second[selfIndex])
            p.second[selfIndex] = p.first();
        return p.second.at(selfIndex);
    }

    void WorkGroupStorage::init(bool bss)
    {
        size_t selfIndex = WorkGroup::self().mInstanceCounter;
        if (bss) {
            for (std::pair<std::function<Any()>, std::vector<Any>> &p : sWorkgroupLocalBssConstructors()) {
                if (p.second.size() <= selfIndex)
                    p.second.resize(selfIndex + 1);
                if (!p.second[selfIndex])
                    p.second[selfIndex] = p.first();
            }
        } else {
            for (std::pair<std::function<Any()>, std::vector<Any>> &p : sWorkgroupLocalObjectConstructors()) {
                if (p.second.size() <= selfIndex)
                    p.second.resize(selfIndex + 1);
                if (!p.second[selfIndex])
                    p.second[selfIndex] = p.first();
            }
        }
    }

	void WorkGroupStorage::finalize(bool bss)
    {
        size_t selfIndex = WorkGroup::self().mInstanceCounter;
        if (bss) {
            for (std::pair<std::function<Any()>, std::vector<Any>> &p : sWorkgroupLocalBssConstructors()) {
                if (p.second.size() > selfIndex)
                    p.second[selfIndex] = {};
            }
        } else {
            for (std::pair<std::function<Any()>, std::vector<Any>> &p : sWorkgroupLocalObjectConstructors()) {
                if (p.second.size() > selfIndex)
                    p.second[selfIndex] = {};
            }
        }
    }

}
}