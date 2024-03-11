#include "GroceryLane.hpp"

GroceryLane::GroceryLane(const utility::laneAttributeSet& attributes)
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

void GroceryLane::runSim(unsigned int minuteTotal, utility::simulationSettings settings, int laneTypeCount, std::vector<utility::laneAttributeSet> laneAttributes)
{
	//starts execution timer
	auto simStart = std::chrono::steady_clock::now();

	if (settings.onlyPrintFinalQueues) { //disables non-final printouts 
		settings.includeArrivalsAndDepartures = settings.includeQueuePrints = false;
		std::cout << "Simulating"; //prints message to indicate the program running in the absence of in-progress printouts
	}

	//prng, referred to https://en.cppreference.com/w/cpp/numeric/random
	mt19937 rng(utility::getTimeSeed());

	std::vector<GroceryLane> lanes;

	//digits to show for each lane number, based on the largest one
	//if no duplicate lanes are to be created, = 0
	int laneNumberDigits = std::any_of(settings.laneCounts.cbegin(), settings.laneCounts.cend(), [](int count) { return count > 1; }) ? ((int)(std::log10(*std::max_element(settings.laneCounts.cbegin(), settings.laneCounts.cend()))) + 1) : 0;
	
	int laneCount = 0; //sum of each type lane count
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
	
	//trims vector size after lane creation is complete
	lanes.shrink_to_fit();

	//skips forward to align first customer arrival with simulation start
	//while every lane is still waiting for their first customer,
	while (lanes.size() != 0 && std::all_of(lanes.cbegin(), lanes.cend(), [](const GroceryLane& lane) { return (bool)lane.getNextArrivalTime(); })) {
		//evenly decrement waiting time of each lane
		for (int j = 0; j < laneCount; ++j) --lanes[j].currentNextArrivalTime;
	}

	int customersServed = 0;

	//simulation begins
	for (unsigned int minute = 0; minute < minuteTotal; ++minute) {
		if (settings.recycleIDInterval && !(minute % settings.recycleIDInterval)) customersServed = 0; //resets count regularly
		
		if (settings.onlyPrintFinalQueues) { //ellipses loading animation if intermediate printouts are omitted
			//1/4 progress || 1/2 progress || 3/4 progress
			if (minute == minuteTotal / 4 || minute == minuteTotal / 2 || minute == (3 * minuteTotal) / 4) std::cout << '.';
		}

		//for each lane
		for (int laneIdx = 0; laneIdx < laneCount; ++laneIdx) {
			lanes[laneIdx].runLaneMinute(customersServed, rng, settings, minute);
		}

		//if queue prints are enabled, print each lane's queue
		if (settings.queuePrintInterval && !(minute % settings.queuePrintInterval) && settings.includeQueuePrints) {
			for (int laneIdx = 0; laneIdx < laneCount; ++laneIdx) 
				lanes[laneIdx].printQueueWithHeader(minute);

			std::cout << std::endl;
			if (settings.pauseOnQueue) utility::pause();
			if (settings.clearOnQueue) utility::clearScreen();
		}
	}

	//concluding the simulation
	//clears "simulating..." message 
	if (settings.onlyPrintFinalQueues) utility::clearScreen();

	//final queue printout
	for (int laneIdx = 0; (settings.includeQueuePrints || settings.onlyPrintFinalQueues) && laneIdx < laneCount; ++laneIdx) 
		lanes[laneIdx].printQueueWithHeader(minuteTotal);
	std::cout << std::endl;

	std::cout << "Simulation Complete!\n\n";

	//worst time printout
	for (int laneIdx = 0; settings.trackWorstTimes && laneIdx < laneCount; ++laneIdx) {
		std::cout 
			<< lanes[laneIdx].laneName << " Highest Total Time - " << std::setw(3) << std::setfill(' ') << lanes[laneIdx].worstTime.duration << " minutes: "
			<< utility::getTimeStamp(lanes[laneIdx].worstTime.start) << "-" << utility::getTimeStamp(lanes[laneIdx].worstTime.end) << std::endl;
	}
	std::cout << std::endl;

	//stops execution timer
	auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - simStart).count();
	//execution time printout
	//minutes:seconds:milliseconds
	if (settings.measureExecutionTime) std::cout << "Execution Time: [" << std::setw(2) << std::setfill('0') << executionTime / 60000 << ":" << std::setw(2) << std::setfill('0') << (executionTime % 60000) / 1000 << ":" << std::setw(4) << std::setfill('0') << executionTime % 1000 << "]\n";
}

void GroceryLane::customerArrival(int& customersServed, std::mt19937& rng, const utility::simulationSettings& settings, const int minute)
{
	//constructs new customer 
	Data newCust;
	if (settings.showGroceryLists)
		newCust = Data(rng, ++customersServed, this->possibleGroceryCountRange(rng));
	else //uses a grocery count instead of a list if a list does not need to be generated for display
		newCust = Data(++customersServed, this->possibleGroceryCountRange(rng));

	//if this customer will immediately check out upon arrival in lane, notes how long they will be checking out
	if (this->custQueue.isEmpty())
		this->timeUntilServiceComplete = newCust.getServiceTime();

	//adds the new customer's service time to the current total wait time of the lane
	this->timeUntilEmpty += newCust.getServiceTime();
	//calculates the time the customer will spend in line
	newCust.setTotalTime(this->timeUntilEmpty);

	//tracks the worst total time of any customer in this lane
	if (settings.trackWorstTimes && this->timeUntilEmpty > this->worstTime.duration) {
		this->worstTime.duration = this->timeUntilEmpty;
		this->worstTime.start = minute;
		this->worstTime.end = minute + this->timeUntilEmpty;
	}

	//adds the customer to the queue
	this->custQueue.enqueue(newCust);
	if (settings.includeArrivalsAndDepartures) {
		//announces new customer
		std::cout << utility::getTimeStamp(minute) << " " << this->laneName << ": " << newCust.arrivalMessage();
		if (settings.showGroceryLists) {
			//displays the new customer's groceries
			std::cout << newCust.getGroceries().listToString() << std::endl;
		}
	}

	//begins count down to next arrival
	this->rollNewArrivalTime(rng);
}

void GroceryLane::customerDeparture(const utility::simulationSettings& settings, const int minute)
{
	//dequeues customer
	Data departingCust = this->custQueue.dequeue();

	if (settings.includeArrivalsAndDepartures)  //anounces departure
		std::cout << utility::getTimeStamp(minute) << " " << this->laneName << ": " << departingCust.departureMessage();

	//if there is another customer to begin serving in the queue, find how long that will take
	if (!this->custQueue.isEmpty())
		this->timeUntilServiceComplete = this->custQueue.peek().getServiceTime();
}

void GroceryLane::decrementLaneTime(void)
{
	//decrements the time until next customer arrival
	--this->currentNextArrivalTime;

	//decrements the wait time for future customers if not 0
	if (this->timeUntilEmpty)
		--this->timeUntilEmpty;

	//decrements the time until the current customer finishes being serviced if not 0
	if (this->timeUntilServiceComplete)
		--this->timeUntilServiceComplete;
}

void GroceryLane::runLaneMinute(int& customersServed, std::mt19937& rng, const utility::simulationSettings& settings, const int minute)
{
	//if a customer is due to arrive to this lane
	if (this->currentNextArrivalTime == 0)
		this->customerArrival(customersServed, rng, settings, minute);

	//if the lane finishes serving a customer
	if (!this->custQueue.isEmpty() && this->timeUntilServiceComplete == 0)
		this->customerDeparture(settings, minute);

	//decrements time values of lane as the current minute passes
	this->decrementLaneTime();
}

void GroceryLane::printQueueWithHeader(const int minute) const
{
	//header
	std::cout << std::endl << utility::getTimeStamp(minute) << " " << this->laneName << " QUEUE:\n";
	//queue
	this->custQueue.printQueue();
}

