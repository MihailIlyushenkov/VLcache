#include <list>
#include <iostream>

#include "LFU_cache.hpp"
#include "cache_assistance.hpp"

int main(const int argc, const char* argv[]){
    // std::cout << "LFU\n";

    int size = -1;
    int testcount = -1;

    std::cin >> size >> testcount;
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

    for(int i = 0; i < testcount; i++){
        if(!checkpage(LFU.getPage(requests[i]), requests[i])) {
            std::cout << "bad page\n";
        }
    }
    
    // LFU.DUMP();
    std::cout << LFU.getHits();

    return 0;
}