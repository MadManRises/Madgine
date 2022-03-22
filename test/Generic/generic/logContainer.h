#pragma once

template <typename T>
void logContainer(const T &c) {
    Engine::Util::LogDummy ss { Engine::Util::MessageType::INFO_TYPE };
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