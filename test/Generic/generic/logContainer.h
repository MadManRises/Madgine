#pragma once

template <typename T>
void logContainer(const T &c) {
    Engine::Log::LogDummy ss { Engine::Log::MessageType::INFO_TYPE };
    bool first = true;
    for (const auto &i : c) {
        if (first) {
            first = false;
        } else {
            ss << ", ";
        }
        ss << i;
    }
}