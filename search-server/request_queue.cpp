#include "request_queue.h"

RequestQueue::RequestQueue(const SearchServer& search_server)
    :search_server_(search_server)
{}

int RequestQueue::GetNoResultRequests() const {
    return requests_.size();
}

std::vector<Document> RequestQueue::AddFindRequest(
    const std::string& raw_query,
    DocumentStatus status) {
    return AddFindRequest(raw_query,
        [status](int document_id,
            DocumentStatus document_status,
            int rating) {
                return document_status == status; });
}
