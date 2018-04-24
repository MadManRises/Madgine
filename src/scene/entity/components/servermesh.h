#pragma once

#include "../entitycomponent.h"
#include "mesh.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			class MADGINE_SERVER_EXPORT ServerMesh : public EntityComponentVirtualImpl<ServerMesh, Mesh>
			{
			public:
				ServerMesh(Entity& entity, const Scripting::LuaTable& table);
				ServerMesh(Entity& entity, const std::string& meshName);
				virtual ~ServerMesh();

				void init() override;


				Vector3 getCenter() const;

				virtual std::string getName() const override;
				virtual void setName(const std::string &name) override;

				virtual void setVisible(bool b) override;
				virtual bool isVisible() const override;


			private:
				Transform* mTransform;
				std::string mMeshName;
			};
		}
	}
}
