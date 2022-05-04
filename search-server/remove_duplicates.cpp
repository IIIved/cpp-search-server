#include "remove_duplicates.h"

using namespace std;

void RemoveDuplicates(SearchServer& search_server) {

    std::vector<int> vec_duplicate_ids;
    auto it_end = search_server.end();
    for (auto it1 = search_server.begin(); it1 != it_end; ++it1) {
        auto it2_begin = it1;
        ++it2_begin;
        if (it2_begin != it_end) {
            for (auto it2 = it2_begin; it2 != it_end; ++it2) {
                if (search_server.CompareDocumentsWords(*it1, *it2)) {
                    vec_duplicate_ids.push_back(*it2);
                    break;
                }
            }
        }
    }

    for (const auto id : vec_duplicate_ids) {
        std::cout << "Found duplicate document id "s
                  << id << std::endl;
        search_server.RemoveDocument(id);
    }

}
