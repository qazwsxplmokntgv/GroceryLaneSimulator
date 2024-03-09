#pragma once
#ifndef TEST_QUEUE_H
#define TEST_QUEUE_H
#include "GroceryLane.hpp"
#include "Queue.hpp"
#include "Utility.hpp"
using std::cout;

class TestQueue {
public:
	static void testEnqueueEmpty(void);
	static void testEnqueueNotEmpty(void);
	static void testOneNode(void);
	static void testMultipleNodes(void);
	static void testRunSim(void);

	static void runTestSequence(void);
};

#endif // !TEST_QUEUE_H
