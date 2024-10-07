#include <list>
#include <unordered_map>
#include <functional>
#include <utility>

#pragma once

enum STATE{
    NORMAL,
    IDEAL,
};

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
        LFUcache(int size, int anchors_count, int* anchors_arr, KeyT excKey, std::function<DPage(KeyT)> GetPageFunc);
    
        DPage getPage(KeyT Key); //Key = unique index of page

        DPage getPageKostyli(KeyT Key); //Key = unique index of page
        void generate_anchors();

        //uses requests, that will appear in the future
        DPage getPage_ideal(KeyT Key, int request_index, std::unordered_map<KeyT, std::list<int>>& RQmap);

        int getLenght();

        int DUMP();

        int rm(int index);

        int getHits() {return FastPageLoadCounter;};
    private:

        

        using ListIt = typename std::list<ListEl<DPage, KeyT>>::iterator;

        int remove_el(ListIt iter);
        ListIt move_el(ListIt elem, ListIt pos);



        long unsigned int MaxSize = 0;
        
        int anchors_count = 0;
        int* anchors = nullptr;
        KeyT anchor_key;

        int FastPageLoadCounter = 0;
        int SlowPageLoadCounter = 0;
        int RemoveCounter = 0;


        std::list<ListEl<DPage, KeyT>> PageList;


        typename std::unordered_map<KeyT, typename std::list<ListEl<DPage, KeyT>>::iterator>  PageMap;
        typename std::unordered_map<int, typename std::list<ListEl<DPage, KeyT>>::iterator>  AnchorMap;

        std::function<DPage(KeyT)> GetPageFunc;
};


template <typename DPage, typename KeyT>
LFUcache<DPage, KeyT>::LFUcache(int size, int anchors_count, int* anchors_arr, KeyT excKey, std::function<DPage(KeyT)> GetPageFunc): 
    MaxSize(size), 
    anchors_count(anchors_count),
    anchors(anchors_arr),
    anchor_key(excKey),
    GetPageFunc(GetPageFunc)
    {};

//removes element from list and maps by given list iterator
//(!)iterator given to this function becomes invalid(!)
template <typename DPage, typename KeyT>
int LFUcache<DPage, KeyT>::remove_el(ListIt iter) { 

    // #ifdef TEST
    // std::cout << "attempt to remove" << "(key " << iter->Key <<":cnt " << iter->counter << ")\n"; 
    // this->DUMP();
    // #endif

    if(iter == PageList.end()) return 0;

    auto mapit = PageMap.find(iter->Key);
    if(mapit != PageMap.end()) {
        PageMap.erase(mapit);
    }

    PageList.erase(iter);

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
DPage LFUcache<DPage, KeyT>::getPage(KeyT Key){

    auto y = PageMap.find(Key);

    if(y == PageMap.end()) {
        //creating list element with given data page and 0 calls

        if(MaxSize == PageList.size()){
            if(PageList.begin()->counter == 0)
                if (PageList.size() == 1) {
                    remove_el(PageList.begin());
                }
                else {
                    int rem_flag = 1;
                    for(ListIt x = PageList.begin(); x != PageList.end(); x++){
                        if (x->counter > 0) {
                            remove_el(--x);
                            rem_flag = 0;
                            break;
                        }
                    }
                    if (rem_flag == 1) {
                        remove_el(--PageList.end());
                    }
                }
            else {
                remove_el(PageList.begin());
            }
        }


        DPage NewPage = GetPageFunc(Key);
        SlowPageLoadCounter++;
        ListEl<DPage, KeyT> NewListEl = {0, Key, NewPage}; 

        PageList.push_front(NewListEl);
        PageMap.emplace(Key, PageList.begin());

        return PageList.begin()->page;
    }
    else {                              
        ListIt x = y->second; //iterator to the list element
    
        if(x == PageList.end()){                       
            //element is not in list (bad, smth went wrong)                    
            std::cout << "invalid page. Something is very wrong" << '\n';
            std::exit(-1);
        }
        else{                                          

            //everything is fine
            FastPageLoadCounter++;
            x->counter++;
            ListIt iter = x;

            for(iter++; iter != PageList.end(); iter++){
                if(iter->counter > x->counter) {

                    ListIt inserted =  PageList.insert(iter, *x);
                    PageList.erase(x);
                    PageMap.erase(Key);
                    PageMap.emplace(Key, inserted);
                    return inserted->page;
                }
            }

            if(PageList.back().counter <= x->counter) {
                PageList.push_back(*x);
                PageMap.erase(Key);
                PageList.erase(x);
                PageMap.emplace(Key, --PageList.end());
                return PageList.back().page;
            }

            return x->page;
        }
    }

    std::cout << "unexpected error\n";
    return GetPageFunc(Key);
}


template <typename DPage, typename KeyT>
typename std::list<ListEl<DPage, KeyT>>::iterator LFUcache<DPage, KeyT>::move_el(ListIt pos, ListIt elem){ //removes element given by 1st iterator and creates new before 2nd iterator
    if(elem == PageList.end() || elem == pos){
        return elem; //operation doesn't make sense
    }
    
    // if(pos != PageList.end()){
    // std::cout << "moving "<< "Page(" << elem->page.data[0] << "):" << elem->Key << "->" << elem->counter 
    //         << "to " << "Page(" << pos->page.data[0] << "):" << pos->Key << "->" << pos->counter << '\n'; 
    // }
    // else {
    // std::cout << "moving "<< "Page(" << elem->page.data[0] << "):" << elem->Key << "->" << elem->counter 
    //         << "to end\n";
    // }
    
    ListIt newpos = PageList.insert(pos, *elem);
    
    // std::cout << "after insert:\n";
    // DUMP();
    
    remove_el(elem); //removing elem by iterator from list and by elem->key in map
    PageMap.emplace(newpos->Key, newpos);

    return newpos;
}

template <typename DPage, typename KeyT>
DPage LFUcache<DPage, KeyT>::getPageKostyli(KeyT Key){
    auto x = PageMap.find(Key);

    if(x == PageMap.end()){
        if(PageList.size() - anchors_count == MaxSize){
            ListIt el_to_remove = PageList.begin(); //removing first not-anchor element (least usages, last called)
            for(; el_to_remove != PageList.end(); el_to_remove++){
                if (el_to_remove->Key != anchor_key) {
                    remove_el(el_to_remove);
                    break;
                }
            }
        }

        //inserting element
        if(anchors[0] == 0){ //inserting to zero-anchor
            ListEl<DPage, KeyT> l{0, Key, GetPageFunc(Key)};
            ListIt iter = PageList.insert(AnchorMap[anchors[0]], l);
            PageMap.emplace(Key, iter);
            SlowPageLoadCounter++;
            return iter->page;
        }
        else {
            std::cout << "anchor[0] mus be 0. Just do it. Anyway, something went (or will go) wrong\n";
        }

    }
    else{
        ListIt iter = x->second;

        iter->counter++;
        if(iter != (--PageList.end())){ //try to advance element if not in end of list
            if(iter->counter <= anchors[anchors_count-1]){ //check if element's counter is supported by anchors
                auto insert_anchor_pr = AnchorMap.find(iter->counter);
                if(insert_anchor_pr != AnchorMap.end()){ //attaching element near anchor
                    FastPageLoadCounter++;
                    return move_el(insert_anchor_pr->second, iter)->page;
                }
                std::cout << "anchor value not in map. Error (kostily has been broken((\n";
            }
            else { //very sad. need to find element's placement in o(n). Begining from biggest anchor.
                for(ListIt i = AnchorMap[anchors[anchors_count-1]]; i != PageList.end(); i++){
                    if(i->counter > iter->counter){
                        FastPageLoadCounter++;
                        return move_el(i, iter)->page;
                    }
                }
                //threre were no return in for -> need to enter element in the end
                FastPageLoadCounter++;
                return move_el(PageList.end(), iter)->page;
            }
        }
        
        FastPageLoadCounter++;
        return x->second->page;
    }

    return GetPageFunc(Key); //should't be called, but anyway
}

template <typename DPage, typename KeyT>
DPage LFUcache<DPage, KeyT>::getPage_ideal(KeyT Key, int request_index, std::unordered_map<KeyT, std::list<int>>& RQmap){
    
    // std::cout << "Key: " << Key << " index: " << request_index << '\n';
    // this->DUMP();

    auto x = PageMap.find(Key);
    ListIt elem_to_remove = PageList.begin();

    if(x == PageMap.end()){

        auto pr = RQmap.find(Key);
        if(pr == RQmap.end()){
            std::cout << "bad request (not in request map)\n";
        }
        else if (pr->second.back() <= request_index) {
            SlowPageLoadCounter++;
            // std::cout << "element (" << Key << " : "<< request_index << ") wont appear in future\n";

            return GetPageFunc(Key); //??????????
        }

        if(PageList.size() == MaxSize){ //removing element
            int cur_dist = -1;
            int max_dist = -1;

            for(ListIt iter = PageList.begin(); iter != PageList.end(); iter++){
                cur_dist = -1;
                // std::cout << "checking elem at key " << iter->Key << '\n';
                auto rqpair = RQmap.find(iter->Key);
                if(rqpair == RQmap.end()) {
                    std::cout << "Error: request is not in request map.\n";
                    return GetPageFunc(Key);;
                } 
                else {
                    // std::cout << "key: " << rqpair->first << "-> ";
                    // for(auto& distance: rqpair->second) {std::cout << distance << ' ';}
                    // std::cout << '\n';

                    for(auto& distance: rqpair->second) { //finding index of next request of this element
                        if(distance > request_index){
                            cur_dist = distance - request_index;
                            // std::cout << "founded dist is " << cur_dist << '\n';
                            break;
                        }
                    }
                }

                if (cur_dist == -1) { //element wont be requested in the future
                    elem_to_remove = iter;
                    // std::cout << "elem to remove cuz wont be used: " << iter->Key << '\n';s
                    break;
                }
                else if(cur_dist > max_dist) {
                    max_dist = cur_dist;
                }
            }

            remove_el(elem_to_remove);
        }

        DPage NewPage = GetPageFunc(Key);
        SlowPageLoadCounter++;
        ListEl<DPage, KeyT> NewListEl = {0, Key, NewPage}; 

        PageList.push_front(NewListEl);
        PageMap.emplace(Key, PageList.begin());

        return NewPage;
    }
    else{
        FastPageLoadCounter++;
        return (x->second->page);
    }
    
    std::cout << "unexpected error\n";
    return GetPageFunc(Key);
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

    std::cout << "\nanchor map\n";
    for(auto &x: AnchorMap){
        std::cout << "Key " << x.first << ": " << "Page(" << x.second->page.data[0] << "):" << -x.second->Key << "->" << x.second->counter << '\n';
    }
    #endif

    std::cout << "\nSlow loads: " << SlowPageLoadCounter << '\n';
    std::cout << "Fast loads: " << FastPageLoadCounter << '\n';
    std::cout << "Removes counter: " << RemoveCounter << '\n';
    std::cout << "List lenght: " << PageList.size() << " (including "<< anchors_count << " anchors)" << '\n';
    
    std::cout << "-------------------------------DUMP END-------------------------------" << '\n';
    
    return 0;
}

template <typename DPage, typename KeyT>
void LFUcache<DPage, KeyT>::generate_anchors(){
    //костыли)

    for(int i = 0; i < anchors_count; i++){
        ListEl<DPage, KeyT> l{anchors[i], anchor_key, GetPageFunc(anchor_key)};
        PageList.push_back(l);
        AnchorMap.emplace(anchors[i], --PageList.end());
    }
    // std::cout << "kostily generated\n";

    // DUMP();

    // for(int i = 0; i < anchors_count; i++){
    //     ListEl<DPage, KeyT> l{100*anchors[i]+1, anchor_key, GetPageFunc(anchor_key)};
    //     std::cout << "inserting " << anchors[i] << '\n';
    //     PageList.insert(AnchorMap[anchors[i]], l);
    //     std::cout << "inserting " << anchors[i] << " succs" << '\n';
    //     DUMP();

    // }
    // std::cout << "full kostily generated\n";
}

#ifdef TEST
int testfoo(int x) {
    return x*x;
}
#endif


