#pragma once

#include "search_server.h"

#include <list>

std::vector<std::vector<Document>>
ProcessQueries(const SearchServer& search_server,
               const std::vector<std::string>& queries);

std::list<Document>
ProcessQueriesJoined(const SearchServer& search_server,
                     const std::vector<std::string>& queries);

std::vector<std::vector<Document>>
ProcessQueries(const SearchServer& search_server,
               const std::vector<std::string>& queries) {
    std::vector<std::vector<Document>> result(queries.size());
    std::transform(std::execution::par,
                   queries.begin(), queries.end(), result.begin(),
                   [&search_server](std::string const qry) {
                       return search_server.FindTopDocuments(qry);
                   });
    return result;
}

std::list<Document>
ProcessQueriesJoined(const SearchServer& search_server,
                     const std::vector<std::string>& queries) {
    std::vector<std::vector<Document>> documents(queries.size());
    std::transform(std::execution::par,
                   queries.begin(), queries.end(), documents.begin(),
                   [&search_server](std::string const qry) {
                       return search_server.FindTopDocuments(qry);
                   });
    std::list<Document> result;
    std::list<Document>::iterator it;
    for (const std::vector<Document>& v : documents) {
        it = result.insert(result.end(), v.begin(), v.end());
    }
    return result;
}
