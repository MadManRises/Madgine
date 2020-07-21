
Project(Bullet)



set(sources "")
FolderSources(sources "" src)
	FolderSources(sources src Madgine)
		FolderSources(sources Madgine physics physicsmanager.cpp physicsmanager.h rigidbody.cpp rigidbody.h)


add_plugin(Bullet Madgine Physics ${sources} PRECOMPILED_HEADER src/Madgine/bulletlib.h)

target_link_plugins(Bullet Scene)

target_link_libraries(Bullet PRIVATE BulletCollision BulletDynamics LinearMath)

if (BUILD_TESTING)
	add_subdirectory(test)
endif()