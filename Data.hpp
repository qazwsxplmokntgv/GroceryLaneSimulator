#pragma 
#ifndef DATA_H
#define DATA_H
#include "List.hpp"
#include <iomanip>
#include <iostream>

class Data {
public:
	//constructors
	//serviceTime is initialized based on groceryCount
	//totalTime is initialized to be equal to service time
	//groceries is initialized to a list of random groceries of groceryCount size
	Data(mt19937& rng, int _customerNumber = 0, int groceryCount = 0);
	Data(int _customerNumber = 0, int _serviceTime = 0, int _totalTime = 0, List _groceries = List()) : customerNumber(_customerNumber), serviceTime(_serviceTime), totalTime(_totalTime), groceries(_groceries) {}
	Data(const Data& copy);
	//assignment
	Data& operator=(const Data& copy);
	//destructor
	~Data();

	//finds and sets service time based on grocery count
	void calculateServiceTime();

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
