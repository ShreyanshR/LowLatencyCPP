//
// Created by Shreyansh Rathore on 30/05/2026.
//

#ifndef LOWLEVELPROJECTS_LRUCACHE_H
#define LOWLEVELPROJECTS_LRUCACHE_H

#include <mutex>
#include <unordered_map>

#include "DoublyLinkedList.h"
#include "Node.h"

template <typename K, typename V>
class LRUCache {
private:
	int capacity;
	std::unordered_map<K, Node<K,V>*> map;
	DoublyLinkedList<K,V> list;
	std::mutex mtx;
public:
	LRUCache(int cap) : capacity(cap) {}
	~LRUCache() {
		for (auto& it: map) {
			delete it.second;
		}
	}

	std::optional<V> get(const K& key) {
		std::lock_guard<std::mutex> lk(mtx);
		auto it = map.find(key);
		if (it == map.end()) {
			return std::nullopt;
		} else {
			list.moveToFront(it->second);
			return it->second->value;
		}
	}

	void put(const K& key, const V& value) {
		std::lock_guard<std::mutex> lk(mtx);
		auto it = map.find(key);
		if (map.size() == capacity && it == map.end()) {
			Node<K,V>* lru = list.removeLast();
			map.erase(lru->key);
			delete lru;
			Node<K,V>* node = new Node<K,V>(key, value);
			list.addFirst(node);
			map[key] = node;
		} else if (it == map.end()){
			Node<K,V>* node = new Node<K,V>(key, value);
			list.addFirst(node);
			map[key] = node;
		}else {
			it->second->value = value;
			list.moveToFront(it->second);
		}
	}
};

#endif //LOWLEVELPROJECTS_LRUCACHE_H
