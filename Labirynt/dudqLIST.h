#pragma once
#ifndef BETTERLIST_H
#define BETTERLIST_H

#include <iostream>
#include <stdexcept>
using namespace std;

template <typename var>
class List {
private:
	struct Node {
		var element;
		Node* next;
	};
	Node* start;
public:
	int length = 0;

	List() : start(nullptr) {}

	~List() {
		clear();
	}

	bool compare(var a, var b) const {
		return a < b;
	}

	void append(var x) {
		Node* node = new Node;
		node->element = x;
		node->next = nullptr;
		length++;

		if (start == nullptr) {
			start = node;
		}
		else {
			Node* current = start;
			while (current->next != nullptr) {
				current = current->next;
			}
			current->next = node;
		}
	}

	void unshift(var x) {
		Node* temp = new Node;
		temp->element = x;
		temp->next = start;
		start = temp;
		length++;
	}

	void print_r() const {
		Node* current = start;
		int i = 0;
		cout << "[" << endl;
		while (current != nullptr) {
			cout << " [" << i++ << "] => [" << current->element << "]" << endl;
			current = current->next;
		}
		cout << "]" << endl;
	}

	var of(int x) const {
		if (x < 0 || x >= length) {
			throw out_of_range("Index out of range");
		}
		Node* current = start;
		for (int i = 0; i < x; i++) {
			current = current->next;
		}
		return current->element;
	}

	int indexOf(var x) const {
		int i = 0;
		Node* current = start;

		while (current != nullptr) {
			if (current->element == x) {
				return i;
			}
			current = current->next;
			i++;
		}
		return -1;
	}

	void clear() {
		Node* current = start;
		Node* nextNode = nullptr;
		while (current != nullptr) {
			nextNode = current->next;
			delete current;
			current = nextNode;
		}
		start = nullptr;
		length = 0;
	}

	void deleteIndex(int x) {
		if (x < 0 || x >= length) {
			throw out_of_range("Index out of range");
		}

		Node* current = start;

		if (x == 0) {
			start = current->next;
			delete current;
			length--;
			return;
		}

		Node* prev = nullptr;
		for (int i = 0; i < x; i++) {
			prev = current;
			current = current->next;
		}

		prev->next = current->next;
		delete current;
		length--;
	}

	void deleteElement(var x) {
		if (indexOf(x) == -1) {
			throw runtime_error("Element does not exist in the list");
		}
		deleteIndex(indexOf(x));
	}

	void deleteElements(var x) {
		if (indexOf(x) == -1) {
			throw runtime_error("Element does not exist in the list");
		}
		while (indexOf(x) != -1) {
			deleteIndex(indexOf(x));
		}
	}

	Node* mergeSort(Node* head) {
		if (!head || !head->next) { return head; }
		Node* middle = getMiddle(head);
		Node* halfList = middle->next;
		middle->next = nullptr;
		Node* left = mergeSort(head);
		Node* right = mergeSort(halfList);
		return merge(left, right);
	}
	Node* merge(Node* left, Node* right) {
		if (!left) return right;
		if (!right) return left;
		Node* result = nullptr;
		if (compare(left->element, right->element)) { result = right; result->next = merge(left, right->next); }
		else { result = left; result->next = merge(left->next, right); }
		return result;
	}
	Node* getMiddle(Node* head) {
		if (!head) return head;
		Node* slow = head;
		Node* fast = head->next;
		while (fast && fast->next) { slow = slow->next; fast = fast->next->next; }
		return slow;
	}
	void sort() {
		start = mergeSort(start);
	}

	var max() {
		if (start == nullptr) {
			throw runtime_error("List is empty");
		}

		Node* current = start;
		var maxElement = current->element;

		while (current != nullptr) {
			if (compare(current->element, maxElement)) {
				maxElement = current->element;
			}
			current = current->next;
		}
		return maxElement;
	}

	var low() {
		if (start == nullptr) {
			throw runtime_error("List is empty");
		}

		Node* current = start;
		var minElement = current->element;

		while (current != nullptr) {
			if (compare(minElement, current->element)) {
				minElement = current->element;
			}
			current = current->next;
		}
		return minElement;
	}

	var last() {
		if (length == 0) {
			throw out_of_range("List is empty. Cannot get the last element.");
		}
		return of(length - 1);
	}
};

#endif // BETTERLIST_H

/*
	/// Dokumentacja


	List<type> listName;
	Tworzenie nowej pustej listy
	type (any) - typ danych np. int, float, bool, string

	listName.append(a);
	Dodanie nowego elementu na koñcu listy;
	a (any) - wartoœæ jak¹ chcemy dodaæ adekwatna do zadeklarowanego typu listy

	listName.unshift(a);
	Dodanie nowego elementu na pocz¹tku listy;
	a (any) - wartoœæ jak¹ chcemy dodaæ adekwatna do zadeklarowanego typu listy

	listName.of(i);
	Zwracanie wartoœci o wskazanym indeksie
	i (int) - numer indeksu

	listName.indexOf(a);
	Zwraca indeks elementu który jest równy podanego stringa
	a (any) - wyszukiwana wartoœæ z typem adekwatnym do zadeklarowanego typu listy

	listName.length
	Zwraca d³ugoœæ listy

	listName.print_r();
	Wypisuje zawartoœæ wraz z jej indeksami

	listName.deleteIndex(i);
	Usuwa element o podanym indeksie
	i (int) - number indeksu

	listName.deleteElement(a);
	Usuwa pierwszy element którego wartoœæ jest równa wartoœci argumentu
	a (any) - wartoœæ z typem adekwatnym do zadeklarowanego typu listy

	listName.deleteElements(a);
	Usuwa wszystkie elementy których wartoœci s¹ równe wartoœci argumentu
	a (any) - wartoœæ z typem adekwatnym do zadeklarowanego typu listy

	listName.sort();
	Sortuje podan¹ listê; dla stringów uk³ada je alfabetycznie

	listName.low();
	Zwraca najmniejsz¹ wartoœæ z listy; dla stringów zwraca pierwszy alfabetycznie

	listName.max();
	Zwraca najmniejsz¹ wartoœæ z listy; dla stringów zwraca pierwszy alfabetycznie

*/
