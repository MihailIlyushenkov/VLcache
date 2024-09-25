#include <list>
#include <iostream>
#include <unordered_map>

struct jorik {
    int name[10] = {0};
};

template <typename Tp>
struct Str{
    int id;
    Tp name;
};

int func(){

    std::list<jorik> L;

    std::unordered_map<int, std::list<jorik>::iterator> M;


    std::unordered_map<int, std::list<Str<jorik>>::iterator> M1;

    auto key = M1.find(1);

    jorik b;

    for(int i = 0; i < 5; i++){
        b.name[0] = i;
        L.push_back(b);
    }

    int i = 0;
    for(auto iter = L.begin(); iter != L.end(); iter++){
        M.emplace(i++, iter);
    }
    int count = i;

    for(i = 0; i < count; i++){
        std::cout << i << ": " << "jorik "<< (*M.at(i)).name[0] << '\n';
    }

    std::cout << "\n\n";

    int index = 0;
    jorik t = *M.at(index);
    auto k = M.find(index);

    L.insert(M.at(3), t);

    i = 0;
    for(auto &x: L){
        std::cout << i << ": " << "jorik "<< *x.name << '\n';
        i++;
    }

    int y = 0;
    int a = ++y;
    std::cout << a << ' '<< y;

    return 0;
}

int foo(){
    std::list<int> L1 = {1, 2, 3, 4, 5, 6, 7};
    std::list<int> L2 = {8, 9, 10, 11, 12, 13};

    using ListIt = std::list<int>::iterator;

    ListIt L1It1 = L1.begin();
    L1It1++;
    L1It1++;
    L1It1++;

    ListIt L1It2 = L1.begin();

    L1.splice(L1It1, L2);

    for(auto &x: L1){
        std::cout << x << '\n';
    }

    return 0;
}

int badf(void* s){
    char* f = (char*) s ;
    f[0] = 'H';
    return 0;
}

int main(){
    std::cout << "in" << '\n';
    const char * s = "hello world";
    std::cout << "out";
    badf((void*) s);
    return 0;
}