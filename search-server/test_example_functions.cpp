#include "test_example_functions.h"

std::string
GenerateWord(std::mt19937& generator, int max_length) {
    const int length = 
    std::uniform_int_distribution(1, max_length)(generator);
    std::string word;
    word.reserve(length);
    for (int i = 0; i < length; ++i) {
        word.push_back(std::uniform_int_distribution('a', 'z')(generator));
    }
    return word;
}

std::vector<std::string>
GenerateDictionary(std::mt19937& generator,
                   int word_count, int max_length) {
    std::vector<std::string> words;
    words.reserve(word_count);
    for (int i = 0; i < word_count; ++i) {
        words.push_back(GenerateWord(generator, max_length));
    }
    std::sort(words.begin(), words.end());
    words.erase(std::unique(words.begin(), words.end()), words.end());
    return words;
}

std::string
GenerateQuery(std::mt19937& generator,
              const std::vector<std::string>& dictionary,
              int max_word_count) {
    const int word_count =
    std::uniform_int_distribution(1, max_word_count)(generator);
    std::string query;
    for (int i = 0; i < word_count; ++i) {
        if (!query.empty()) {
            query.push_back(' ');
        }
        query += dictionary[std::uniform_int_distribution<int>(0,
                            dictionary.size() - 1)(generator)];
    }
    return query;
}

std::vector<std::string>
GenerateQueries(std::mt19937& generator,
                const std::vector<std::string>& dictionary,
                int query_count, int max_word_count) {
    std::vector<std::string> queries;
    queries.reserve(query_count);
    for (int i = 0; i < query_count; ++i) {
        queries.push_back(GenerateQuery(generator, dictionary,
                                        max_word_count));
    }
    return queries;
}

std::string
GenerateQuery2(std::mt19937& generator,
               const std::vector<std::string>& dictionary,
               int word_count, double minus_prob) {
    std::string query;
    for (int i = 0; i < word_count; ++i) {
        if (!query.empty()) {
            query.push_back(' ');
        }
        if (std::uniform_real_distribution<>(0, 1)(generator)
            < minus_prob) {
            query.push_back('-');
        }
        query += dictionary[std::uniform_int_distribution<int>(0, dictionary.size() - 1)(generator)];
    }
    return query;
}

std::vector<std::string>
GenerateQueries2(std::mt19937& generator,
                const std::vector<std::string>& dictionary,
                int query_count, int max_word_count) {
    std::vector<std::string> queries;
    queries.reserve(query_count);
    for (int i = 0; i < query_count; ++i) {
        queries.push_back(GenerateQuery2(generator, dictionary,
                                        max_word_count));
    }
    return queries;
}
