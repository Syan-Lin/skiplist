#include <iostream>
#include <map>
#include <unordered_map>
#include <chrono>

#include "skip_list.h"

using namespace std;

void benchmark(const string model, const int scale) {
    vector<pair<int, int>> vec;
    map<int, int> mp;
    unordered_map<int, int> hash;
    SkipList st;

    auto start = chrono::high_resolution_clock::now();
    // insert benchmark
    for(int i = 0; i < scale; i++) {
        if(model == "vector") {
            vec.push_back({i, i});
        } else if(model == "map") {
            mp[i] = i;
        } else if(model == "hash") {
            hash[i] = i;
        } else if(model == "SkipList") {
            st.insert(i, i);
        }
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Insert benchmark(" << model << ") time: " << elapsed.count() << " seconds" << endl;

    // query benchmark
    start = chrono::high_resolution_clock::now();
    for(int i = 0; i < scale; i++) {
        if(model == "vector") {
            auto ele = find(vec.begin(), vec.end(), pair<int, int>(i, i));
        } else if(model == "map") {
            auto ele = mp.find(i);
        } else if(model == "hash") {
            auto ele = hash.find(i);
        } else if(model == "SkipList") {
            auto ele = st.get(i);
        }
    }
    end = chrono::high_resolution_clock::now();
    elapsed = end - start;
    cout << "Query benchmark(" << model << ") time: " << elapsed.count() << " seconds" << endl;

    // range benchmark
    start = chrono::high_resolution_clock::now();
    const int seg = scale / 10;
    for(int i = 0; i < 10; i++) {
        if(model == "SkipList") {
            auto ele = st.get_range(i * seg, (i + 1) * seg);
        } else {
            for(int j = i * seg; j < (i + 1) * seg && j < scale; j++) {
                if(model == "vector") {
                    auto ele = find(vec.begin(), vec.end(), pair<int, int>(j, j));
                } else if(model == "map") {
                    auto ele = mp.find(j);
                } else if(model == "hash") {
                    auto ele = hash.find(j);
                }
            }
        }
    }
    end = chrono::high_resolution_clock::now();
    elapsed = end - start;
    cout << "Range benchmark(" << model << ") time: " << elapsed.count() << " seconds" << endl;

    // del benchmark
    start = chrono::high_resolution_clock::now();
    for(int i = 0; i < scale; i++) {
        if(model == "vector") {
            vec.erase(find(vec.begin(), vec.end(), pair<int, int>(i, i)));
        } else if(model == "map") {
            mp.erase(mp.find(i));
        } else if(model == "hash") {
            hash.erase(hash.find(i));
        } else if(model == "SkipList") {
            st.remove(i);
        }
    }
    end = chrono::high_resolution_clock::now();
    elapsed = end - start;
    cout << "Del benchmark(" << model << ") time: " << elapsed.count() << " seconds" << endl;
    cout << endl;
}

int main() {
    const int SCALE = 1e6;
    benchmark("vector", SCALE);
    benchmark("map", SCALE);
    benchmark("hash", SCALE);
    benchmark("SkipList", SCALE);

    return 0;
}
