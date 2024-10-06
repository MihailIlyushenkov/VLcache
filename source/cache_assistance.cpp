#include <iostream>

#include "LFU_cache.hpp"
#include "cache_assistance.hpp"

MYPage SlowGetPage(int Key){
    MYPage a;
    a.data[0] = Key;
    return a;
}

void printpage(MYPage* p) {
    if (p == 0) {
        std::cout << "bad page value \n";
    }
    else std::cout << "got page " << p->data[0] << '\n';
}

int checkpage(MYPage* p, int val){
    return (p->data[0] == val)?1:0;
}

int* readtestsdata(const char* filename, size_t* testcount){
    FILE* file = fopen("test/data.bin", "rb");
    if (file == nullptr){
        std::cout << "cant open file\n";
        return 0;
    }

    if (fread(testcount, sizeof(int), 1, file) != 1) {
        std::cout << "can't read amount of tests\n";
        return 0;
    }
    #ifdef TEST
    std::cout << *testcount << '\n';
    std::cout << *lowcalls << '\n';
    std::cout << *highcalls << '\n';
    std::cout << *uniquecalls << '\n';
    #endif

    int* data = new int [sizeof(int)*(*testcount)];

    if (fread(data, sizeof(int), *testcount, file) != *testcount) {
        std::cout << "amount of tests (" << *testcount << ") does not coincide with reallity\n";
        delete data;
        return 0;
    }

    return data;
}

int testfile(const char* filename){
    int size = 1000;

    LFUcache<MYPage, int> LFU(size, SlowGetPage);

    size_t testcount = 0;

    int* data = readtestsdata("test/data.bin", &testcount);

    for(size_t i = 0; i < testcount; i++){
        MYPage* P = LFU.getPage(data[i]);
        if(!checkpage(P, data[i])) std::cout << "bad test\n"; 
    }    

    LFU.DUMP();

    return 0;
}
