#pragma once

#include "Modules/madgineobject/madgineobject.h"

#include "Modules/keyvalueutil/virtualscopebase.h"

#include "Modules/serialize/serializableunit.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace Tools {

    struct MADGINE_TOOLS_EXPORT ToolBase : public VirtualScopeBase<>, Serialize::SerializableUnit<ToolBase>, MadgineObject<ToolBase> {
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
		
		//virtual App::Application &app(bool = true) override;

        const ImRoot *parent() const;

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