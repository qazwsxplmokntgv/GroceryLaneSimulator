#pragma once
#ifndef UTILITY_H
#define UTILITY_H
#include <algorithm>
#include <array>
#include <chrono>
#include <conio.h> //_getch() utilized in menuNav() & pause()
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <sstream>
#include <vector>

namespace utility {

	struct laneAttributeSet {
		std::uniform_int_distribution<> 
			groceryCounts,
			arrivalTimes;
		std::string laneName;
	};

	struct timeInterval {
		int duration,
			start,
			end;
	};

	const int menuTypes = 4;
	const std::array<int, menuTypes> menuSizes = { 4, 14, 7, 3 };

	enum menu { //underlying values == total opts (excluding non-predefined opts)
		mainMenu,
		settingsMenu,
		timeUnitMenu,
		laneAttMenu
	};

	enum timeUnit {
		minute = 1,
		hour = 60,
		day = 1440,
		week = 10080, 
		month = 43800,
		year = 525600
	};

	struct simulationSettings {
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
		std::vector<laneAttributeSet> laneTypeAttributes = {
			laneAttributeSet{ std::uniform_int_distribution<>(1, 35), std::uniform_int_distribution<>(1, 5), "EXPR" }, //express
			laneAttributeSet{ std::uniform_int_distribution<>(20, 60), std::uniform_int_distribution<>(3, 8), "NORM"}  //normal
		};
	};

	simulationSettings reset(simulationSettings& settings);

	//returns a timestamp year.month.day|hour:minute
	std::string getTimeStamp(int minute);

	std::string getUnits(timeUnit unit, bool asUpper);

	int getNumericInput(void);

	laneAttributeSet getLaneAttributeInput(void);

	//prints control info and title
	void printMenuHeader(std::ostream& out);

	//interprets user menu navigation inputs
	//returns any selections made by the user
	int menuNav(int& currentSelection, int menuOpts, bool& stayInMenu);

	//continuously runs the menu until the user selects an option
	int runMenu(int& currentSelection, menu& currentMenu, const simulationSettings& settings, const std::array<std::vector<std::string>, menuTypes>& menuOpts);

	//gets a seed for use with rng engines based on current time
	unsigned int getTimeSeed(void);

	//avoid security issues and overhead of equivalent system commands
	void pause(void);
	void clearScreen(void);

}
#endif // !UTILITY_H