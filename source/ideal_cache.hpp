#include <list>
#include <vector>
#include <unordered_map>
#include <functional>
#include <utility>
#include <queue>
#pragma once

#include<stdio.h>

template <typename DPage, typename KeyT>
class ideal_cache{
    using ListIt = typename std::list<DPage>::iterator;

    size_t MaxSize = 0;
    size_t FastPageLoadCounter = 0;

    struct Compare {
        bool operator()(const std::pair<KeyT, size_t>& a, const std::pair<KeyT, size_t>& b) const {
            bool a_is_infinite = (a.second == __SIZE_MAX__);
            bool b_is_infinite = (b.second == __SIZE_MAX__);

            if (a_is_infinite && b_is_infinite)
                return false;
            else if (a_is_infinite)
                return false;
            else if (b_is_infinite)
                return true;
            else
                return a.second < b.second;
        }
    };

    typename std::priority_queue<std::pair<KeyT, size_t>, std::vector<std::pair<KeyT, size_t>>, Compare> index_queue;

    typename std::unordered_map<KeyT, size_t> ElemMap; //pairs element key -> next global use index
    typename std::unordered_map<KeyT, size_t> LocalIndexMap; //element key -> current local use index
    typename std::unordered_map<KeyT, std::vector<size_t>> GlobalIndexMap; //element key -> array of usage indexes

    typename std::vector<KeyT> RequestVector;

    

    public:

    ideal_cache(size_t MaxSize, std::vector<KeyT> RequestVector): 
        MaxSize(MaxSize),
        RequestVector(RequestVector)
        {}; 

    size_t count_fast_loads();

    void DUMP();
};

template <typename DPage, typename KeyT>
size_t ideal_cache<DPage, KeyT>::count_fast_loads(){
    //processing request vector
    for(size_t i = 0; i < RequestVector.size(); i++){
        LocalIndexMap[RequestVector[i]] = 0;
        GlobalIndexMap[RequestVector[i]].push_back(i);
    }

    for(size_t i = 0; i < RequestVector.size(); i++){        
        // DUMP();


        KeyT elem_key = RequestVector[i];
        
        size_t elem_next_local_index = ++LocalIndexMap[elem_key]; //следующее вхождение элемента
        size_t elem_next_global_index = __SIZE_MAX__; //индекс следующего вхождения
        
        if(elem_next_local_index < GlobalIndexMap[elem_key].size()){ //элемент встретится в будущем
            elem_next_global_index = (GlobalIndexMap[elem_key])[LocalIndexMap[elem_key]];
        }

        // std::cout << "processing " << RequestVector[i] << " with next usage of " << elem_next_global_index << '\n';

        //element already in cache
        if(ElemMap.find(elem_key) != ElemMap.end()){
            //push all indexes and counters and add to queue
            index_queue.emplace(elem_key, elem_next_global_index);
            ElemMap[elem_key] = elem_next_global_index;
            FastPageLoadCounter++;
        }

        //element not in cache
        else{
            if(elem_next_global_index == __SIZE_MAX__) continue; //элемент не встречается в будущем -> не добавляем в кэш
            
            if(ElemMap.size() < MaxSize){ //inserting, cuz why not
                ElemMap[elem_key] = elem_next_global_index;
                index_queue.emplace(elem_key, elem_next_global_index);
                continue;
            }
            
            else{
                while (!index_queue.empty()){ //cleaning queue
                    auto [q_elem, q_dist] = index_queue.top();
                    if(ElemMap.find(q_elem) != ElemMap.end() && ElemMap[q_elem] == q_dist) break;
                    index_queue.pop();
                }

                if(!index_queue.empty()){
                    auto [marked_for_remove, _] = index_queue.top();
                    ElemMap.erase(marked_for_remove);
                    index_queue.pop();
                }

                // неоптимизированный алгоритм
                // size_t max_index = 0;
                // KeyT elem_for_delete = -1;
                // for(auto &x: ElemMap){
                //     if(x.second > max_index) {
                //         std::cout << max_index << " < " << x.second << '\n';
                //         elem_for_delete = x.first;
                //         max_index = x.second;
                //     }
                //     else {
                //         std::cout << max_index << " > " << x.second << '\n';
                //     }
                // }
                // if(elem_for_delete == -1) {
                //     std::cout << "bad\n";
                // }
                // std::cout << "deleted " << elem_for_delete << '\n';
                // ElemMap.erase(elem_for_delete);

                ElemMap[elem_key] = elem_next_global_index;
                index_queue.emplace(elem_key, elem_next_global_index);
            }
        }
    }
    return FastPageLoadCounter;
}

template <typename DPage, typename KeyT>
void ideal_cache<DPage, KeyT>::DUMP(){
    std::cout << "----------------------------DUMP START---------------------------\n";
    std::cout << "local index:\n";
    for(auto &x: LocalIndexMap){
        std::cout << x.first << ": " << x.second << '\n';
    }
    std::cout << "\nGlobal index:\n";
    for(auto &x: GlobalIndexMap){
        std::cout << x.first << ": ";
        for(auto &index: x.second){
            std::cout << index << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\nElements (" << ElemMap.size() << ")\n";
    for(auto &x: ElemMap){
        std::cout << x.first << " : " << x.second << '\n';
    }
    std::cout << "\nPQ:\n";

    auto dump_index_queue = index_queue;
    
    while(!dump_index_queue.empty()){
        auto [key, index] = dump_index_queue.top();
        std::cout << key << " : " << index << '\n';
        dump_index_queue.pop();
    }

    std::cout << "----------------------------DUMP END---------------------------\n";
}