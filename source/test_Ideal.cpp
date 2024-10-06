#include <list>
#include <iostream>

#include "LFU_cache.hpp"
#include "cache_assistance.hpp"

int main(const int argc, const char* argv[]){
    // std::cout << "ideal\n";
    int size = -1;
    int testcount = -1;

    std::cin >> size >> testcount;
    // std::cout << "size " << size << ", testcount: " << testcount << '\n';
    if(std::cin.bad()){
        std::cout << "Error while reading size and amount of tests from cin.\n";
        return -1;
    }

    int* requests = (int*) calloc(sizeof(int), testcount);

    for(int i = 0; i < testcount; i++){
        std::cin >> requests[i];
        if(std::cin.bad()){
            std::cout << "Error while reading tests data from cin.\n";
        }
    }

    LFUcache<MYPage, int> LFU(size, SlowGetPage);
    std::unordered_map<int, std::list<int>> ReqMap;
    
    for(int i = 0; i < testcount; i++){
        auto x = ReqMap.find(requests[i]);
        if(x == ReqMap.end()){
            std::list<int> indexes;
            indexes.push_back(i);
            ReqMap.emplace(requests[i], indexes);

        }
        else{
            x->second.push_back(i);
        }
    }

    for(int i = 0; i < testcount; i++){
        LFU.getPage_ideal(requests[i], i, ReqMap);
    }

    // LFU.DUMP();
    std::cout << LFU.getHits();

    return 0;
}