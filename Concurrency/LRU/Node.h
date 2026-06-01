#ifndef LOWLEVELPROJECTS_NODE_H
#define LOWLEVELPROJECTS_NODE_H

template <typename K, typename V>
struct Node {
	K key;
	V value;
	Node* prev;
	Node* next;
	Node(K k, V v) {
		key = k;
		value = v;
		prev = next = nullptr;
	}
};

#endif //LOWLEVELPROJECTS_NODE_H
