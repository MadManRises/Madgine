#pragma once

namespace Engine {

template <typename T>
struct UnmanagedSinglyLinkedNode {
    UnmanagedSinglyLinkedNode() = default;
    UnmanagedSinglyLinkedNode(const UnmanagedSinglyLinkedNode &) = delete;
    UnmanagedSinglyLinkedNode(UnmanagedSinglyLinkedNode &&) = delete;
    UnmanagedSinglyLinkedNode &operator=(UnmanagedSinglyLinkedNode &&) = delete;
    UnmanagedSinglyLinkedNode &operator=(const UnmanagedSinglyLinkedNode &) = delete;

    std::atomic<T *> mNext = nullptr;
};

template <typename T>
struct UnmanagedDoublyLinkedNode : UnmanagedSinglyLinkedNode<T> {
    UnmanagedDoublyLinkedNode() = default;
    UnmanagedDoublyLinkedNode(const UnmanagedDoublyLinkedNode &) = delete;
    UnmanagedDoublyLinkedNode(UnmanagedDoublyLinkedNode &&) = delete;
    UnmanagedDoublyLinkedNode &operator=(UnmanagedDoublyLinkedNode &&) = delete;
    UnmanagedDoublyLinkedNode &operator=(const UnmanagedDoublyLinkedNode &) = delete;

    std::atomic<T *> mPrev = nullptr;
};

}