#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP

#include <unordered_map>
#include <vector>
#include <optional>

template <typename K, typename V>
class Dictionary {
private:
    std::unordered_map<K, V> map;

public:
    Dictionary() : map() {};  
    Dictionary(const std::unordered_map<K, V>& initialMap) : map(initialMap) {}

    void add(const K& key, const V& value) {
        map[key] = value;
    }

    void remove(const K& key) {
        map.erase(key);
    }

    std::optional<V> get(const K& key) const {
        auto it = map.find(key);
        if (it != map.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    std::vector<K> keys() const {
        std::vector<K> keyList;
        keyList.reserve(map.size());
        for (const auto& pair : map) {
            keyList.push_back(pair.first);
        }
        return keyList;
    }
    
    std::vector<V> values() const {
        std::vector<V> valueList;
        valueList.reserve(map.size());
        for (const auto& pair : map) {
            valueList.push_back(pair.second);
        }
        return valueList;
    }
};

#endif