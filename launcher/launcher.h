#pragma once

#include <functional>

namespace Engine::Window {
struct MainWindow;
}

int launch(std::function<void(Engine::Window::MainWindow &)> callback = {});