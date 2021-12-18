#pragma once

#include "madgineobject/madgineobject.h"

#include "Meta/keyvalue/virtualscope.h"

#include "Meta/serialize/virtualserializableunit.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace Tools {

    struct MADGINE_TOOLS_EXPORT ToolBase : Serialize::VirtualSerializableUnitBase<VirtualScopeBase<>, Serialize::SerializableUnitBase>, MadgineObject<ToolBase> {
        SERIALIZABLEUNIT(ToolBase);

        ToolBase(ImRoot &root);
        virtual ~ToolBase() = default;

        virtual void render();
        virtual void renderMenu();
        virtual void renderStatus();
        virtual void update();

        virtual std::string_view key() const = 0;

        bool isVisible();
        void setVisible(bool v);

        ToolBase &getToolComponent(size_t index);
        template <typename T>
        T &getTool()
        {
            return static_cast<T &>(getToolComponent(component_index<T>()));
        }

        ImRoot &root();

        Threading::TaskQueue *taskQueue() const;

    protected:
        virtual bool init();
        virtual void finalize();
        friend struct MadgineObject<ToolBase>;

        bool mVisible = false;

        ImRoot &mRoot;
    };

}
}

RegisterType(Engine::Tools::ToolBase);