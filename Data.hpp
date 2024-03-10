#pragma 
#ifndef DATA_H
#define DATA_H
#include "List.hpp"
#include <iomanip>
#include <iostream>

class Data {
public:
	//constructors
	Data(mt19937& rng, int _customerNumber = 0, int groceryCount = 0);
	Data(int _customerNumber = 0, int groceryCount = 0); //'fake' grocery list for performance when actual items are never actually displayed
	Data(const Data& copy);
	//assignment
	Data& operator=(const Data& copy);
	//destructor
	~Data();

	//finds and sets service time based on grocery count
	void calculateServiceTime();
	void calculateServiceTime(int groceryCount);

	string arrivalMessage() const;
	string departureMessage() const;

	//getters
	int getCustNum(void) const;
	int getServiceTime(void) const;
	int getTotalTime(void) const;
	List getGroceries(void) const;
	//setters
	void setCustNum (int _custNum);
	void setTotalTime(int _totalTime);

private:
	int customerNumber,
		serviceTime,
		totalTime;
	List groceries;
};

std::ostream& operator<<(std::ostream& lhs, const Data& rhs);

#endif // !DATA_H
