#include "log_duration.h"
#include "process_queries.h"
#include "search_server.h"
#include "test_example_functions.h"

using namespace std::string_literals;

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

void RemoveDuplicates(SearchServer& search_server) {
    for (const auto id : search_server.GetDuplicates()) {
        std::cout << "Found duplicate document id "s
                  << id << std::endl;
        search_server.RemoveDocument(id);
    }
}
/*
void PrintDocument(const Document& document) {
    std::cout << "{ "s
              << "document_id = "s << document.id << ", "s
              << "relevance = "s << document.relevance << ", "s
              << "rating = "s << document.rating << " }"s
              << std::endl;
}
*/
int main() {

    SearchServer search_server("and with"s);

    int id = 0;
    for (
        const std::string& text : {
            "white cat and yellow hat"s,
            "curly cat curly tail"s,
            "nasty dog with big eyes"s,
            "nasty pigeon john"s,
        }
    ) {
        search_server.AddDocument(++id, text,
                                  DocumentStatus::ACTUAL, {1, 2});
    }

    std::cout << "ACTUAL by default:"s << std::endl;
    // последовательная версия
    for (const Document& document : 
         search_server.FindTopDocuments("curly nasty cat"s)) {
        PrintDocument(document);
    }
    std::cout << "BANNED:"s << std::endl;
    // последовательная версия
    for (const Document& document :
         search_server.FindTopDocuments(std::execution::seq,
                                        "curly nasty cat"s,
                                        DocumentStatus::BANNED)) {
        PrintDocument(document);
    }

    std::cout << "Even ids:"s << std::endl;
    // параллельная версия
    for (const Document& document : 
         search_server.FindTopDocuments(std::execution::par,
                                        "curly nasty cat"s,
         [](int document_id, DocumentStatus status, int rating) {
             return document_id % 2 == 0; })) {
        PrintDocument(document);
    }

/*
/// ProcessQueries test
    {
        std::mt19937 generator;
        const auto dictionary = GenerateDictionary(generator,
                                                   2'000, 25);
        const auto documents = GenerateQueries(generator,
                                               dictionary,
                                               20'000, 10);

        SearchServer ss(dictionary[0]);
        for (size_t i = 0; i < documents.size(); ++i) {
            ss.AddDocument(i, documents[i], DocumentStatus::ACTUAL,
                            {1, 2, 3});
        }

        const auto queries = GenerateQueries(generator, dictionary,
                                             2'000, 7);
        TEST_PROCESS_QUERIES(ProcessQueries);
    }

/// ProcessQueriesJoined test
    {
        SearchServer ss("and with"s);

        int id = 0;
        for (const std::string& text : {
                "funny pet and nasty rat"s,
                "funny pet with curly hair"s,
                "funny pet and not very nasty rat"s,
                "pet with rat and rat and rat"s,
                "nasty rat with curly hair"s
            }) {
            ss.AddDocument(++id, text, DocumentStatus::ACTUAL,
                           {1, 2});
        }

        const std::vector<std::string>
        queries = {"nasty rat -not"s,
                   "not very funny nasty pet"s,
                   "curly hair"s};

        for (const Document& document :
             ProcessQueriesJoined(ss, queries)) {
            std::cout << "Document "s << document.id
                      << " matched with relevance "s
                      << document.relevance
                      << std::endl;
        }
    }

/// RemoveDocument test
    {
        std::mt19937 generator;
        const auto dictionary = GenerateDictionary(generator,
                                                   10'000, 25);
        const auto documents = GenerateQueries(generator,
                                               dictionary,
                                               10'000, 100);

        {
            SearchServer ss(dictionary[0]);
            for (size_t i = 0; i < documents.size(); ++i) {
                ss.AddDocument(i, documents[i],
                               DocumentStatus::ACTUAL, {1, 2, 3});
            }
            TEST_REMOVE_DOCUMENT(seq);
        }

        {
            SearchServer ss(dictionary[0]);
            for (size_t i = 0; i < documents.size(); ++i) {
                ss.AddDocument(i, documents[i],
                               DocumentStatus::ACTUAL, {1, 2, 3});
            }
            TEST_REMOVE_DOCUMENT(par);
        }
    }

/// MatchDocument test
    {
        std::mt19937 generator;
        const auto dictionary = GenerateDictionary(generator,
                                                   1000, 10);
        const auto documents = GenerateQueries2(generator,
                                                dictionary,
                                                10'000, 70);
        const std::string queries = GenerateQuery2(generator,
                                                   dictionary,
                                                   500, 0.1);

        SearchServer ss(dictionary[0]);
        for (size_t i = 0; i < documents.size(); ++i) {
            ss.AddDocument(i, documents[i],
                           DocumentStatus::ACTUAL, {1, 2, 3});
        }

        TEST_MATCH_DOCUMENT(seq);
        TEST_MATCH_DOCUMENT(par);
    }

/// FindDocument test
    {
        std::mt19937 generator;
        const auto dictionary = GenerateDictionary(generator,
                                                   1000, 10);
        const auto documents = GenerateQueries2(generator,
                                                dictionary,
                                                10'000, 70);

        SearchServer ss(dictionary[0]);
        for (size_t i = 0; i < documents.size(); ++i) {
            ss.AddDocument(i, documents[i],
                           DocumentStatus::ACTUAL, {1, 2, 3});
        }

        const auto queries = GenerateQueries2(generator,
                                              dictionary,
                                              100, 70);
        TEST_FIND_DOCUMENT(seq);
        TEST_FIND_DOCUMENT(par);
    }
*/
    return 0;
}
