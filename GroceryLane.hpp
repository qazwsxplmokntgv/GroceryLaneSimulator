#pragma once
#ifndef GROCERY_LANE_H
#define GROCERY_LANE_H
#include "Data.hpp"
#include "Queue.hpp"
#include "Utility.hpp"
using std::uniform_int_distribution;

class GroceryLane {
public:
	GroceryLane(const utility::LaneAttributeSet& attributes);
	GroceryLane(const GroceryLane& copy);
	GroceryLane& operator=(const GroceryLane& copy);
	~GroceryLane() {};

	//set currentNextArrivalTime based on nextArrivalTimeRange
	void rollNewArrivalTime(mt19937& rng);

	int getNextArrivalTime(void) const;

	static void runSim(unsigned int minuteTotal, utility::SimulationSettings settings, int laneTypeCount, std::vector<utility::LaneAttributeSet> laneTypeAttributes);

private:
	//queue of customers currently present in lane
	Queue<Data> custQueue;
	//possible number of groceries a customer of this lane can have
	uniform_int_distribution<> possibleGroceryCountRange;
	//possible number of minutes until the arrival of the next customer
	uniform_int_distribution<> nextArrivalTimeRange;	
	//minutes until the arrival of the next customer
	int currentNextArrivalTime,
		//minutes until, if no more customers are enqueued, the lane will be empty
		timeUntilEmpty,
		//minutes until the current front customer finishes checking out
		timeUntilServiceComplete;
	//worst observed total time for this lane
	utility::TimeInterval worstTime;

	//name of the lane
	std::string laneName;
};

#endif // !GROCERY_LANE_H
