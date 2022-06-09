#include "search_server.h"

// PUBLIC

void SearchServer::AddDocument(int document_id,
                   const std::string_view document,
                   DocumentStatus status,
                   const std::vector<int>& ratings) {
    if ((document_id < 0) ||
        (documents_.count(document_id) > 0)) {
        throw std::invalid_argument("Invalid document_id");
    }

    const auto words = SplitIntoWordsNoStop(document);
    const double inv_word_count = 1.0 / words.size();

    for (const auto word : words) {
        auto it = words_.insert(std::string(word));
        std::string_view sv_word = *(it.first);
        word_to_document_freqs_[sv_word][document_id]
            += inv_word_count;
        document_to_word_freqs_[document_id][sv_word]
            += inv_word_count;
    }

    documents_.emplace(document_id,
                       DocumentData
                       {ComputeAverageRating(ratings), status});

    document_ids_.emplace(document_id);
}

int SearchServer::GetDocumentCount() const {
    return documents_.size();
}

std::set<int>::const_iterator SearchServer::begin() const {
    return document_ids_.begin();
}

std::set<int>::const_iterator SearchServer::end() const {
    return document_ids_.end();
}

std::list<int> 
SearchServer::GetDuplicates() const {
    std::list<int> result;
    std::set<std::list<std::string_view>> bunch_of_words;
    for (const auto& [id, words] : document_to_word_freqs_) {
        std::list<std::string_view> key(words.size());
        std::transform(words.begin(), words.end(),
                       key.begin(),
                       [](const auto& value) {
                           return value.first;
                       });
        if (bunch_of_words.count(key) > 0) {
            result.push_back(id);
        } else {
            bunch_of_words.insert(key);
        }
    }
    return result;
}

const std::map<std::string_view, double>&
SearchServer::GetWordFrequencies(int document_id) const {
    static std::map<std::string_view, double> result;
    if (documents_.count(document_id) > 0) {
        result = document_to_word_freqs_.at(document_id);
    } else {
        result.clear();
    }
    return result;
}




// FindTopDocuments
std::vector<Document>
SearchServer::FindTopDocuments(
              const std::string_view raw_query,
              DocumentStatus status) const {
    return FindTopDocuments(raw_query,
           [status](int document_id,
                    DocumentStatus document_status,
                    int rating) {
                        return document_status == status;
                    });
}

std::vector<Document>
SearchServer::FindTopDocuments(
              const std::string_view raw_query) const {
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}

// MatchDocument
std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(std::string_view raw_query, int document_id) const {
    return SearchServer::MatchDocument(std::execution::seq, raw_query, document_id);
}

// PRIVATE

bool SearchServer::IsStopWord(const std::string_view word) const {
    return stop_words_.count(word) > 0;
}

bool SearchServer::IsValidWord(const std::string_view word) {
    return std::none_of(word.begin(), word.end(),
                        [](char c) {
                            return c >= '\0' && c < ' ';
                        });
}

bool SearchServer::IsNonValidWord(const std::string_view word) {
    return std::any_of(word.begin(), word.end(),
                       [](char c) {
                           return c >= '\0' && c < ' ';
                       });
}

std::vector<std::string_view>
SearchServer::SplitIntoWordsNoStop(
              const std::string_view text) const {
    std::vector<std::string_view> words;
    for (const auto word : SplitIntoWords(text)) {
        if (!IsValidWord(word)) {
            throw std::invalid_argument("Word "s + std::string(word)
                                                 + " is invalid"s);
        }
        if (!IsStopWord(word) && word.size() > 0) {
            words.push_back(word);
        }
    }
    return words;
}

int SearchServer::ComputeAverageRating(
                  const std::vector<int>& ratings) {
    if (ratings.empty()) {
        return 0;
    }
    return std::accumulate(ratings.begin(), ratings.end(), 0) /
           static_cast<int>(ratings.size());
}

double SearchServer::ComputeWordInverseDocumentFreq(
                     const std::string_view word) const {
    return log(GetDocumentCount() * 1.0 /
               word_to_document_freqs_.at(word).size());
}

SearchServer::QueryWord
SearchServer::ParseQueryWord(const std::string_view text) const {
    if (text.empty()) {
        throw std::invalid_argument("Query word is empty"s);
    }

    std::string_view word = text;
    bool is_minus = false;
    if (word[0] == '-') {
        is_minus = true;
        word = word.substr(1);
    }
    if (word.empty() || word[0] == '-' || IsNonValidWord(word)) {
        throw std::invalid_argument("Query word "s
                                    + std::string(text)
                                    + " is invalid"s);
    }
    return { word, is_minus, IsStopWord(word) };
}

// RemoveDocument
void SearchServer::RemoveDocument(int document_id) {
    RemoveDocument(std::execution::seq, document_id);
}