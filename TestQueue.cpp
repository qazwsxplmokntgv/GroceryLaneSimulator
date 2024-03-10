#include "TestQueue.hpp"

void TestQueue::testEnqueueEmpty(void)
{
	mt19937 rng(utility::getTimeSeed()); //rng engine for test
	cout << "EMPTY QUEUE ENQUEUE:\n";
	//empty queue
	Queue<Data> q;
	//customer to be enqueued
	Data testCust(rng, 1, 2); 

	//testing enqueue on empty queue
	cout << "Enqueue successful? " << (q.enqueue(testCust) ? "TRUE" : "FALSE") << std::endl;
}

void TestQueue::testEnqueueNotEmpty(void)
{
	mt19937 rng(utility::getTimeSeed());
	cout << "NON EMPTY QUEUE ENQUEUE:\n";

	Queue<Data> q;
	Data cust1(rng, 1, 2);
	Data cust2(rng, 2, 2);
	q.enqueue(cust1); //q now has one customer

	//testing enqueue of second customer
	cout << "Enqueue successful? " << (q.enqueue(cust2) ? "TRUE" : "FALSE") << std::endl;
}

void TestQueue::testOneNode(void)
{
	mt19937 rng(utility::getTimeSeed());
	cout << "SINGLE NODE QUEUE DEQUEUE:\n";

	Queue<Data> q;
	Data testCust(rng, 1, 2);
	q.enqueue(testCust); //q now has one customer

	//does the data returned by dequeue match the correct customer
	cout << "Correct dequeue() return? " << (q.dequeue().getCustNum() == testCust.getCustNum() ? "TRUE" : "FALSE") << std::endl;
	//did dequeue remove testCust from queue
	cout << "Dequeue successful? " << (q.isEmpty() ? "TRUE" : "FALSE") << std::endl;
}

void TestQueue::testMultipleNodes(void)
{
	mt19937 rng(utility::getTimeSeed());
	cout << "MULTI NODE QUEUE DEQUEUE:\n";

	Queue<Data> q;
	Data cust1(rng, 1, 2);
	Data cust2(rng, 2, 500);

	q.enqueue(cust1); //cust1 is first in 
	q.enqueue(cust2); //cust2 is second in

	cout << "Correct first dequeue() return? " << (q.dequeue().getCustNum() == cust1.getCustNum() ? "TRUE" : "FALSE") << std::endl;
	cout << "Dequeue successful? " << (q.pHead == q.pTail ? "TRUE" : "FALSE") << std::endl;
	cout << "Correct second dequeue() return? " << (q.dequeue().getCustNum() == cust2.getCustNum() ? "TRUE" : "FALSE") << std::endl;
	cout << "Dequeue successful? " << (q.isEmpty() ? "TRUE" : "FALSE") << std::endl;
}

void TestQueue::testRunSim(void)
{
	cout << "24 HOUR GROCERY LANE SIM:\n";

	utility::SimulationSettings settings;

	int laneTypeCount = 2;

	std::vector<utility::LaneAttributeSet> laneTypeAttributes = {
		utility::LaneAttributeSet{ uniform_int_distribution<>(1, 35), uniform_int_distribution<>(1, 5), "EXPR" }, //express
		utility::LaneAttributeSet{ uniform_int_distribution<>(20, 60), uniform_int_distribution<>(3, 8), "NORM"}  //normal
	};

	//24 hours with default settings
	GroceryLane::runSim(24 * 60, settings, laneTypeCount, laneTypeAttributes);

	cout << "24 hour simulation completed successfully\n";
}

void TestQueue::runTestSequence(void)
{
	testEnqueueEmpty();
	std::cout << std::endl;
	utility::pause();
	utility::clearScreen();
	testEnqueueNotEmpty();
	std::cout << std::endl;
	utility::pause();
	utility::clearScreen();
	testOneNode();
	std::cout << std::endl;
	utility::pause();
	utility::clearScreen();
	testMultipleNodes();
	std::cout << std::endl;
	utility::pause();
	utility::clearScreen();
	testRunSim();
	std::cout << std::endl;
	utility::pause();
	utility::clearScreen();
}

