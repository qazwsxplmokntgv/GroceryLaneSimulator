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

void GroceryLane::setNextArrivalTime(int newArrivalTime)
{
	this->currentNextArrivalTime = newArrivalTime;
}

void GroceryLane::runSim(unsigned int minuteTotal, utility::simulationSettings& settings, int laneTypeCount, std::vector<utility::laneAttributeSet>& laneAttributes)
{
	//starts execution timer
	auto simStart = std::chrono::steady_clock::now();

	//prng, referred to https://en.cppreference.com/w/cpp/numeric/random
	mt19937 rng(utility::getTimeSeed());

	//digits to show for each lane number, based on the largest one
	//if no duplicate lanes are to be created, = 0
	int laneNumberDigits = std::any_of(settings.laneCounts.cbegin(), settings.laneCounts.cend(), [](int count) { return count > 1; }) ? ((int)(std::log10(*std::max_element(settings.laneCounts.cbegin(), settings.laneCounts.cend()))) + 1) : 0;
	
	int laneCount = 0; //sum of each type lane count
	std::for_each(settings.laneCounts.cbegin(), settings.laneCounts.cend(), [&laneCount](int count) { laneCount += count; });
	
	std::vector<GroceryLane> lanes;

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

	if (!lanes.empty()) { //if there are lanes
		//time until the first customer arrives in any lane
		int minutesUntilFirstArrival = lanes[0].currentNextArrivalTime; 
		std::for_each(lanes.cbegin() + 1, lanes.cend(), [&minutesUntilFirstArrival](const GroceryLane& lane) { if (minutesUntilFirstArrival > lane.getNextArrivalTime()) minutesUntilFirstArrival = lane.getNextArrivalTime(); });
		//skips forward to align first customer arrival with simulation start
		std::for_each(lanes.begin(), lanes.end(), [minutesUntilFirstArrival](GroceryLane& lane) { lane.setNextArrivalTime(lane.getNextArrivalTime() - minutesUntilFirstArrival); });
	}

	if (settings.onlyPrintFinalQueues) { //disables non-final printouts 
		settings.includeArrivalsAndDepartures = settings.includeQueuePrints = false;
		std::cout << "Simulating"; //prints message to indicate the program running in the absence of in-progress printouts
	}

	unsigned int customersServed = 0;

	//simulation begins
	for (unsigned int minute = 0; minute < minuteTotal; ++minute) {
		if (settings.recycleIDInterval && !(minute % settings.recycleIDInterval)) customersServed = 0; //resets count regularly
		
		//ellipses loading animation if intermediate printouts are omitted
		//1/4 progress || 1/2 progress || 3/4 progress
		if (settings.onlyPrintFinalQueues && (minute == minuteTotal >> 2 || minute == minuteTotal >> 1 || minute == (3 * minuteTotal) >> 2)) std::cout << '.';

		//simulate one minute of each lane
		std::for_each(lanes.begin(), lanes.end(), [&customersServed, &rng, &settings, minute](GroceryLane& lane) { lane.runLaneMinute(customersServed, rng, settings, minute); });

		//if queue prints are enabled, print each lane's queue
		if (settings.includeQueuePrints && settings.queuePrintInterval && !(minute % settings.queuePrintInterval)) {
			std::for_each(lanes.begin(), lanes.end(), [minute](GroceryLane& lane) { lane.printQueueWithHeader(minute); });
			std::cout << '\n';
			if (settings.pauseOnQueue) utility::pause();
			if (settings.clearOnQueue) utility::clearScreen();
		}
	}

	//concluding the simulation
	//clears "simulating..." message 
	if (settings.onlyPrintFinalQueues) utility::clearScreen();

	//final queue printout
	if (settings.includeQueuePrints || settings.onlyPrintFinalQueues)
		std::for_each(lanes.begin(), lanes.end(), [minuteTotal](GroceryLane& lane) { lane.printQueueWithHeader(minuteTotal); });

	std::cout << "\nSimulation Complete!\n\n";

	//worst time printout
	for (int laneIdx = 0; settings.trackWorstTimes && laneIdx < laneCount; ++laneIdx) {
		std::cout 
			<< lanes[laneIdx].laneName << " Highest Total Time - " << std::setw(3) << std::setfill(' ') << lanes[laneIdx].worstTime.duration << " minutes: "
			<< utility::getTimeStamp(lanes[laneIdx].worstTime.start) << "-" << utility::getTimeStamp(lanes[laneIdx].worstTime.end) << '\n';
	}
	//stops execution timer
	auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - simStart).count();
	//execution time printout
	//minutes:seconds:milliseconds
	if (settings.measureExecutionTime) std::cout << "\nExecution Time: [" << std::setw(2) << std::setfill('0') << executionTime / 60000 << ":" << std::setw(2) << std::setfill('0') << (executionTime % 60000) / 1000 << ":" << std::setw(4) << std::setfill('0') << executionTime % 1000 << "]" << std::endl;
}

void GroceryLane::customerArrival(unsigned int& customersServed, std::mt19937& rng, const utility::simulationSettings& settings, const unsigned int minute)
{
	//constructs new customer 
	Data newCust;
	if (settings.showGroceryLists)
		newCust = Data(rng, ++customersServed, this->possibleGroceryCountRange(rng));
	else //uses a grocery count instead of a list if a list does not need to be generated for display
		newCust = Data(++customersServed, this->possibleGroceryCountRange(rng));

	//if this customer will immediately begin check out upon arrival in lane, notes how long they will be checking out
	if (this->custQueue.isEmpty())
		this->timeUntilServiceComplete = newCust.getServiceTime();

	//adds the new customer's service time to the running wait time of the lane
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
			std::cout << newCust.getGroceries().listToString() << '\n';
		}
	}

	//begins count down to next arrival
	this->rollNewArrivalTime(rng);
}

void GroceryLane::customerDeparture(const utility::simulationSettings& settings, const unsigned int minute)
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

void GroceryLane::runLaneMinute(unsigned int& customersServed, std::mt19937& rng, const utility::simulationSettings& settings, const unsigned int minute)
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

void GroceryLane::printQueueWithHeader(const unsigned int minute) const
{
	//header
	std::cout << '\n' << utility::getTimeStamp(minute) << " " << this->laneName << " QUEUE:\n";
	//queue
	this->custQueue.printQueue();
}

