#pragma once

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <deque>

#include "search_server.h"

class RequestQueue {
public:

    explicit RequestQueue(const SearchServer& search_server) : search_server_(search_server){
        empty_reqest_count = 0;
        count_ = 0;
    }

    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
        std::vector<Document> documents = search_server_.FindTopDocuments(raw_query, document_predicate);
        Add_Degue(documents);
        Delete_Deque();
        return documents;
    }

    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);

    std::vector<Document> AddFindRequest(const std::string& raw_query);

    int GetNoResultRequests() const;

    private:
    struct QueryResult {
        int time = 0;
        bool result = false;
    };
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    const SearchServer& search_server_;
    int count_;
    int empty_reqest_count;

    void Add_Degue(std::vector<Document> documents);

    void Delete_Deque();
};
