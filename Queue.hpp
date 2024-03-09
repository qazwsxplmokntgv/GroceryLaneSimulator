#pragma once 
#ifndef QUEUE_H
#define QUEUE_H
#include "Node.hpp"
#include <iostream>

class TestQueue;

template <class T>
class Queue {
	friend TestQueue;
public:
	//constructors
	Queue(Node<T>* _pHead = nullptr);
	Queue(const Queue& copy);
	//assignment
	Queue& operator=(const Queue& copy);
	//destructor
	~Queue();
	

	//returns whether queue is empty
	bool isEmpty() const;
	//returns the first node without modifying queue
	T peek(void) const;
	//adds new data to end of queue
	//returns success/failure
	bool enqueue(const T& data);
	//removes node at front of queue
	//returns data of said node
	T dequeue(void);
	//displays all entries of queue
	void printQueue() const;
	void printQueue(const Node<T>* const startNode) const;

	Node<T>* getHead(void) const;

private:
	Node<T>* pHead;
	Node<T>* pTail;
};

template<typename T>
inline Queue<T>::Queue(Node<T>* _pHead)
{
	this->pHead = _pHead;
	this->pTail = _pHead;
	while (pHead != nullptr && pTail->getNext() != nullptr) {
		pTail = pTail->getNext();
	}
}

template<typename T>
inline Queue<T>::Queue(const Queue& copy)
{
	Node<T>* pCopyCurr = copy.pHead;
	while (pCopyCurr != nullptr) {
		enqueue(pCopyCurr->getData());
		pCopyCurr = pCopyCurr->getNext();
	}
}

template<typename T>
inline Queue<T>& Queue<T>::operator=(const Queue& copy)
{
	this->pHead = copy.pHead;
	this->pTail = copy.pTail;
	return *this;
}

template<typename T>
inline Queue<T>::~Queue()
{
	if (!isEmpty())
		delete pHead;
}

template<typename T>
inline bool Queue<T>::isEmpty() const
{
	return pHead == nullptr;
}

template<typename T>
inline T Queue<T>::peek(void) const
{
	return pHead->getData();
}

template<typename T>
inline bool Queue<T>::enqueue(const T& data)
{
	//allocates new node being enqueued
	Node<T>* newNode = new Node<T>(data);

	//if allocation fails
	if (newNode == nullptr) return false;

	if (isEmpty()) {
		//if queue is empty, new node also becomes new head
		pHead = newNode;
	}
	else {
		//if a tail exists, newNode now follows it
		pTail->setNext(newNode);
	}
	//newNode is now the last node in queue
	pTail = newNode;


	return true;
}

template<typename T>
inline T Queue<T>::dequeue(void)
{
	//stores ptr to node to be dequeued
	Node<T>* deqNode = pHead;

	//updates pHead
	pHead = pHead->getNext();

	//prevents nodes following deqNode from being deleted by destructor
	deqNode->setNext(nullptr);

	//prevents pTail from dangling
	if (isEmpty()) pTail = nullptr;

	//deallocs deqNode, returning its data
	T deqData(deqNode->getData());
	delete deqNode;
	return deqData;
}

template<typename T>
inline void Queue<T>::printQueue() const
{
	if (isEmpty()) {
		std::cout << "\t    (empty)\n";
		return;
	}
	printQueue(pHead);
}

template<typename T>
inline void Queue<T>::printQueue(const Node<T>* const startNode) const
{
	//if queue end is reached
	if (startNode == nullptr) return;
	//prints data
	std::cout << "\t    " << startNode->getData() << std::endl;
	//recursive call
	printQueue(startNode->getNext());
}

template<typename T>
inline Node<T>* Queue<T>::getHead(void) const
{
	return pHead;
}

#endif // !QUEUE_H
