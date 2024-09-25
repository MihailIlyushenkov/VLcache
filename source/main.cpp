#include <list>
#include <iostream>

#include "cache.hpp"

struct MYPage
{
    int data[100];
};

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

int* readtests(const char* filename, int* testcount, int* lowcalls, int* highcalls, int* uniquecalls){
    FILE* file = fopen("test/data.bin", "rb");
    if (file == nullptr){
        std::cout << "cant open file\n";
        return 0;
    }

    if (fread(testcount, sizeof(int), 1, file) != 1) {
        std::cout << "can't read amount of tests\n";
        return 0;
    }
    if (fread(lowcalls, sizeof(int), 1, file) != 1) {
        std::cout << "can't read amount of lowcalls\n";
        return 0;
    }
    if (fread(highcalls, sizeof(int), 1, file) != 1) {
        std::cout << "can't read amount of highcalls\n";
        return 0;
    }
    if (fread(uniquecalls, sizeof(int), 1, file) != 1) {
        std::cout << "can't read amount of uniquecalls\n";
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


int main(const int argc, const char* argv[]){
    int size = 100000;


    LFUcache<MYPage, int> LFC(size, SlowGetPage);

    int testcount = 0;
    int lowcalls = 0;
    int highcalls = 0;
    int uniquecalls = 0;

    int* data = readtests("test/data.bin", &testcount, &lowcalls, &highcalls, &uniquecalls);

    for(int i = 0; i < testcount; i++){
        MYPage* P = LFC.getPage(data[i]);
        if(!checkpage(P, data[i])) std::cout << "bad test\n"; 
    }    

    LFC.DUMP();

    std::cout << "in inputed data there were:\n\t" << lowcalls << " calls to pages from range with low values (high freq)\n\t" 
    << highcalls << " calls to pages from range with high values (low freq)\n\t" << uniquecalls << " unique calls\n";

    return 0;
}

//если класс попадает в две единицы трансляции и он полексемно не эквивалентен, то это UB в чистом виде
//inline функции это исключение из ODR
//ключевое слово static означает новое определение на каждый TU
// inline int foo(int x) {return x;} //ok, исключение из ORD
// static int foo(int x) {return x;} //ok, multiple defs

//const ссылки продлевают время жизни
//ссылки как члены класса это плохая идея
//временный объект живет до конца полного выражения

//ссылки на объекты деградируют

//процесс искажения имен - манглирование