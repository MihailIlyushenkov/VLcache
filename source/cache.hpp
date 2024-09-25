#include <list>
#include <unordered_map>
#include <functional>
#include <utility>

#pragma once

template <typename PageT, typename KeyT>
struct ListEl
{
    int counter = 0;
    KeyT Key;
    PageT page;
};

template <typename DPage, typename KeyT>
class LFUcache{
    public:
        LFUcache(int size, std::function<DPage(KeyT)> GetPageFunc);
    
        DPage* getPage(KeyT Key); //Key = unique index of page

        int getLenght();

        int DUMP();

        int rm(int index);

    private:

        using ListIt = typename std::list<ListEl<DPage, KeyT>>::iterator;

        int remove_el(ListIt iter);


        long unsigned int MaxSize = 0;
        int pagecnt = 0;

        int FastPageLoadCounter = 0;
        int SlowPageLoadCounter = 0;
        int RemoveCounter = 0;


        std::list<ListEl<DPage, KeyT>> PageList;

        typename std::unordered_map<KeyT, typename std::list<ListEl<DPage, KeyT>>::iterator>  PageMap;

        std::function<DPage(KeyT)> GetPageFunc;
};


template <typename DPage, typename KeyT>
LFUcache<DPage, KeyT>::LFUcache(int size, std::function<DPage(KeyT)> GetPageFunc): 
    MaxSize(size), 
    GetPageFunc(GetPageFunc) 
    {};

//removes element from list and maps by given list iterator
//(!)iterator given to this function becomes invalid(!)
template <typename DPage, typename KeyT>
int LFUcache<DPage, KeyT>::remove_el(ListIt iter) { 

    #ifdef TEST
    std::cout << "attempt to remove" << "(key " << iter->Key <<":cnt " << iter->counter << ")\n"; 
    this->DUMP();
    #endif

    PageList.erase(iter);

    #ifdef DETTEST
    std::cout << "list erase sucsessful\n";
    #endif

    auto mapit = PageMap.find(iter->Key);

    #ifdef DETTEST
    // std::cout << "got mapiterator\n";
    #endif

    if(mapit != PageMap.end()) {
        PageMap.erase(mapit);
    }

    #ifdef DETTEST
    // std::cout << "map erase sucsessful\n";
    #endif

    RemoveCounter++;
    return 0;
} 

template <typename DPage, typename KeyT>
int LFUcache<DPage, KeyT>::rm(int index) {
    
    ListIt iter = PageList.begin();
    for(int i = 0; i < index; i++){
        iter++;
    }

    remove_el(iter);

    return 0;
}


template <typename DPage, typename KeyT>
DPage* LFUcache<DPage, KeyT>::getPage(KeyT Key){

    auto y = PageMap.find(Key);

    if(y == PageMap.end()) {
        //creating list element with given data page and 0 calls

        #ifdef TEST
        std::cout << "key not in map\n";
        #endif

        if(MaxSize == PageList.size()){
            if(PageList.begin()->counter == 0)
                if (PageList.size() == 1) {

                    #ifdef DETTEST
                    std::cout << "remove on " << __LINE__ << "\n";
                    #endif
                    remove_el(PageList.begin());
                }
                else {
                    int rem_flag = 1;
                    for(ListIt x = PageList.begin(); x != PageList.end(); x++){
                        if (x->counter > 0) {

                            #ifdef DETTEST
                            std::cout << "remove on " << __LINE__ << "\n";
                            std::cout << "iterator now is Page(" << x->page.data[0] << "):" << x->Key << "->" << x->counter << '\n';
                            #endif
                            
                            remove_el(--x);

                            #ifdef DETTEST
                            std::cout << "removing done";
                            #endif

                            rem_flag = 0;
                            break;
                        }
                    }
                    if (rem_flag == 1) {

                        #ifdef DETTEST
                        std::cout << "remove on " << __LINE__ << "\n";
                        #endif

                        remove_el(--PageList.end());
                    }
                }
            else {
                #ifdef DETTEST
                std::cout << "remove on " << __LINE__ << "\n";
                #endif
                remove_el(PageList.begin());
            }
            #ifdef DETTEST
            std::cout << "erasing done\n";
            #endif
        }


        #ifdef DETTEST
        std::cout << "inserting " << Key << " ..." << '\n';
        #endif

        DPage NewPage = GetPageFunc(Key);
        SlowPageLoadCounter++;
        ListEl<DPage, KeyT> NewListEl = {0, Key, NewPage}; 

        PageList.push_front(NewListEl);
        PageMap.emplace(Key, PageList.begin());
        
        #ifdef DETTEST
        std::cout << "inserting done\n";
        #endif

        return &(PageList.begin()->page);
    }
    else {                              
        #ifdef DETTEST
        std::cout << "element is in map\n";
        #endif
        ListIt x = y->second; //iterator to the list element
    
        if(x == PageList.end()){                       
            //element is not in list (bad, smth went wrong)                    
            std::cout << "invalid page. Something is very wrong" << '\n';
            std::exit(-1);
        }
        else{                                          

            //everything is fine
            #ifdef DETTEST
            std::cout << "page(" << x->Key <<") found in list with counter: " << x->counter << '\n';
            #endif

            FastPageLoadCounter++;
            x->counter++;
            ListIt iter = x;

            for(iter++; iter != PageList.end(); iter++){
                if(iter->counter > x->counter) {
                    #ifdef DETTEST
                    // std::cout << iter->counter << ">" << x->counter << '\n';
                    #endif

                    ListIt inserted =  PageList.insert(iter, *x);
                    PageList.erase(x);
                    PageMap.erase(Key);
                    PageMap.emplace(Key, inserted);
                    #ifdef DETTEST
                    // std::cout << "moved list element\n";
                    #endif
                    return &(inserted->page);
                }
                else {
                    #ifdef DETTEST 
                    // std::cout << iter->counter << "<=" << x->counter << '\n';
                    #endif
                }
            }

            if(PageList.back().counter <= x->counter) {
                PageList.push_back(*x);
                PageMap.erase(Key);
                PageList.erase(x);
                PageMap.emplace(Key, --PageList.end());
                return &(PageList.back().page);
            }

            return &(x->page);
        }
    }

    return 0;
}

template <typename DPage, typename KeyT>
int LFUcache<DPage, KeyT>::getLenght(){
    return PageList.size();
}

template <typename DPage, typename KeyT>
int LFUcache<DPage, KeyT>::DUMP(){
    std::cout << "-------------------------------DUMP START-------------------------------" << '\n';
    #ifdef TEST
    std::cout << "\nlist\n";
    for(auto &x: PageList){
        std::cout << "Page(" << x.page.data[0] << "):" << x.Key << "->" << x.counter << '\n';
    }

    std::cout << "\numap\n";
    for(auto &x: PageMap){
        std::cout << "Key " << x.first << ": " << "Page(" << (*x.second).page.data[0] << "):" << (*x.second).Key << "->" << (*x.second).counter << '\n';
    }
    #endif

    std::cout << "\nSlow loads: " << SlowPageLoadCounter << '\n';
    std::cout << "Fast loads: " << FastPageLoadCounter << '\n';
    std::cout << "Removes counter: " << RemoveCounter << '\n';
    std::cout << "List lenght: " << PageList.size() << '\n';
    
    std::cout << "-------------------------------DUMP END-------------------------------" << '\n';

    
    return 0;
}

#ifdef TEST
int testfoo(int x) {
    return x*x;
}
#endif


