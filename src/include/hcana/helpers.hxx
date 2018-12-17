#ifndef hcana_algos_hh
#define hcana_algos_hh

#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

namespace hcana {

  template <typename Vector, typename Value>
  auto lower_bound(Vector&& v, const Value& val)
  {
    return std::lower_bound(std::begin(v), std::end(v), val);
  }

  template <typename Vector, typename Value>
  auto upper_bound(Vector&& v, const Value& val)
  {
    return std::upper_bound(std::begin(v), std::end(v), val);
  }

  template <typename Vector, typename Value>
  auto bounds(Vector&& v, const Value& val)
  {
    return std::make_pair(std::lower_bound(std::begin(v), std::end(v), val),
                          std::upper_bound(std::begin(v), std::end(v), val));
  }

  template <typename Vector, typename Value, typename Fun>
  auto lower_bound(Vector&& v, const Value& val, Fun fun)
  {
    return std::lower_bound(std::begin(v), std::end(v), val, fun);
  }

  template <typename Vector, typename Value, typename Fun>
  auto upper_bound(Vector&& v, const Value& val, Fun fun)
  {
    return std::upper_bound(std::begin(v), std::end(v), val, fun);
  }

  template <typename Vector, typename Value, typename Fun>
  auto bounds(Vector&& v, const Value& val, Fun fun)
  {
    return std::make_pair(std::lower_bound(std::begin(v), std::end(v), val, fun),
                          std::upper_bound(std::begin(v), std::end(v), val, fun));
  }

  template <typename Vector>
  auto adjacent_find(Vector&& v) {
    return std::adjacent_find(std::begin(v), std::end(v));
  }

  template <typename Vector, typename Fun>
  auto adjacent_find(Vector&& v, Fun fun) {
    return std::adjacent_find(std::begin(v), std::end(v), fun);
  }

  /** Returns the iterator to first discontinuous element.
   *  If container is continuous it resturns std::end(v).
   *  This version uses the increment operator on the container type to test for "continuity".
   */
  template <class ForwardIt>
  auto find_discontinuity_impl(ForwardIt first, ForwardIt last) {
    if (first == last ){
      // empty container
      return last;
    }
    auto result = first;
    for (auto it = first; it != last; it++) {
      if (it + 1 == last) {
        // Got to the end without finding discontinutity
        return last;
      }
      auto val = *it;
      if (*(it + 1) != ++val) {
        return it+1;
      }
    }
    return last;
  }

  /** Returns the iterator to first discontinuous element. 
   *  If container is continuous it resturns std::end(v).
   */
  template <typename Vector>
  auto find_discontinuity(Vector&& v) {
    return find_discontinuity_impl(std::begin(v), std::end(v));
  }

  /** Returns the iterator to first discontinuous element. 
   *  If container is continuous it resturns std::end(v).
   *  \param Fun is a binary predcate of the the form [](T x1, T x2)->bool{} 
   *  which returns true if x1 and x2 are "continuous".
   */
  template <typename ForwardIt, typename Fun>
  auto find_discontinuity_impl(ForwardIt first, ForwardIt last, Fun fun) {
    if (first == last ){
      // empty container
      return last;
    }
    auto result = first;
    for(auto it = first; it != last; it++) {
      if (it + 1 == last) {
        // Got to the end without finding discontinutity
        return last;
      }
      auto val = *it;
      if (!fun(val,*(it + 1))) {
        return it+1;
      }
    }
    return last;
  }
  template <typename Vector, typename Fun>
  auto find_discontinuity(Vector&& v, Fun fun) {
    return find_discontinuity_impl(std::begin(v), std::end(v), fun);
  }

  /** Returns the number of discountinuties of a container. 
   *  A range of zero size, single element, or continuous range will return 0.
   */
  template <typename Vector>
  auto count_discontinuities(Vector&& v) {
    auto first   = std::begin(v);
    auto last    = std::end(v);
    auto current = find_discontinuity_impl(first, last);
    int  count   = 0;
    while (current != last) {
      current = find_discontinuity_impl(current, last);
      count++;
    }
    return count;
  }

  template <typename Vector, typename Fun>
  auto count_discontinuities(Vector&& v, Fun fun) {
    auto first   = std::begin(v);
    auto last    = std::end(v);
    auto current = find_discontinuity_impl(first, last, fun);
    int  count   = 0;
    while (current != last) {
      current = find_discontinuity_impl(current, last, fun);
      count++;
    }
    return count;
  }

  /** Returns a vector of iterators to the start of each discontiuitiy. 
   * 
   */
  template <typename Vector>
  auto get_discontinuities(Vector&& v) {
    auto first   = std::begin(v);
    auto last    = std::end(v);
    using Iterator = decltype(first);
    std::vector<std::pair<Iterator, Iterator>> cont_ranges;
    auto current = find_discontinuity_impl(first, last);
    cont_ranges.push_back(std::make_pair(first, current));
    while (current != last) {
      auto prev = current;
      current   = find_discontinuity_impl(current, last);
      cont_ranges.push_back(std::make_pair(prev, current));
    }
    return cont_ranges;
  }
  /** Returns a vector of iterators to the start of each discontiuitiy. 
   * 
   */
  template <typename Vector, typename Fun>
  auto get_discontinuities(Vector&& v, Fun fun) {
    auto first   = std::begin(v);
    auto last    = std::end(v);
    using Iterator = decltype(first);
    std::vector<std::pair<Iterator, Iterator>> cont_ranges;
    auto current = find_discontinuity_impl(first, last, fun);
    //std::vector<Vector::iterator> res = {first};
    cont_ranges.push_back(std::make_pair(first, current));
    while (current != last) {
      auto prev = current;
      current   = find_discontinuity_impl(current, last, fun);
      cont_ranges.push_back(std::make_pair(prev, current));
    }
    return cont_ranges;
  }
  //{
  //  std::vector<int>  test_vector = {1,2,5,6,7, 9,10, 20};
  //  auto test_res = hcana::find_discontinuity(test_vector);
  //  std::cout << " find_discontinuity test: " << *test_res << "\n";
  //}
  //{
  //  std::vector<int>  test_vector = {0,2,4,8,10,12,13, 16,18,20,23};
  //  auto              test_res    = hcana::find_discontinuity(
  //      test_vector, [](const int& x1, const int& x2) { return x1 + 2 == x2; });
  //  std::cout << " find_discontinuity test2: " << *test_res << "\n";
  //}

  template <typename Vector, typename Value>
  auto find(Vector&& v, const Value& val)
  {
    return std::find(std::begin(v), std::end(v), val);
  }

  template <typename Vector, typename Fun>
  auto find_if(Vector&& v, Fun fun)
  {
    return std::find_if(std::begin(v), std::end(v), fun);
  }

  template <typename Vector, typename Value>
  auto* ptr_find(Vector&& v, const Value& val)
  {
    auto it = std::find(std::begin(v), std::end(v), val);
    return it != std::end(v) ? &*it : nullptr;
  }

  template <typename Vector, typename Fun>
  auto* ptr_find_if(Vector&& v, Fun fun)
  {
    auto it = std::find_if(std::begin(v), std::end(v), fun);
    return it != std::end(v) ? &*it : nullptr;
  }

  template <typename Vector, typename Value>
  bool contains(Vector&& v, const Value& val)
  {
    return find(v, val) != std::end(v);
  }

  template <typename Vector, typename Value>
  void remove_one(Vector&& v, const Value& val)
  {
    auto it = find(v, val);
    if (it != v.end())
    {
      v.erase(it);
    }
  }

  template <typename Vector, typename Function>
  void remove_one_if(Vector&& v, const Function& val)
  {
    auto it = find_if(v, val);
    if (it != v.end())
    {
      v.erase(it);
    }
  }

  template <typename Vector, typename Fun>
  bool any_of(Vector&& v, Fun fun)
  {
    return std::any_of(std::begin(v), std::end(v), fun);
  }

  template <typename Vector, typename Fun>
  auto all_of(Vector&& v, Fun fun)
  {
    return std::all_of(std::begin(v), std::end(v), fun);
  }

  template <typename Vector, typename Fun>
  bool none_of(Vector&& v, Fun fun)
  {
    return std::none_of(std::begin(v), std::end(v), fun);
  }

  template <typename Vector, typename Fun>
  auto remove_if(Vector&& v, Fun fun)
  {
    return std::remove_if(std::begin(v), std::end(v), fun);
  }

  template <typename Vector, typename Fun>
  auto count_if(Vector&& v, Fun fun)
  {
    return std::count_if(std::begin(v), std::end(v), fun);
  }

  template <typename Vector, typename Fun>
  auto max_element(Vector&& v, Fun fun)
  {
    return std::max_element(std::begin(v), std::end(v), fun);
  }

  template <typename Vector>
  auto sort(Vector&& v)
  {
    return std::sort(std::begin(v), std::end(v));
  }

  template <typename Vector, typename T>
  auto fill(Vector&& v, const T& val)
  {
    return std::fill(std::begin(v), std::end(v), val);
  }

  template <typename Vector>
  auto unique(Vector&& v)
  {
    return std::unique(std::begin(v), std::end(v));
  }

  template <typename Vector, typename Fun>
  auto sort(Vector&& v, Fun fun)
  {
    return std::sort(std::begin(v), std::end(v), fun);
  }

  template <typename Vector, typename OutputIterator, typename Fun>
  auto transform(Vector&& v, OutputIterator it, Fun f)
  {
    return std::transform(v.begin(), v.end(), it, f);
  }

  template <typename Vector1, typename Vector2>
  void copy(const Vector1& source, Vector2& destination)
  {
    destination.reserve(destination.size() + source.size());
    std::copy(source.begin(), source.end(), std::back_inserter(destination));
  }

  template <typename Vector1, typename Vector2, typename Pred>
  void copy_if(const Vector1& source, Vector2& destination, Pred predicate)
  {
    std::copy_if(
      source.begin(), source.end(), std::back_inserter(destination),
      predicate);
  }
}

#endif
