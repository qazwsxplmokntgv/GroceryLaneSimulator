#include "GroceryLane.hpp"
#include "TestQueue.hpp"

/*
*|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*|	Name:		Dylan Smith		|\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\|
*|	Section:	1.10			|>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>|
*|	Assignment:	PA5				|////////////////////////////////////////////////////////////////////////////////////|
*|	Due Date:	3/8/24			|////////////////////////////////////////////////////////////////////////////////////|
*|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*|	Grocery Lane Sim: Heavily user-customizable simulation of an express and normal checkout lane at a grocery store.|
*|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/

int main(void) {
	utility::SimulationSettings settings;

	//current menu to display
	utility::menu currentMenu = utility::mainMenu;

	//current user menu selection
	int currentSelection = 1;

	while (true) {
		bool stayInMenu = true;
		//display and update menu until user selects something
		while (stayInMenu) {
			utility::clearScreen();

			utility::printMenuHeader();

			//menu printouts
			switch (currentMenu) {
			case utility::mainMenu:
				std::cout
					<< (currentSelection == 1 ? "\033[1m > " : "\033[0m   ") << "1. Run Simulation\n"
					<< (currentSelection == 2 ? "\033[1m > " : "\033[0m   ") << "2. Run Tests\n"
					<< (currentSelection == 3 ? "\033[1m > " : "\033[0m   ") << "3. Settings\n"
					<< (currentSelection == 4 ? "\033[1m > " : "\033[0m   ") << "4. Exit\n";
				break;
			case utility::settingsMenu:
				std::cout //toggle dependencies are reflected by menu
					<< (currentSelection ==  1 ? "\033[1m > " : "\033[0m   ") << " 1. Show Queue Printouts           [" << (settings.includeQueuePrints && !settings.onlyPrintFinalQueues ? "TRUE" : "FALSE") << "]\t" << (settings.onlyPrintFinalQueues ? "[LOCKED BY 4]" : "") << "\n"
					<< (currentSelection ==  2 ? "\033[1m > " : "\033[0m   ") << " 2. Show Arrivals & Departures     [" << (settings.includeArrivalsAndDepartures && !settings.onlyPrintFinalQueues ? "TRUE" : "FALSE") << "]\t" << (settings.onlyPrintFinalQueues ? "[LOCKED BY 4]" : "") << "\n"
					<< (currentSelection ==  3 ? "\033[1m > " : "\033[0m   ") << " 3. Show Grocery Lists On Arrival  [" << (settings.showGroceryLists && settings.includeArrivalsAndDepartures && !settings.onlyPrintFinalQueues ? "TRUE" : "FALSE") << "]\t" << (settings.onlyPrintFinalQueues ? "[LOCKED BY 4]" : !settings.includeArrivalsAndDepartures ? "[LOCKED BY 2]" : "") << "\n"
					<< (currentSelection ==  4 ? "\033[1m > " : "\033[0m   ") << " 4. Only Print Final Queue States  [" << (settings.onlyPrintFinalQueues ? "TRUE" : "FALSE") << "]\n"
					<< (currentSelection ==  5 ? "\033[1m > " : "\033[0m   ") << " 5. Track Worst Times              [" << (settings.trackWorstTimes ? "TRUE" : "FALSE") << "]\n"
					<< (currentSelection ==  6 ? "\033[1m > " : "\033[0m   ") << " 6. Pause On Queue Print           [" << (settings.pauseOnQueue && settings.includeQueuePrints && !settings.onlyPrintFinalQueues ? "TRUE" : "FALSE") << "]\t" << (settings.onlyPrintFinalQueues ? "[LOCKED BY 4]" : !settings.includeQueuePrints ? "[LOCKED BY 1] " : "") << "\n"
					<< (currentSelection ==  7 ? "\033[1m > " : "\033[0m   ") << " 7. Clear On Queue Print           [" << (settings.clearOnQueue && settings.includeQueuePrints && !settings.onlyPrintFinalQueues ? "TRUE" : "FALSE") << "]\t" << (settings.onlyPrintFinalQueues ? "[LOCKED BY 4]" : !settings.includeQueuePrints ? "[LOCKED BY 1] " : "") << "\n"
					<< (currentSelection ==  8 ? "\033[1m > " : "\033[0m   ") << " 8. Show Execution Time            [" << (settings.measureExecutionTime ? "TRUE" : "FALSE") << "]\n"
					<< (currentSelection ==  9 ? "\033[1m > " : "\033[0m   ") << " 9. Express Lane Count             [" << (settings.expressLaneCount) << "]\n"
					<< (currentSelection == 10 ? "\033[1m > " : "\033[0m   ") << "10. Normal Lane Count              [" << (settings.normalLaneCount) << "]\n"
					<< (currentSelection == 11 ? "\033[1m > " : "\033[0m   ") << "11. ID Recycling Interval          [" << ((double)settings.recycleIDInterval) / settings.inputUnits << "]\n"
					<< (currentSelection == 12 ? "\033[1m > " : "\033[0m   ") << "12. Queue Printout Interval        [" << ((double)settings.queuePrintInterval) / settings.inputUnits << "]\n"
					<< (currentSelection == 13 ? "\033[1m > " : "\033[0m   ") << "13. Input Units                    [" << (settings.inputUnits == utility::minute ? "MINUTES" : settings.inputUnits == utility::hour ? "HOURS" : settings.inputUnits == utility::day ? "DAYS" : "WEEKS") << "]\n"
					<< (currentSelection == 14 ? "\033[1m > " : "\033[0m   ") << "14. Restore Default Settings\n"
					<< (currentSelection == 15 ? "\033[1m > " : "\033[0m   ") << "15. Back\n";
				break;
			case utility::timeUnitMenu:
				std::cout
					<< (currentSelection == 1 ? "\033[1m > " : "\033[0m   ") << " 1. Minutes\n"
					<< (currentSelection == 2 ? "\033[1m > " : "\033[0m   ") << " 2. Hours\n"
					<< (currentSelection == 3 ? "\033[1m > " : "\033[0m   ") << " 3. Days\n"
					<< (currentSelection == 4 ? "\033[1m > " : "\033[0m   ") << " 4. Weeks\n"
					<< (currentSelection == 5 ? "\033[1m > " : "\033[0m   ") << " 5. Back\n";
				break;
			}
			//interprets user menu navigation inputs to determine a selected action to perform below
			currentSelection = utility::menuNav(currentSelection, currentMenu, stayInMenu);
		}

		//acts on user selection
		switch (currentMenu) {
		case utility::mainMenu:
			switch (currentSelection) {
			case 1: //run sim
				std::cout << "\nEnter Simulation Duration (" << (settings.inputUnits == utility::minute ? "minutes" : settings.inputUnits == utility::hour ? "hours" : settings.inputUnits == utility::day ? "days" : "weeks") << "): ";
				//ensure input is valid
				while (!std::isdigit(std::cin.peek())) std::cin.ignore(1);
				int simDuration;
				std::cin >> simDuration;
				simDuration *= settings.inputUnits;
				utility::clearScreen();
				GroceryLane::runSim(simDuration, settings);
				utility::pause();
				utility::clearScreen();
				break;
			case 2: //tests
				utility::clearScreen();
				TestQueue::runTestSequence();
				break;
			case 3: //settings
				currentMenu = utility::settingsMenu;
				currentSelection = 1;
				break;
			case 4: //exit program
				return 0;
			}
			break;
		case utility::settingsMenu:
			switch (currentSelection) {
			case 1: //toggles
				if (settings.onlyPrintFinalQueues) break; //toggle dependency lock - prevents hidden toggling if other setting is overriding it
				settings.includeQueuePrints = !settings.includeQueuePrints;
				break;
			case 2:
				if (settings.onlyPrintFinalQueues) break;
				settings.includeArrivalsAndDepartures = !settings.includeArrivalsAndDepartures;
				break;
			case 3:
				if (settings.onlyPrintFinalQueues || !settings.includeArrivalsAndDepartures) break;
				settings.showGroceryLists = !settings.showGroceryLists;
				break;
			case 4:
				settings.onlyPrintFinalQueues = !settings.onlyPrintFinalQueues;
				break;
			case 5:
				settings.trackWorstTimes = !settings.trackWorstTimes;
				break;
			case 6:
				if (settings.onlyPrintFinalQueues || !settings.includeQueuePrints) break;
				settings.pauseOnQueue = !settings.pauseOnQueue;
				break; 
			case 7:
				if (settings.onlyPrintFinalQueues || !settings.includeQueuePrints) break;
				settings.clearOnQueue = !settings.clearOnQueue;
				break;
			case 8:
				settings.measureExecutionTime = !settings.measureExecutionTime;
				break;
			case 9:
				std::cout << "\nEnter New Count: ";
				//ensure input is valid
				while (!std::isdigit(std::cin.peek())) std::cin.ignore(1);
				std::cin >> settings.expressLaneCount;
				break;
			case 10:
				std::cout << "\nEnter New Count: ";
				//ensure input is valid
				while (!std::isdigit(std::cin.peek())) std::cin.ignore(1);
				std::cin >> settings.normalLaneCount;
				break;
			case 11: //adjust ID recycle interval
				std::cout << "\nEnter New Interval: ";
				//ensure input is valid
				while (!std::isdigit(std::cin.peek())) std::cin.ignore(1);
				std::cin >> settings.recycleIDInterval;
				settings.recycleIDInterval *= settings.inputUnits;
				break;
			case 12: //adjust queue print interval
				std::cout << "\nEnter New Interval: ";
				//ensure input is valid
				while (!std::isdigit(std::cin.peek())) std::cin.ignore(1);
				std::cin >> settings.queuePrintInterval;
				settings.queuePrintInterval *= settings.inputUnits;
				break;
			case 13: //change unit that user input is interpreted as
				currentMenu = utility::timeUnitMenu;
				currentSelection = settings.inputUnits == utility::minute ? 1 : settings.inputUnits == utility::hour ? 2 : settings.inputUnits == utility::day ? 3 : 4;
				break;
			case 14: //reset settings
				settings.showGroceryLists = false;
				settings.includeQueuePrints = settings.includeArrivalsAndDepartures = true;
				settings.onlyPrintFinalQueues = false;
				settings.trackWorstTimes = true;
				settings.pauseOnQueue = settings.clearOnQueue = false;
				settings.measureExecutionTime = true;
				settings.recycleIDInterval = 1440;
				settings.queuePrintInterval = 10;
				settings.expressLaneCount = settings.normalLaneCount = 1;
				settings.inputUnits = utility::minute;
				break;
			case 15: 
				currentMenu = utility::mainMenu;
				currentSelection = 3;//settings menu option in main menu - where user last was before moving to submenu
				break;
			}
			break;
		case utility::timeUnitMenu:
			switch (currentSelection) {
			case 1: //minutes
				settings.inputUnits = utility::minute;
				break;
			case 2: //hours
				settings.inputUnits = utility::hour;
				break;
			case 3: //days
				settings.inputUnits = utility::day;
				break;
			case 4: //weeks
				settings.inputUnits = utility::week;
				break;
			case 5: //back
				break;
			}
			currentMenu = utility::settingsMenu;
			currentSelection = 13; //input change menu option in settings menu - where user last was before moving to submenu
			break;
		}
	}
}