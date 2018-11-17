#include "../../../baselib.h"

#include "mesh.h"

#include "Interfaces/scripting/types/api.h"

#include "Interfaces/generic/keyvalueiterate.h"



namespace Engine
{




	namespace Scene
	{
		namespace Entity
		{
			
			ENTITYCOMPONENTVIRTUALBASE_IMPL(Mesh, Mesh);

			KeyValueMapList Mesh::maps()
			{
				return Scope::maps().merge(MAP(Name, getName, setName), MAP(Visible, isVisible, setVisible));
			}
		}
	}
}
