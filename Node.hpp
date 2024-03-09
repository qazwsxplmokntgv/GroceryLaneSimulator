#pragma once
#ifndef NODE_H
#define NODE_H

template <typename T>
class Node {
public:
	//constructors
	Node(const T& _data, Node<T>* _pNext = nullptr) : data(_data), pNext(_pNext) {}
	Node(const Node<T>& copy);
	//assignment
	Node<T>& operator=(const Node<T>& copy);
	//destructor
	~Node();

	//getters
	const T& getData(void) const;
	Node<T>* getNext(void) const;
	//setters
	void setData(const T& _data);
	void setNext(Node<T>* _pNext);

private:
	T data;
	Node* pNext;
};


template<typename T>
inline Node<T>::Node(const Node& copy)
{
	this->data = T(copy.data);
	this->pNext = new Node<T>(copy.data);
}

template<typename T>
inline Node<T>& Node<T>::operator=(const Node<T>& copy)
{
	this->data = T(copy.data);
	this->pNext = copy.pNext;
	return *this;
}

template<typename T>
inline Node<T>::~Node()
{
	if (pNext != nullptr) {
		delete pNext;
		pNext = nullptr;
	}
}

template<typename T>
inline const T& Node<T>::getData(void) const
{
	return this->data;
}

template<typename T>
inline Node<T>* Node<T>::getNext(void) const
{
	return this->pNext;
}

template<typename T>
inline void Node<T>::setData(const T& _data)
{
	this->data = _data;
}

template<typename T>
inline void Node<T>::setNext(Node<T>* _pNext)
{
	this->pNext = _pNext;
}

#endif // !NODE_H