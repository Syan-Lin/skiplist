#pragma once

#include <memory>
#include <any>

using std::shared_ptr, std::any;

class Node {
public:
    Node() = default;
    explicit Node(const int key, const any value) : key(key), value(value) {}

    shared_ptr<Node> next;
    shared_ptr<Node> prev;
    shared_ptr<Node> down;
    int key;
    any value;
};
