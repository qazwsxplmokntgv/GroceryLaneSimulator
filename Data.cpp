#include "Data.hpp"

Data::Data(mt19937& rng, int _customerNumber, int _groceryCount)
{
	this->customerNumber = _customerNumber;
	this->groceries = List(_groceryCount, rng);
	this->calculateServiceTime();
	this->totalTime = 0;
	this->groceryCount = _groceryCount;
}

Data::Data(int _customerNumber, int _groceryCount)
{
	this->customerNumber = _customerNumber;
	this->groceries = List();
	this->calculateServiceTime(_groceryCount);
	this->totalTime = 0;
	this->groceryCount = _groceryCount;
}

Data::Data(const Data& copy)
{
	this->customerNumber = copy.customerNumber;
	this->serviceTime = copy.serviceTime;
	this->totalTime = copy.totalTime;
	this->groceries = copy.groceries;
	this->groceryCount = copy.groceryCount;
}

Data& Data::operator=(const Data& copy)
{
	this->customerNumber = copy.customerNumber;
	this->serviceTime = copy.serviceTime;
	this->totalTime = copy.totalTime;
	this->groceries = copy.groceries;
	this->groceryCount = copy.groceryCount;
	return *this;
}

Data::~Data() {}

void Data::calculateServiceTime()
{
	const int groceriesPerMinute = 8;
	serviceTime = (int)ceil((double)groceries.getSize() / groceriesPerMinute);
}

void Data::calculateServiceTime(int groceryCount)
{
	const int groceriesPerMinute = 8;
	serviceTime = (int)ceil((double)groceryCount / groceriesPerMinute);
}

string Data::arrivalMessage() const
{
	return "Customer " + std::string((6 - std::to_string(customerNumber).length()), '0') + std::to_string(customerNumber) + " arrives: " + std::to_string(groceryCount) + " groceries (" + std::to_string(serviceTime) + " minutes).\n";
}

string Data::departureMessage() const
{
	return "Customer " + std::string((6 - std::to_string(customerNumber).length()), '0') + std::to_string(customerNumber) + " departs: " + std::to_string(totalTime) + " total minutes elapsed since arrival.\n";
}

int Data::getCustNum(void) const
{
	return customerNumber;
}

int Data::getServiceTime(void) const
{
	return serviceTime;
}

int Data::getTotalTime(void) const
{
	return totalTime;
}

List Data::getGroceries(void) const
{
	return groceries;
}

void Data::setCustNum(int _custNum)
{
	this->customerNumber = _custNum;
}

void Data::setTotalTime(int _totalTime)
{
	this->totalTime = _totalTime;
}

std::ostream& operator<<(std::ostream& lhs, const Data& rhs)
{
	lhs << "Customer " << std::setw(6) << std::setfill('0') << rhs.getCustNum() << " - " << "Service: " << rhs.getServiceTime() << " minutes | Total: " << rhs.getTotalTime() << " minutes";
	return lhs;
}