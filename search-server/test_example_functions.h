#pragma once

#include "log_duration.h"
#include "search_server.h"

#include <iostream>
#include <random>

std::string
GenerateWord(std::mt19937& generator, int max_length);

std::vector<std::string>
GenerateDictionary(std::mt19937& generator,
                   int word_count, int max_length);

std::string
GenerateQuery(std::mt19937& generator,
              const std::vector<std::string>& dictionary,
              int max_word_count);

std::vector<std::string>
GenerateQueries(std::mt19937& generator,
                const std::vector<std::string>& dictionary,
                int query_count, int max_word_count);

std::string
GenerateQuery2(std::mt19937& generator,
               const std::vector<std::string>& dictionary,
               int word_count, double minus_prob = 0);

std::vector<std::string>
GenerateQueries2(std::mt19937& generator,
                 const std::vector<std::string>& dictionary,
                 int query_count, int max_word_count);

// ProcessQueries test
template <typename QueriesProcessor>
void Test_Process_Queries(std::string_view mark,
                          QueriesProcessor processor,
                          const SearchServer& search_server,
                          const std::vector<std::string>& queries) {
    LOG_DURATION(mark);
    const auto documents_lists = processor(search_server, queries);
}

#define TEST_PROCESS_QUERIES(processor) Test_Process_Queries(#processor, processor, ss, queries)

// RemoveDocument test
template <typename ExecutionPolicy>
void Test_Remove_Document(std::string_view mark,
                          SearchServer search_server,
                          ExecutionPolicy&& policy) {
    LOG_DURATION(mark);
    const int document_count = search_server.GetDocumentCount();
    for (int id = 0; id < document_count; ++id) {
        search_server.RemoveDocument(policy, id);
    }
    std::cout << search_server.GetDocumentCount() << std::endl;
}

#define TEST_REMOVE_DOCUMENT(mode) Test_Remove_Document(#mode, ss, std::execution::mode)

// MatchDocument test
template <typename ExecutionPolicy>
void Test_Match_Document(std::string_view mark,
                         SearchServer search_server,
                         const std::string& query,
                         ExecutionPolicy&& policy) {

    LOG_DURATION(mark);
    const int document_count = search_server.GetDocumentCount();
    int word_count = 0;
    for (int id = 0; id < document_count; ++id) {
        const auto [words, status] = 
              search_server.MatchDocument(policy, query, id);
        word_count += words.size();
    }
    std::cout << word_count << std::endl;
}

#define TEST_MATCH_DOCUMENT(policy) Test_Match_Document(#policy, ss, queries, std::execution::policy)

// FindDocument test
template <typename ExecutionPolicy>
void Test_Find_Document(std::string_view mark,
                        const SearchServer& search_server,
                        const std::vector<std::string>& queries,
                        ExecutionPolicy&& policy) {
    LOG_DURATION(mark);
    double total_relevance = 0;
    for (const std::string_view query : queries) {
        for (const auto& document :
             search_server.FindTopDocuments(policy, query)) {
            total_relevance += document.relevance;
        }
    }
    std::cout << total_relevance << std::endl;
}

#define TEST_FIND_DOCUMENT(policy) Test_Find_Document(#policy, ss, queries, std::execution::policy)
