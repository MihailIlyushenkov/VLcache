#include <list>
#include <iostream>
#include <math.h>

#include "LFU_cache.hpp"
#include "cache_assistance.hpp"


int main(const int argc, const char* argv[]){

    size_t size = 0;
    size_t testcount = 0;

    std::cin >> size >> testcount;
    if( (size == 0) ||(testcount == 0)){
        std::cout << "Invalid conut of test/cache size\n";
        return -1;
    }

    int* requests = new int[testcount];

    for(size_t i = 0; i < testcount; i++){
        std::cin >> requests[i];
        if(std::cin.bad()){
            std::cout << "Error while reading tests data from cin.\n";
        }
    }

    size_t anchors_count = ceil(sqrt(testcount));
    // std::cout << anchors_count << '\n';
    int anchors[anchors_count] = {};
    for(size_t i = 0; i < anchors_count; i++){
        anchors[i] = i;
    }
    int anchor_key = -__INT_MAX__;


    LFUcache<MYPage, int> LFU(size, anchors_count, anchors, anchor_key, SlowGetPage);
    LFU.generate_anchors();

    for(size_t i = 0; i < testcount; i++){
        if(!checkpage(LFU.getPage(requests[i]), requests[i])) {
            std::cout << "bad page\n";
        }
    }

    // LFU.DUMP();
    std::cout << LFU.getHits() << '\n';

    return 0;
}