#pragma once

template <class Iterator>
class iterator_range {
public:
  iterator_range(Iterator begin, Iterator end)
    : begin_(begin), end_(end) {}
  
  auto begin() -> Iterator { return begin_; }
  auto end() -> Iterator { return end_; }

  auto size() -> decltype(Iterator()-Iterator()) {
    return end_ - begin_;
  }
  bool empty() { return begin_ == end_; }

private:
  Iterator begin_, end_;
};

template <class Iterator>
auto make_iterator_range(Iterator begin, Iterator end)
-> iterator_range<Iterator> {
  return iterator_range<Iterator>(begin, end);
}

