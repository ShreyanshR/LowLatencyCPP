//
// Created by Shreyansh Rathore on 30/05/2026.
//

#ifndef LOWLEVELPROJECTS_DOUBLYLINKEDLIST_H
#define LOWLEVELPROJECTS_DOUBLYLINKEDLIST_H
#include "Node.h"

template <typename K, typename V>
class DoublyLinkedList {
private:
	Node<K, V>* head;
	Node<K, V>* tail;
public:
	DoublyLinkedList() {
		head = new Node<K, V> (K{}, V{});
		tail = new Node<K, V> (K{}, V{});
		head->next = tail;
		tail->prev = head;
	}
	~DoublyLinkedList() {
		delete head;
		delete tail;
	}
	void addFirst(Node<K,V>* node){
		node->next = head->next;
		head->next->prev = node;
		head->next = node;
		node->prev = head;
	}
	void removeNode(Node<K,V>* node) {
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	void moveToFront(Node<K,V>* node) {
		removeNode(node);
		addFirst(node);
	}
	Node<K,V>* removeLast() {
		Node<K,V>* temp = tail->prev;
		removeNode(temp);
		return temp;
	}

};

#endif //LOWLEVELPROJECTS_DOUBLYLINKEDLIST_H
