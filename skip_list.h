#pragma once

#include "node.h"
#include <optional>
#include <vector>

using std::vector, std::optional;

class SkipList {
public:
    SkipList();

    void insert(int key, any value);
    void remove(int key);
    // get range of [key1, key2)
    auto get_range(int key1, int key2) const -> vector<any>;

    auto get(int key) const -> optional<any>;
    auto size() const -> size_t;

#ifdef UNIT_TEST
public:
#else
private:
#endif
    static auto search_line(int key, shared_ptr<Node> head) -> shared_ptr<Node>;
    static auto search_element(int key, shared_ptr<Node> node) -> shared_ptr<Node>;
    static auto search_path(int key, shared_ptr<Node> node) -> vector<shared_ptr<Node>>;
    static auto make_dummy_head() -> shared_ptr<Node>;
    static auto random_level(int max_level) -> int;

    void insert(int key, any value, int level);

    vector<shared_ptr<Node>> heads_;
    size_t nnode_;

};
