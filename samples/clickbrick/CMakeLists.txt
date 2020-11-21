

Project(ClickBrick)

set(sources "")

FolderSources(sources "" src brick.cpp brick.h mainmenuhandler.cpp mainmenuhandler.h gamehandler.cpp gamehandler.h gamemanager.cpp gamemanager.h gameoverhandler.cpp gameoverhandler.h)	

add_plugin(ClickBrick ClickBrick Game ${sources} PRECOMPILED_HEADER src/clickbricklib.h)

target_link_plugins(ClickBrick UI SceneRenderer ClientTools)

target_link_libraries(ClickBrick PRIVATE Im3D)