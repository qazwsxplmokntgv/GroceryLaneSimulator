#pragma once
#ifndef UTILITY_H
#define UTILITY_H
#include <algorithm>
#include <array>
#include <chrono>
#include <conio.h> //_getch() utilized in menuNav() & pause()
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace utility {

	typedef struct timeInterval {
		int duration,
			start,
			end;
	}TimeInterval;

	enum menu { //underlying values are total opts
		mainMenu = 4,
		settingsMenu = 15,
		timeUnitMenu = 5
	};

	enum timeUnit {
		minute = 1,
		hour = 60,
		day = 1440,
		week = 10080
	};

	typedef struct simulationSettings {
		//toggle for including printout of grocery lists
		bool showGroceryLists = false,
			//toggle for inclusion of full queue printouts every 10 minutes
			includeQueuePrints = true,
			//toggle for inclusion of arrival and departure messages
			includeArrivalsAndDepartures = true,
			//overrides above two bools and prints only the final states of the queues
			onlyPrintFinalQueues = false,
			//toggle for tracking the highest observed time in each lane
			trackWorstTimes = true,
			//toggles for pausing and clearing after each print of the queues
			pauseOnQueue = false,
			clearOnQueue = false,
			//toggle for showing simulation execution time
			measureExecutionTime = true;
		//number of minutes before customer IDs will be recycled	
		int recycleIDInterval = 1440,
			//number of minutes between queue printouts
			queuePrintInterval = 10,
			//number of types of lanes
			laneTypeCount = 2;
		//number of each type of lane to simulate
		std::vector<int> laneCounts = { 1,1 };
		//unit used to interpret user input
		timeUnit inputUnits = minute;
	}SimulationSettings;

	//returns a timestamp week.day|hour:minute
	std::string getTimeStamp(int minute);

	//prints control info and title
	void printMenuHeader(void);

	//interprets user menu navigation inputs
	//returns any selections made by the user
	int menuNav(int currentSelection, menu currentMenu, bool& stayInMenu);

	//gets a seed for use with rng engines based on current time
	unsigned int getTimeSeed(void);

	//avoid security issues and overhead of equivalent system commands
	void pause(void);
	void clearScreen(void);

}
#endif // !UTILITY_H