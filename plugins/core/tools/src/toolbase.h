#pragma once

#include "Modules/madgineobject/madgineobject.h"

#include "Modules/keyvalue/virtualscopebase.h"

#include "Modules/serialize/serializableunit.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace Tools {

    struct MADGINE_TOOLS_EXPORT ToolBase : public MadgineObject, public VirtualScopeBase, public Serialize::SerializableUnit<ToolBase> {
        SERIALIZABLEUNIT;

        ToolBase (ImRoot &root);
        virtual ~ToolBase() = default;

        virtual void render();
        virtual void renderMenu();
        virtual void update();

        virtual std::string_view key() const = 0;

        bool isVisible();
        void setVisible(bool v);

        ToolBase &getToolComponent(size_t index, bool = true);
        template <typename T>
        T &getTool()
        {
            return static_cast<T &>(getToolComponent(component_index<T>()));
        }
        ToolBase &getSelf(bool = true);
		
		//virtual App::Application &app(bool = true) override;

    protected:

        virtual const MadgineObject *parent() const override;
        virtual bool init() override;
        virtual void finalize() override;

		bool mVisible = false;
        
        ImRoot &mRoot;
    };

}
}

RegisterType(Engine::Tools::ToolBase);