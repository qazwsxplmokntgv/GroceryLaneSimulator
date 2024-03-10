#include "GroceryLane.hpp"

GroceryLane::GroceryLane(const utility::LaneAttributeSet& attributes)
{
	this->custQueue = Queue<Data>();
	this->possibleGroceryCountRange = attributes.groceryCounts;
	this->nextArrivalTimeRange = attributes.arrivalTimes;
	this->currentNextArrivalTime = this->timeUntilEmpty = this->timeUntilServiceComplete = this->worstTime.duration = this->worstTime.start = this->worstTime.end = 0;
	this->laneName = attributes.laneName;
}

GroceryLane::GroceryLane(const GroceryLane& copy)
{
	this->custQueue = copy.custQueue;
	this->possibleGroceryCountRange = copy.possibleGroceryCountRange;
	this->nextArrivalTimeRange = copy.nextArrivalTimeRange;
	this->currentNextArrivalTime = copy.currentNextArrivalTime;
	this->timeUntilEmpty = copy.timeUntilEmpty;
	this->timeUntilServiceComplete = copy.timeUntilServiceComplete;
	this->worstTime = copy.worstTime;
	this->laneName = copy.laneName;
}

GroceryLane& GroceryLane::operator=(const GroceryLane& copy)
{
	this->custQueue = copy.custQueue;
	this->possibleGroceryCountRange = copy.possibleGroceryCountRange;
	this->nextArrivalTimeRange = copy.nextArrivalTimeRange;
	this->currentNextArrivalTime = copy.currentNextArrivalTime;
	this->timeUntilEmpty = copy.timeUntilEmpty;
	this->timeUntilServiceComplete = copy.timeUntilServiceComplete;
	this->worstTime = copy.worstTime;
	this->laneName = copy.laneName;
	return *this;
}

void GroceryLane::rollNewArrivalTime(mt19937& rng)
{
	currentNextArrivalTime = nextArrivalTimeRange(rng);
}

int GroceryLane::getNextArrivalTime(void) const
{
	return currentNextArrivalTime;
}

void GroceryLane::runSim(unsigned int minuteTotal, utility::SimulationSettings settings, int laneTypeCount, std::vector<utility::LaneAttributeSet> laneAttributes)
{
	auto simStart = std::chrono::steady_clock::now();

	if (settings.onlyPrintFinalQueues) {
		//disables non-final printouts 
		settings.includeArrivalsAndDepartures = settings.includeQueuePrints = false;
		std::cout << "Simulating";
	}

	//prng, referred to https://en.cppreference.com/w/cpp/numeric/random
	mt19937 rng(utility::getTimeSeed());

	std::vector<GroceryLane> lanes;

	//digits to show for each lane number, based on the largest one
	//if no duplicate lanes are to be created, = 0
	int laneNumberDigits = std::any_of(settings.laneCounts.cbegin(), settings.laneCounts.cend(), [](int count) { return count > 1; }) ? ((int)(std::log10(*std::max_element(settings.laneCounts.cbegin(), settings.laneCounts.cend()))) + 1) : 0;
	
	int laneCount = 0;
	std::for_each(settings.laneCounts.cbegin(), settings.laneCounts.cend(), [&laneCount](int count) { laneCount += count; });
	
	//creates lanes of each type
	for (int laneType = 0; laneType < laneTypeCount; ++laneType) {
		//creates each instance of a lane type
		for (int laneTypeInstanceNum = 1; laneTypeInstanceNum <= settings.laneCounts[laneType]; ++laneTypeInstanceNum) {
			GroceryLane newLane(laneAttributes[laneType]);

			//adds lane numbering if there are multiple of at least one lane type
			if (laneNumberDigits > 0) {
				string rawLaneNum(std::to_string(laneTypeInstanceNum));
				newLane.laneName.append("-");
				//adds leading 0s if necessary to keep all lane number digit counts equal
				for (int k = 0; k < laneNumberDigits - rawLaneNum.size(); ++k) newLane.laneName.append("0");
				//adds actual lane number to name following any leading 0s
				newLane.laneName.append(rawLaneNum);
			}
			//rolls first customer of this lane's arrival time
			newLane.rollNewArrivalTime(rng);
			//adds the new lane to vector containing all lanes
			lanes.push_back(newLane);
		}
	}

	//skips forward to align first customer arrival with simulation start
	//referred to https://en.cppreference.com/w/cpp/algorithm/all_any_none_of & https://en.cppreference.com/w/cpp/language/lambda
	//while every lane is still waiting for their first customer, evenly decrement waiting time of each
	while (std::all_of(lanes.cbegin(), lanes.cend(), [](GroceryLane lane) { return (bool)lane.getNextArrivalTime(); })) {
		for (int j = 0; j < laneCount; ++j) {
			--lanes[j].currentNextArrivalTime;
		}
	}

	int customersServed = 0;

	//simulation begins
	for (unsigned int i = 0; i < minuteTotal; ++i) {
		if (settings.recycleIDInterval && !(i % settings.recycleIDInterval)) customersServed = 0; //resets count every 24 hours (1440 minutes)
		
		if (settings.onlyPrintFinalQueues) { //ellipses loading animation if intermediate printouts are omitted
			//1/4 progress || 1/2 progress || 3/4 progress
			if (i == minuteTotal / 4 || i == minuteTotal / 2 || i == (3 * minuteTotal) / 4) { 
				std::cout << '.';
			}
		}

		//for each lane
		for (int j = 0; j < laneCount; ++j) {
			//if a customer is due to arrive to this lane
			if (lanes[j].currentNextArrivalTime == 0) {

				//constructs new customer 
				Data newCust(rng, ++customersServed, lanes[j].possibleGroceryCountRange(rng));

				//if this customer will immediately check out upon arrival in lane, notes how long they will be checking out
				if (lanes[j].custQueue.isEmpty()) 
					lanes[j].timeUntilServiceComplete = newCust.getServiceTime();

				//adds the new customer's service time to the current total wait time of the lane
				lanes[j].timeUntilEmpty += newCust.getServiceTime();
				//calculates the time the customer will spend in line
				newCust.setTotalTime(lanes[j].timeUntilEmpty);

				//tracks the worst total time of any customer in this lane
				if (settings.trackWorstTimes && lanes[j].timeUntilEmpty > lanes[j].worstTime.duration) {
					lanes[j].worstTime.duration = lanes[j].timeUntilEmpty;
					lanes[j].worstTime.start = i;
					lanes[j].worstTime.end = i + lanes[j].timeUntilEmpty;
				}

				//adds the customer to the queue
				lanes[j].custQueue.enqueue(newCust);
				if (settings.includeArrivalsAndDepartures) {
					//announces new customer
					std::cout << utility::getTimeStamp(i) << " " << lanes[j].laneName << ": " << newCust.arrivalMessage();
					if (settings.showGroceryLists) {
						//displays the new customer's groceries
						std::cout << newCust.getGroceries().listToString() << std::endl;
					}
				}

				//begins count down to next arrival
				lanes[j].rollNewArrivalTime(rng);
			}

			//if the lane finishes serving a customer
			if (!lanes[j].custQueue.isEmpty() && lanes[j].timeUntilServiceComplete == 0) {
				//dequeues customer
				Data departingCust = lanes[j].custQueue.dequeue();
				if (settings.includeArrivalsAndDepartures) {
					//anounces departure
					std::cout << utility::getTimeStamp(i) << " " << lanes[j].laneName << ": " << departingCust.departureMessage();
				}
				//if there is another customer to begin serving in the queue, find how long that will take
				if (!lanes[j].custQueue.isEmpty())
					lanes[j].timeUntilServiceComplete = lanes[j].custQueue.peek().getServiceTime();
			}

			//decrements the time until next customer arrival
			--lanes[j].currentNextArrivalTime;
			//decrements the wait time for future customers if not 0
			if(lanes[j].timeUntilEmpty) --lanes[j].timeUntilEmpty;
			//decrements the time until the current customer finishes being serviced if not 0
			if (lanes[j].timeUntilServiceComplete) --lanes[j].timeUntilServiceComplete;
		}

		if (settings.queuePrintInterval && !(i % settings.queuePrintInterval) && settings.includeQueuePrints) {
			for (int j = 0; j < laneCount; ++j) {
				std::cout << std::endl;
				std::cout << utility::getTimeStamp(i) << " " << lanes[j].laneName << " QUEUE" << ":\n";
				lanes[j].custQueue.printQueue();
			}
			std::cout << std::endl;
			if (settings.pauseOnQueue) utility::pause();
			if (settings.clearOnQueue) utility::clearScreen();
		}
	}
	std::cout << std::endl;

	//clears "simulating..." message 
	if (settings.onlyPrintFinalQueues) utility::clearScreen();

	//final queue printout
	for (int i = 0; (settings.includeQueuePrints || settings.onlyPrintFinalQueues) && i < laneCount; ++i) {
		std::cout << utility::getTimeStamp(minuteTotal) << " " << lanes[i].laneName << " QUEUE" << ":\n";
		lanes[i].custQueue.printQueue();
		std::cout << std::endl;
	}

	std::cout << "Simulation Complete!\n\n";

	//worst time printout
	for (int i = 0; settings.trackWorstTimes && i < laneCount; ++i) {
		std::cout 
			<< lanes[i].laneName << " Highest Total Time - " << lanes[i].worstTime.duration << " minutes: " 
			<< utility::getTimeStamp(lanes[i].worstTime.start) << "-" << utility::getTimeStamp(lanes[i].worstTime.end) << std::endl;
	}
	std::cout << std::endl;

	//referred to https://en.cppreference.com/w/cpp/header/chrono
	auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - simStart).count();
	//minutes:seconds:milliseconds
	if (settings.measureExecutionTime) std::cout << "Execution Time: [" << std::setw(2) << std::setfill('0') << executionTime / 60000 << ":" << std::setw(2) << std::setfill('0') << (executionTime % 60000) / 1000 << ":" << std::setw(4) << std::setfill('0') << executionTime % 1000 << "]\n";
}

