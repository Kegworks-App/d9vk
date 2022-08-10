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

  template<class Key, class T, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
  class lru_map {

  public:
    typedef typename std::list<std::pair<Key, T>>::const_iterator const_iterator;

    void insert(Key key, T value) {
      auto cacheIter = m_cache.find(key);
      if (cacheIter != m_cache.end())
        m_list.erase(cacheIter->second);

      m_list.push_back(std::make_pair(key, value));
      auto iter = m_list.cend();
      iter--;
      m_cache[key] = iter;
    }

    void erase(const Key& key) {
      auto cacheIter = m_cache.find(key);
      if (cacheIter == m_cache.end())
        return;

      m_list.erase(cacheIter->second);
      m_cache.erase(cacheIter);
    }

    const_iterator erase(const_iterator iter) {
      auto cacheIter = m_cache.find(iter->first);
      m_cache.erase(cacheIter);
      return m_list.erase(iter);
    }

    const_iterator find(const Key& key) {
      auto cacheIter = m_cache.find(key);
      return cacheIter != m_cache.end() ? cacheIter->second : cend();
    }

    void touch(const Key& key) {
      auto cacheIter = m_cache.find(key);
      if (cacheIter == m_cache.end())
        return;

      std::pair<Key, T> pair = *cacheIter->second;
      m_list.erase(cacheIter->second);
      m_list.push_back(pair);
      auto iter = m_list.cend();
      --iter;
      m_cache[key] = iter;
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

    void rehash(size_t buckets) {
      m_cache.rehash(buckets);
    }

  private:
    std::list<std::pair<Key, T>> m_list;
    std::unordered_map<Key, const_iterator, Hash, Pred> m_cache;

  };

}
