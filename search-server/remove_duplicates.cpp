#include "remove_duplicates.h"

#include <iostream>

using namespace std;

void RemoveDuplicates(SearchServer& search_server)
{
    std::map<std::set<std::string>, int> word_to_document_freqs;
    std::set<int> documents_to_delete;

    for (auto document_id : search_server)
    {
        std::set<std::string> words;

        for (auto& [document, freqs] : search_server.GetWordFrequencies(document_id))
        {
            words.insert(document);
        }
        if (word_to_document_freqs.find(words) == word_to_document_freqs.end())
        {
            word_to_document_freqs[words] = document_id;
        }
        else if (document_id > word_to_document_freqs[words])
        {
            word_to_document_freqs[words] = document_id;
            documents_to_delete.insert(document_id);
        }

    }

    for (auto document : documents_to_delete)
    {
        std::cout << "Found duplicate document id " << document << std::endl;
        search_server.RemoveDocument(document);
    }
}