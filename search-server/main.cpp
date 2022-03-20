

// -------- Начало модульных тестов поисковой системы ----------

// Тест проверяет, что поисковая система исключает стоп-слова при добавлении документов
void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }

    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT_HINT(server.FindTopDocuments("in"s).empty(), "Stop words must be excluded from documents"s);
    }
}

/*

Разместите код остальных тестов здесь
*/

// Тест проверяет, что добавленный документ находится по поисковому запросу, который содержит слова из документа.
void TestDocumentsAdding() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("cat"s);
        ASSERT(found_docs.size() == 1u);
        const Document& doc0 = found_docs[0];
        ASSERT(doc0.id == doc_id);
    }
 
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("dog near red town"s);
        ASSERT(found_docs.empty());
    }
}
 
void TestMinusWordsSupporting() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT(server.FindTopDocuments("in the -city"s).empty());
    }
    
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in the city"s);
        ASSERT(found_docs.size() == 1);
        const Document& doc0 = found_docs[0];
        ASSERT(doc0.id == doc_id);
    }
 
}
 
void TestMatching() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
   
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        auto [value, data] = server.MatchDocument("dog in the big -city"s, doc_id);
        ASSERT(value.empty());
    }
}
 
void TestRelevanceSorting() {
    {
        const int doc_id_1 = 42;
        const string content_1 = "cat in the city city city"s;
        const vector<int> ratings_1 = {1, 2, 3};
 
        const int doc_id_2 = 43;
        const string content_2 = "fox fox the city city city"s;
        const vector<int> ratings_2 = {4, 5, 6};
 
        const int doc_id_3 = 44;
        const string content_3 = "dog dog dog in the city"s;
        const vector<int> ratings_3 = {2, 4, 2};
 
        SearchServer server;
        server.AddDocument(doc_id_1, content_1, DocumentStatus::ACTUAL, ratings_1);
        server.AddDocument(doc_id_2, content_2, DocumentStatus::ACTUAL, ratings_2);
        server.AddDocument(doc_id_3, content_3, DocumentStatus::ACTUAL, ratings_3);
        const auto found_docs = server.FindTopDocuments("cat fox dog"s);
        ASSERT(found_docs.size() == 3);
        ASSERT(found_docs[0].id == doc_id_3);
        ASSERT(found_docs[1].id == doc_id_2);
        ASSERT(found_docs[2].id == doc_id_1);
    }
}
 
void TestingTheRatingCalculation() {
    {
        const int doc_id_1 = 42;
        const string content_1 = "cat in the city city city"s;
        const vector<int> ratings_1 = {1, 2, 3};
 
        SearchServer server;
        server.AddDocument(doc_id_1, content_1, DocumentStatus::ACTUAL, ratings_1);
        const auto found_docs = server.FindTopDocuments("cat fox dog"s);
        ASSERT(!found_docs.empty());
        ASSERT(found_docs[0].rating == round((1 + 2 + 3 + 0.0) / 3));
    }
}
 
void TestFilteringSearchResultsUsingAPredicate() {
    const int doc_id = 42;
    const string content = "cat in the city city city"s;
    const vector<int> ratings = {1, 2, 3};
    const DocumentStatus document_status = DocumentStatus::ACTUAL;

    {
        SearchServer server;
        server.AddDocument(doc_id, content, document_status, ratings);
        const auto found_docs = server.FindTopDocuments("cat fox dog"s, [] (int document_id, DocumentStatus status, int rating) {
            
            if (status == DocumentStatus::ACTUAL) {document_id +=1;};
            return rating > 1;
        });
        ASSERT(!found_docs.empty());
        ASSERT(found_docs[0].id == doc_id);
    }
 
    {
        SearchServer server;
        server.AddDocument(doc_id, content, document_status, ratings);
        const auto found_docs = server.FindTopDocuments("cat fox dog"s, [] (int document_id, DocumentStatus status, int rating) {
        if (status == DocumentStatus::ACTUAL) {document_id +=1;};
        return rating > 3;
        });
        ASSERT(found_docs.empty());
    }
 
}
 
void TestForSearchingForDocumentsWithAGivenStatus() {
    const int doc_id = 42;
    const string content = "cat in the city city city"s;
    const vector<int> ratings = {1, 2, 3};
    const DocumentStatus document_status = DocumentStatus::BANNED;
    
    {
        SearchServer server;
        server.AddDocument(doc_id, content, document_status, ratings);
        const auto found_docs = server.FindTopDocuments("cat fox dog"s, DocumentStatus::ACTUAL);
        ASSERT(found_docs.empty());
    }
 
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::BANNED, ratings);
        const auto found_docs = server.FindTopDocuments("cat fox dog"s, DocumentStatus::BANNED);
        ASSERT(!found_docs.empty());
        ASSERT(found_docs[0].id == doc_id);
    }
 
 
}
 
void TestRelevanceAccuracy() {
    
    {
        const int doc_id_1 = 42;
        const string content_1 = "cat in the city city city"s;
        const vector<int> ratings_1 = {1, 2, 3};
 
        const int doc_id_2 = 43;
        const string content_2 = "fox fox the city city city"s;
        const vector<int> ratings_2 = {4, 5, 6};
 
        const int doc_id_3 = 44;
        const string content_3 = "dog dog dog in the city"s;
        const vector<int> ratings_3 = {2, 4, 2};
 
        SearchServer server;
        
        server.AddDocument(doc_id_1, content_1, DocumentStatus::ACTUAL, ratings_1);
        server.AddDocument(doc_id_2, content_2, DocumentStatus::ACTUAL, ratings_2);
        server.AddDocument(doc_id_3, content_3, DocumentStatus::ACTUAL, ratings_3);
        
        const auto found_docs = server.FindTopDocuments("cat fox dog"s);
        ASSERT(found_docs.size() == 3);
        ASSERT(found_docs[0].id == doc_id_3);
        ASSERT(abs(found_docs[0].relevance - log(3)/2) < 1e-6);
        ASSERT(found_docs[1].id == doc_id_2);
        ASSERT(abs(found_docs[1].relevance - log(3)/3) < 1e-6);
        ASSERT(found_docs[2].id == doc_id_1);
        ASSERT(abs(found_docs[2].relevance - log(3)/6) < 1e-6);
    }
}

// Функция TestSearchServer является точкой входа для запуска тестов
void TestSearchServer() {
    RUN_TEST(TestExcludeStopWordsFromAddedDocumentContent);
    RUN_TEST(TestDocumentsAdding);
    RUN_TEST(TestMinusWordsSupporting);
    RUN_TEST(TestMatching);
    RUN_TEST(TestRelevanceSorting);
    RUN_TEST(TestingTheRatingCalculation);
    RUN_TEST(TestFilteringSearchResultsUsingAPredicate);
    RUN_TEST(TestForSearchingForDocumentsWithAGivenStatus);
    RUN_TEST(TestRelevanceAccuracy);
    // Не забудьте вызывать остальные тесты здесь
}

// --------- Окончание модульных тестов поисковой системы -----------

int main() {
    TestSearchServer();
    // Если вы видите эту строку, значит все тесты прошли успешно
    cout << "Search server testing finished"s << endl;
}
