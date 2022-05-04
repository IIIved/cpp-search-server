#pragma once

#include <deque>

#include "search_server.h"

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);

    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(
        const std::string& raw_query,
        DocumentPredicate document_predicate);

    std::vector<Document> AddFindRequest(
        const std::string& raw_query,
        DocumentStatus status);

    std::vector<Document> AddFindRequest(
        const std::string& raw_query) {
        return AddFindRequest(raw_query, DocumentStatus::ACTUAL);
    }

    int GetNoResultRequests() const;
private:
    std::deque<bool> requests_;
    const static int min_in_day_ = 1440;
    int current_time = 0;
    const SearchServer& search_server_;
};

// class RequestQueue public:
template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(
    const std::string& raw_query,
    DocumentPredicate document_predicate) {
    auto vec_documents = search_server_.FindTopDocuments(raw_query,
        document_predicate);

    if (current_time == min_in_day_) {
        requests_.pop_front();
    }
    else {
        ++current_time;
    }
    for (int i = min_in_day_; i < requests_.size(); ++i) {
        requests_.pop_front();
    }
    if (vec_documents.empty()) {
        requests_.push_back(false);
    }
    return vec_documents;
}
