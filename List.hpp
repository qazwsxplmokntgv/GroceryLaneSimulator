#pragma once 
#ifndef LIST_H
#define LIST_H
#include "Node.hpp"
#include <random>
#include <string>
using std::string;
using std::mt19937;

class List {
public:
	List(Node<string>* _pHead = nullptr, int _size = 0);
	List(int groceryCount, mt19937& rng); //constructs list filled with groceryCount # of random groceries
	List(const List& copy);
	List& operator=(const List& copy);
	~List();

	//populates list with groceryCount groceries
	void populateWithGroceries(int groceryCount, mt19937& rng);

	bool isEmpty() const;
	//adds node of data to front of list
	bool insertAtFront(const string& data);
	//adds node of data to end of list
	bool insertAtEnd(const string& data);

	string listToString(void) const;

	int getSize(void) const;
	Node<string>* getHead(void) const;

private:
	Node<string>* pHead;
	Node<string>* pTail;
	int size;
};

//returns a grocery from a set list
string genGrocery(mt19937& rng);

#endif // !LIST_H
