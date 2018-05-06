#pragma once

#include "../entitycomponent.h"
#include "mesh.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			class MADGINE_CLIENT_EXPORT OgreMesh : public EntityComponentVirtualImpl<OgreMesh, Mesh>
			{
			public:
				OgreMesh(Entity& entity, const Scripting::LuaTable& table);
				OgreMesh(Entity& entity, const std::string& meshName = "");
				virtual ~OgreMesh();

				void init() override;
				void finalize() override;


				Ogre::Entity* getMesh() const;

				Vector3 getCenter() const;

				virtual std::string getName() const override;
				virtual void setName(const std::string& mesh) override;

				virtual bool isVisible() const override;
				virtual void setVisible(bool b) override;

				void destroyObject();

			private:
				OgreTransform* mTransform;
				Ogre::Entity* mObject;
			};
		}
	}
}
