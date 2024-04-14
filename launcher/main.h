#pragma once

#include <functional>

namespace Engine{
namespace Window {
    struct MainWindow;
}
namespace App {
    struct Application;
}
}

int desktopMain(int argc, char **argv, std::function<void(Engine::App::Application &, Engine::Window::MainWindow &)> callback = {});