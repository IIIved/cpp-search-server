#pragma once

#include <iostream>
#include <set>
#include <string>
#include <vector>


using namespace std;


template <typename StringContainer>
set<string> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    set<string> non_empty_strings;
    for (const string& str : strings) {
        if (!str.empty()) {
            non_empty_strings.insert(str);
        }
    }
    return non_empty_strings;
}

vector<string> SplitIntoWords(const string& text);