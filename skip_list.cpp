//
// Created by 林思源 on 24-5-30.
//

#include <memory>
#include <random>
#include "skip_list.h"

SkipList::SkipList() {
    nnode_ = 0;
}

auto SkipList::make_dummy_head() -> shared_ptr<Node> {
    return std::make_shared<Node>(INT_MIN, 0);
}

auto SkipList::random_level(const int max_level) -> int {
    int level = 1;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dist(0.0, 1.0);

    while(dist(gen) < 0.5 && level < max_level) {
        level++;
    }

    return level;
}

auto SkipList::search_line(const int key, shared_ptr<Node> head) -> shared_ptr<Node> {
    if(!head->next) {
        return head;
    }
    while(head->next) {
        if(head->next->key > key && head->key <= key) {
            break;
        }
        head = head->next;
    }
    return head;
}

auto SkipList::search_element(const int key, shared_ptr<Node> node) -> shared_ptr<Node> {
    while(true) {
        node = search_line(key, node);
        if(!node->down) {
            break;
        }
        node = node->down;
    }
    return node;
}

auto SkipList::search_path(const int key, shared_ptr<Node> node) -> vector<shared_ptr<Node>> {
    vector<shared_ptr<Node>> path;
    while(node->down) {
        node = search_line(key, node);
        path.push_back(node);
        node = node->down;
    }
    node = search_line(key, node);
    path.push_back(node);
    return path;
}

void SkipList::remove(int key) {
    nnode_--;
    auto path = search_path(key, heads_[0]);
    if(path.back()->key != key) {
        return;
    }
    for(auto node : path) {
        if(node->key == key) {
            node->prev->next = node->next;
            if(node->next) {
                node->next->prev = node->prev;
            }
        }
    }
}

auto SkipList::get(const int key) const -> std::optional<any> {
    if(const auto node = search_element(key, heads_[0]); node->key == key) {
        return node->value;
    } else {
        return std::nullopt;
    }
}

auto SkipList::get_range(const int key1, const int key2) const -> vector<any> {
    const auto begin = search_element(key1, heads_[0]);
    const auto end = search_element(key2, heads_[0]);
    vector<any> values;
    for(auto ele = begin; ele != end->next; ele = ele->next) {
        if(ele->key >= key1 && ele->key < key2) {
            values.push_back(ele->value);
        }
    }
    return values;
}

auto SkipList::size() const -> size_t {
    return nnode_;
}

void SkipList::insert(int key, any value, int level) {
    while(level > heads_.size()) {
        heads_.insert(heads_.begin(), make_dummy_head());
        if(heads_.size() > 1) {
            heads_[0]->down = heads_[1];
        }
    }
    auto path = search_path(key, heads_[0]);
    if(path.back()->key == key) {
        path.back()->value = value;
        return;
    }

    nnode_++;
    std::shared_ptr<Node> last_layer;
    for(int i = path.size() - 1; i >= static_cast<int>(path.size()) - level; i--) {
        auto node = std::make_shared<Node>(key, value);
        if(path[i]->next) {
            path[i]->next->prev = node;
        }
        node->next = path[i]->next;
        node->prev = path[i];
        path[i]->next = node;
        if(last_layer) {
            node->down = last_layer;
        }
        last_layer = node;
    }
}

void SkipList::insert(int key, any value) {
    int max_level = static_cast<int>(log(static_cast<double>(nnode_)) / log(2.0)) + 1;
    int level = random_level(max_level);
    insert(key, value, level);
}
