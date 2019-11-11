

Project(ClickBrick)

set(sources "")

FolderSources(sources "" src brick.cpp brick.h mainmenuhandler.cpp mainmenuhandler.h gamehandler.cpp gamehandler.h gamemanager.cpp gamemanager.h gameoverhandler.cpp gameoverhandler.h)	

add_plugin(UI ClickBrick Game ${sources})

target_link_plugins(UI Client SceneRenderer)

target_link_libraries(UI PUBLIC Modules)