#include "log_duration.h"
#include "paginator.h"
#include "remove_duplicates.h"
#include "request_queue.h"
#include "search_server.h"

using namespace std;

void AddDocument(SearchServer& search_server, int document_id,
    const std::string& document,
    DocumentStatus status,
    const std::vector<int>& ratings) {
    try {
        search_server.AddDocument(document_id, document,
            status, ratings);
    }
    catch (const std::invalid_argument& e) {
        std::cout << "Ошибка добавления документа "s
            << document_id << ": "s << e.what()
            << std::endl;
    }
}

void FindTopDocuments(const SearchServer& search_server,
    const std::string& raw_query) {
    std::cout << "Результаты поиска по запросу: "s
        << raw_query << std::endl;
    const int document_count = search_server.GetDocumentCount();
    std::cout << "Всего документов: "s << document_count << std::endl;
    try {
        for (const Document& document :
            search_server.FindTopDocuments(raw_query)) {
            PrintDocument(document);
        }
    }
    catch (const std::invalid_argument& e) {
        std::cout << "Ошибка поиска: "s << e.what() << std::endl;
    }
}

void MatchDocuments(const SearchServer& search_server,
    const std::string& query) {
    try {
        std::cout << "Матчинг документов по запросу: "s
            << query << std::endl;

        for (const int document_id : search_server) {
            const auto [words, status] =
                search_server.MatchDocument(query, document_id);
            PrintMatchDocumentResult(document_id, words, status);
        }

    }
    catch (const std::invalid_argument& e) {
        std::cout << "Ошибка матчинга документов на запрос "s
            << query << ": "s << e.what() << std::endl;
    }
}


int main() {
    SearchServer search_server("and with"s);

    AddDocument(search_server, 1, "funny pet and nasty rat"s, DocumentStatus::ACTUAL, {7, 2, 7});
    AddDocument(search_server, 2, "funny pet with curly hair"s, DocumentStatus::ACTUAL, {1, 2});

    // дубликат документа 2, будет удалён
    AddDocument(search_server, 3, "funny pet with curly hair"s, DocumentStatus::ACTUAL, {1, 2});

    // отличие только в стоп-словах, считаем дубликатом
    AddDocument(search_server, 4, "funny pet and curly hair"s, DocumentStatus::ACTUAL, {1, 2});

    // множество слов такое же, считаем дубликатом документа 1
    AddDocument(search_server, 5, "funny funny pet and nasty nasty rat"s, DocumentStatus::ACTUAL, {1, 2});

    // добавились новые слова, дубликатом не является
    AddDocument(search_server, 6, "funny pet and not very nasty rat"s, DocumentStatus::ACTUAL, {1, 2});

    // множество слов такое же, как в id 6, несмотря на другой порядок, считаем дубликатом
    AddDocument(search_server, 7, "very nasty rat and not very funny pet"s, DocumentStatus::ACTUAL, {1, 2});

    // есть не все слова, не является дубликатом
    AddDocument(search_server, 8, "pet with rat and rat and rat"s, DocumentStatus::ACTUAL, {1, 2});

    // слова из разных документов, не является дубликатом
    AddDocument(search_server, 9, "nasty rat with curly hair"s, DocumentStatus::ACTUAL, {1, 2});
    
    std::cout << "Before duplicates removed: "s << search_server.GetDocumentCount() << std::endl;
    RemoveDuplicates(search_server);
    std::cout << "After duplicates removed: "s << search_server.GetDocumentCount() << std::endl;
}