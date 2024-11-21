#include <list>
#include <iostream>

#include "ideal_cache.hpp"
#include "cache_assistance.hpp"

int main(const int argc, const char* argv[]){
    // std::cout << "ideal\n";
    int size = -1;
    int testcount = -1;

    std::cin >> size >> testcount;
    std::vector<int> requests(testcount);

    // std::cout << "size " << size << ", testcount: " << testcount << '\n';
    if(std::cin.bad()){
        std::cout << "Error while reading size and amount of tests from cin.\n";
        return -1;
    }

    for(int i = 0; i < testcount; i++){
        std::cin >> requests[i];
        if(std::cin.bad()){
            std::cout << "Error while reading tests data from cin.\n";
        }
    }

    ideal_cache<MYPage, int> ideal_cache(size, requests);

    
    std::cout << ideal_cache.count_fast_loads() << '\n';
    
    return 0;
}