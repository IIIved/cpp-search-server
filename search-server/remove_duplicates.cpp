
#include "remove_duplicates.h"

bool MapComparition(std::map<std::string, double> Map_1, std::map<std::string, double> Map_2){
    bool result = false;
    int temp = 0;
    for (const auto [word, freq] : Map_1){
        if (Map_2.count(word)){
            temp++;
        }
    }
    if (temp == Map_2.size()){
        result = true;
    }
    return result;
}

void RemoveDuplicates(SearchServer& search_server){
    std::map<std::string, double> temp_result_1, temp_result_2;
    std::set<int> id_to_delete;
    auto it_begin = search_server.begin();
    auto it_end = search_server.end();

    for (;it_begin != it_end; it_begin++) {
        temp_result_1 = search_server.GetWordFrequencies(*it_begin);
        auto it_plus = it_begin;
        for (; it_plus != it_end; it_plus++){
            temp_result_2 = search_server.GetWordFrequencies(*it_plus);
            if (it_plus != it_begin && it_plus != it_end && temp_result_1.size() == temp_result_2.size()){
                if (MapComparition(temp_result_1, temp_result_2)){
                    id_to_delete.insert(*it_plus);
                }
            }
        }
        //it_begin = it_temp;
    }
    for (const int x : id_to_delete){
        std::cout<< "Found duplicate document id"s << " "s << x << std::endl;
        search_server.RemoveDocument(x);
    }
}
