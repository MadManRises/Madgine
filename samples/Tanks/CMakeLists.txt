if (NOT TARGET Bullet OR NOT TARGET Audio)
	return()
endif()

Project(Tanks)

set(sources "")

FolderSources(sources "" src tankbehaviors.cpp)	

add_plugin(Tanks Madgine Sample ${sources})

target_link_libraries(Tanks PUBLIC Modules)
target_link_libraries(Tanks PRIVATE BulletDynamics BulletCollision BulletSoftBody LinearMath)


target_link_plugins(Tanks Scene Bullet Audio)