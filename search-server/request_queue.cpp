#include "request_queue.h"

    std::vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
        std::vector<Document> documents = search_server_.FindTopDocuments(raw_query, status);
        Add_Degue(documents);
        Delete_Deque();
        return documents;
    }

    std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {
        std::vector<Document> documents = search_server_.FindTopDocuments(raw_query);
        Add_Degue(documents);
        Delete_Deque();
        return documents;
    }

    int RequestQueue::GetNoResultRequests() const {
        return empty_reqest_count;

    }


    void RequestQueue::Add_Degue(std::vector<Document> documents){
        count_++;
        if (documents.empty())
            empty_reqest_count++;
        requests_.push_back({count_, !documents.empty()});
    }

    void RequestQueue::Delete_Deque(){
        QueryResult x;
        if (requests_.size() > min_in_day_){
            x = requests_.front();
            if (x.result == 0)
                empty_reqest_count--;
            requests_.pop_front();
        }
    }
