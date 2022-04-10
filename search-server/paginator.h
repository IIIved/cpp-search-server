#pragma once

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>


template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end);

    Iterator begin() const; 
    
    Iterator end() const; 

    size_t  size() const; 

private:
    Iterator first_, last_;
    size_t size_;
};

template <typename Iterator>
IteratorRange<Iterator>::IteratorRange(Iterator begin, Iterator end)
    : first_(begin)
    , last_(end)
    , size_(std::distance(first_, last_)) {}

template <typename Iterator>
Iterator IteratorRange<Iterator>::begin() const {
    return first_;
}

template <typename Iterator>
Iterator IteratorRange<Iterator>::end() const {
    return last_;
}

template <typename Iterator>
size_t IteratorRange<Iterator>::size() const {
    return size_;
}

template <typename Iterator>
inline std::ostream& operator<<(std::ostream& out, const IteratorRange<Iterator>& range) {
    for (Iterator it = range.begin(); it != range.end(); ++it) {
        out << *it;
    }
    return out;
}

template <typename Iterator>
class Paginator {
public:
    Paginator(Iterator begin, Iterator end, size_t page_size); 

    auto begin() const; 

    auto end() const; 

    size_t size() const; 

private:
    std::vector<IteratorRange<Iterator>> pages_;
};

template <typename Iterator>
Paginator<Iterator>::Paginator(Iterator begin, Iterator end, size_t page_size) {
    for (size_t left = std::distance(begin, end); left > 0;) {
        const size_t current_page_size = std::min(page_size, left);
        const Iterator current_page_end = std::next(begin, current_page_size);
        pages_.push_back({begin, current_page_end});
        left -= current_page_size;
        begin = current_page_end;
    }
}

template <typename Iterator>
auto Paginator<Iterator>::begin() const {
    return pages_.begin();
}

template <typename Iterator>
auto Paginator<Iterator>::end() const {
    return pages_.end();
}

template <typename Iterator>
size_t Paginator<Iterator>::size() const {
    return pages_.size();
}

template <typename Container>
inline auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}

