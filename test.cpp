#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_COLORS_ANSI
#define UNIT_TEST

#include <doctest/doctest.h>
#include <unordered_map>
#include <memory>
#include <cmath>
#include <iostream>

#include "node.h"
#include "skip_list.h"

using namespace std;

TEST_CASE("random_test") {

    const double DELTA     = 0.02;
    const int    MAX_LEVEL = 10;
    const int    N         = 10000;

    unordered_map<int, int> count;
    for(int i = 0; i < N; i++) {
        int num = SkipList::random_level(MAX_LEVEL);
        count[num]++;
    }
    for(int i = 1; i <= MAX_LEVEL; i++) {
        double true_prob = 1.0 / pow(2, i);
        double prob = count[i] * 1.0 / N;
        CHECK(abs(true_prob - prob) < DELTA);
    }
}

void make_chain(vector<shared_ptr<Node>> chain) {
    for(int i = 0; i < chain.size() - 1; i++) {
        chain[i]->next = chain[i + 1];
        chain[i + 1]->prev = chain[i];
    }
}

template<typename T>
bool check_vec(vector<T> v1, vector<T> v2) {
    if(v1.size() != v2.size()) {
        return false;
    }
    for(int i = 0; i < v1.size(); i++) {
        if(v1[i] != v2[i]) {
            return false;
        }
    }
    return true;
}

template<>
bool check_vec(vector<any> v1, vector<any> v2) {
    if(v1.size() != v2.size()) {
        return false;
    }
    for(int i = 0; i < v1.size(); i++) {
        if(any_cast<int>(v1[i]) != any_cast<int>(v2[i])) {
            return false;
        }
    }
    return true;
}

bool check_layer(shared_ptr<Node> head, vector<vector<shared_ptr<Node>>> layers) {
    auto check_line = [&layers](shared_ptr<Node> head, int n) {
        auto node = head;
        for(int i = 0; i < layers[n].size(); i++) {
            if(!node || node->key != layers[n][i]->key) {
                return false;
            }
            node = node->next;
        }
        if(node) {
            return false;
        }
        return true;
    };
    int n_layer = 0;
    while(head->down) {
        if(!check_line(head, n_layer)) {
            return false;
        }
        head = head->down;
        n_layer++;
    }
    if(!check_line(head, n_layer)) {
        return false;
    }
    return true;
}

bool check_key(shared_ptr<Node> head, vector<vector<int>> keys) {
    auto check_line = [&keys](shared_ptr<Node> head, int n) {
        auto node = head;
        for(int i = 0; i < keys[n].size(); i++) {
            if(!node || node->key != keys[n][i]) {
                return false;
            }
            node = node->next;
        }
        if(node) {
            return false;
        }
        return true;
    };
    int n_layer = 0;
    while(head->down) {
        if(!check_line(head, n_layer)) {
            return false;
        }
        head = head->down;
        n_layer++;
    }
    if(!check_line(head, n_layer)) {
        return false;
    }
    return true;
}

void print_layers(shared_ptr<Node> head) {
    while(head) {
        auto node = head;
        while(node) {
            cout << node->key << " ";
            node = node->next;
        }
        cout << endl;
        head = head->down;
    }
    cout << endl;
}

TEST_CASE("search_test") {
    // layer1: 1 3 4
    auto layer1 = SkipList::make_dummy_head();
    auto n1 = make_shared<Node>(1, 1);
    auto n3 = make_shared<Node>(3, 3);
    auto n4 = make_shared<Node>(4, 4);
    make_chain({layer1, n1, n3, n4});

    // layer2: 0 1 2 3 4 5
    auto layer2 = SkipList::make_dummy_head();
    auto nn0 = make_shared<Node>(0, 0);
    auto nn1 = make_shared<Node>(1, 1);
    auto nn2 = make_shared<Node>(2, 2);
    auto nn3 = make_shared<Node>(3, 3);
    auto nn4 = make_shared<Node>(4, 4);
    auto nn5 = make_shared<Node>(5, 5);
    make_chain({layer2, nn0, nn1, nn2, nn3, nn4, nn5});
    layer1->down = layer2;
    n1->down = nn1;
    n3->down = nn3;
    n4->down = nn4;

    // search_line layer1: 1 3 4
    // 0 -> head
    // 1 -> 1
    // 2 -> 1
    // 3 -> 3
    // 16 -> 4
    CHECK(SkipList::search_line(0, layer1)->key == layer1->key);
    CHECK(SkipList::search_line(1, layer1)->key == n1->key);
    CHECK(SkipList::search_line(2, layer1)->key == n1->key);
    CHECK(SkipList::search_line(3, layer1)->key == n3->key);
    CHECK(SkipList::search_line(16, layer1)->key == n4->key);

    // search_element
    // layer1:   1   3 4
    // layer2: 0 1 2 3 4 5
    // -1 -> head
    // 0 -> 0
    // 1 -> 1
    // 2 -> 2
    // 10 -> 5
    CHECK(SkipList::search_element(-1, layer1)->key == layer2->key);
    CHECK(SkipList::search_element(0, layer1)->key == nn0->key);
    CHECK(SkipList::search_element(1, layer1)->key == nn1->key);
    CHECK(SkipList::search_element(2, layer1)->key == nn2->key);
    CHECK(SkipList::search_element(10, layer1)->key == nn5->key);

    // search_path
    // layer1:   1   3 4
    // layer2: 0 1 2 3 4 5
    // -1 -> [head, head]
    // 0 -> [head, 0]
    // 1 -> [1, 1]
    // 2 -> [1, 2]
    // 6 -> [4, 5]
    CHECK(check_vec(SkipList::search_path(-1, layer1), {layer1, layer2}));
    CHECK(check_vec(SkipList::search_path(0, layer1), {layer1, nn0}));
    CHECK(check_vec(SkipList::search_path(1, layer1), {n1, nn1}));
    CHECK(check_vec(SkipList::search_path(2, layer1), {n1, nn2}));
    CHECK(check_vec(SkipList::search_path(6, layer1), {n4, nn5}));
}

TEST_CASE("function test") {
    using namespace std;

    // layer1: 1 3 4
    auto layer1 = SkipList::make_dummy_head();
    auto n1 = make_shared<Node>(1, 1);
    auto n3 = make_shared<Node>(3, 3);
    auto n4 = make_shared<Node>(4, 4);
    make_chain({layer1, n1, n3, n4});

    // layer2: 0 1 2 3 4 5 10
    auto layer2 = SkipList::make_dummy_head();
    auto nn0 = make_shared<Node>(0, 0);
    auto nn1 = make_shared<Node>(1, 1);
    auto nn2 = make_shared<Node>(2, 2);
    auto nn3 = make_shared<Node>(3, 3);
    auto nn4 = make_shared<Node>(4, 4);
    auto nn5 = make_shared<Node>(5, 5);
    make_chain({layer2, nn0, nn1, nn2, nn3, nn4, nn5});
    layer1->down = layer2;
    n1->down = nn1;
    n3->down = nn3;
    n4->down = nn4;

    SUBCASE("get") {
        // layer1:   1   3 4
        // layer2: 0 1 2 3 4 5
        // -1 -> nullopt
        // 0 -> 0
        // 1 -> 1
        // 2 -> 2
        // 5 -> 5
        // 6 -> nullopt
        SkipList table = SkipList();
        table.heads_.emplace_back(layer1);

        CHECK_FALSE(table.get(-1).has_value());
        CHECK(any_cast<int>(table.get(0).value()) == 0);
        CHECK(any_cast<int>(table.get(1).value()) == 1);
        CHECK(any_cast<int>(table.get(2).value()) == 2);
        CHECK(any_cast<int>(table.get(5).value()) == 5);
        CHECK_FALSE(table.get(6).has_value());
    }

    SUBCASE("get_range") {
        auto nn10 = make_shared<Node>(10, 10);
        make_chain({nn5, nn10});
        // layer1:   1   3 4
        // layer2: 0 1 2 3 4 5 10
        // [-3, -1) -> null
        // [-1, 0) -> null
        // [-1, 1) -> [0]
        // [0, 4) -> [0, 1, 2, 3]
        // [5, 8) -> [5]
        // [6, 10) -> null
        // [10, 15) -> [10]
        SkipList table = SkipList();
        table.heads_.emplace_back(layer1);

        CHECK(check_vec(table.get_range(-3, -1), {}));
        CHECK(check_vec(table.get_range(-1, 0), {}));
        CHECK(check_vec(table.get_range(-1, 1), {0}));
        CHECK(check_vec(table.get_range(0, 4), {0, 1, 2, 3}));
        CHECK(check_vec(table.get_range(5, 8), {5}));
        CHECK(check_vec(table.get_range(6, 10), {}));
        CHECK(check_vec(table.get_range(10, 15), {10}));
    }

    SUBCASE("remove") {
        auto nn10 = make_shared<Node>(10, 10);
        make_chain({nn5, nn10});
        // layer1:   1   3 4
        // layer2: 0 1 2 3 4 5 10
        // 0
        // layer1: 1   3 4
        // layer2: 1 2 3 4 5 10
        // 1
        // layer1:   3 4
        // layer2: 2 3 4 5 10
        // 10
        // layer1:   3 4
        // layer2: 2 3 4 5
        // 15
        // layer1:   3 4
        // layer2: 2 3 4 5
        SkipList table = SkipList();
        table.heads_.emplace_back(layer1);
        vector<vector<shared_ptr<Node>>> layer;

        table.remove(0);
        layer = {{layer1, n1, n3, n4}, {layer2, nn1, nn2, nn3, nn4, nn5, nn10}};
        CHECK(check_layer(layer1, layer));
        CHECK(table.size() == -1);
        table.remove(1);
        layer = {{layer1, n3, n4}, {layer2, nn2, nn3, nn4, nn5, nn10}};
        CHECK(check_layer(layer1, layer));
        CHECK(table.size() == -2);
        table.remove(10);
        layer = {{layer1, n3, n4}, {layer2, nn2, nn3, nn4, nn5}};
        CHECK(check_layer(layer1, layer));
        CHECK(table.size() == -3);
        table.remove(15);
        layer = {{layer1, n3, n4}, {layer2, nn2, nn3, nn4, nn5}};
        CHECK(check_layer(layer1, layer));
        CHECK(table.size() == -4);
    }

    SUBCASE("insert") {
        SkipList table = SkipList();

        const int N = 20;

        int n = 0;
        vector<int> gen{0, 0, 0, 0, 0, 1, 1, 1, 1, 1};

        for(int i = 0; i < N; i++) {
            int level = 1;
            int max_level = static_cast<int>(log(static_cast<double>(table.nnode_)) / log(2.0)) + 1;
            while(gen[n % 10] == 1 && level < max_level) {
                level++;
            }
            n++;
            table.insert(i, i, level);
        }
        table.insert(0, 2);
        table.insert(1, 2);
        CHECK(any_cast<int>(table.get(0).value()) == 2);
        CHECK(any_cast<int>(table.get(1).value()) == 2);
        vector<vector<int>> keys{
            {-2147483648, 16, 17, 18, 19},
            {-2147483648, 8, 9, 15, 16, 17, 18, 19},
            {-2147483648, 5, 6, 7, 8, 9, 15, 16, 17, 18, 19},
            {-2147483648, 5, 6, 7, 8, 9, 15, 16, 17, 18, 19},
            {-2147483648, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19}
        };
        CHECK(table.size() == N);
        CHECK(check_key(table.heads_[0], keys));
    }

    SUBCASE("insert_del") {
        SkipList table = SkipList();

        const int N = 20;

        int n = 0;
        vector<int> gen{0, 0, 0, 0, 0, 1, 1, 1, 1, 1};

        unordered_set<int> st;
        for(int i = 0; i < N; i++) {
            int level = 1;
            int max_level = static_cast<int>(log(static_cast<double>(table.nnode_)) / log(2.0)) + 1;
            while(gen[n % 10] == 1 && level < max_level) {
                level++;
            }
            n++;
            table.insert(i, i, level);
        }
        for(int i = 0; i < N / 2; i++) {
            table.remove(i);
        }
        vector<vector<int>> keys{
            {-2147483648, 16, 17, 18, 19},
            {-2147483648, 15, 16, 17, 18, 19},
            {-2147483648, 15, 16, 17, 18, 19},
            {-2147483648, 15, 16, 17, 18, 19},
            {-2147483648, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19}
        };
        CHECK(table.size() == N / 2);
        CHECK(check_key(table.heads_[0], keys));
    }
}