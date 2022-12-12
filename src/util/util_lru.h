#include <cstdint>
#include <list>
#include <unordered_map>

namespace dxvk {

  template<typename T, class Hash = std::hash<T>, class Pred = std::equal_to<T>>
  class lru_list {

  public:
    typedef typename std::list<T>::const_iterator const_iterator;

    void insert(T value) {
      auto cacheIter = m_cache.find(value);
      if (cacheIter != m_cache.end())
        m_list.erase(cacheIter->second);

      m_list.push_back(value);
      auto iter = m_list.cend();
      iter--;
      m_cache[value] = iter;
    }

    void erase(const T& value) {
      auto cacheIter = m_cache.find(value);
      if (cacheIter == m_cache.end())
        return;

      m_list.erase(cacheIter->second);
      m_cache.erase(cacheIter);
    }

    const_iterator erase(const_iterator iter) {
      auto cacheIter = m_cache.find(*iter);
      m_cache.erase(cacheIter);
      return m_list.erase(iter);
    }

    void touch(const T& value) {
      auto cacheIter = m_cache.find(value);
      if (cacheIter == m_cache.end())
        return;

      m_list.erase(cacheIter->second);
      m_list.push_back(value);
      auto iter = m_list.cend();
      --iter;
      m_cache[value] = iter;
    }

    void touch(const_iterator iter) {
      T value = *iter;
      m_list.erase(iter);
      m_list.push_back(value);
      auto endIter = m_list.cend();
      --endIter;
      m_cache[value] = endIter;
    }

    const_iterator cbegin() const {
      return m_list.cbegin();
    }

    const_iterator cend() const {
      return m_list.cend();
    }

    uint32_t size() const noexcept {
      return m_list.size();
    }

  private:
    std::list<T> m_list;
    std::unordered_map<T, const_iterator, Hash, Pred> m_cache;

  };

}
