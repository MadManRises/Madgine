from Engine import Vector3, Behavior, Contextual, Duration
from Engine.Scene.Entity import EntityDescriptor, Transform, Mesh, Entity
from Engine.Scene import SceneContainer
from Engine.Physics import RigidBody, CollisionShapeManager
from Engine.Render import GPUMeshLoader

from Behaviors.Native import Wait_Simulation, Translate
from Behaviors.NodeGraph import Bullet

@Behavior
async def TankShooting(entity : Contextual[Entity], scene : Contextual[SceneContainer]):
    while True:
        await Wait_Simulation(Duration(3000000000))
        pos = entity.Transform.WorldPosition
        pos.y = pos.y + 1.4
        scene.createEntity("Bullet", EntityDescriptor(
	        Transform(
                  mPosition=pos,
                  mScale=Vector3(0.2, 0.2, 0.2),
                  mOrientation=entity.Transform.WorldOrientation
		    ),
		    Mesh(
			      mMesh=GPUMeshLoader.Resource("Bullet")
		    ),
            RigidBody(
                Kinematic=True,
                Shape=CollisionShapeManager.Resource("Cube")
            ),
            Translate(entity.Transform.WorldOrientation * Vector3(0, 0, 1.0), 10.0),
            Bullet()
        ))