#pragma once

template <typename T>
void logContainer(const T &c) {
    std::stringstream ss;
    bool first = true;
    for (const auto &i : c) {
        if (first) {
            first = false;
        } else {
            ss << ", ";
        }
        ss << i;
    }
    LOG(ss.str());
}