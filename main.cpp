#include "GroceryLane.hpp"
#include "TestQueue.hpp"

/*
*|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*| Name:       Dylan Smith     |\\\\\\\\\\\\\\\\\\\\\\\\\\|
*| Section:    1.10            |>>>>>>>>>>>>>>>>>>>>>>>>>>|
*| Assignment: PA5             |//////////////////////////|
*| Due Date:   3/8/24          |//////////////////////////|
*|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*| Grocery Lane Sim: Heavily user-customizable simulation |
*| of various checkout lanes at a grocery store.          |
*|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/

/* idea log / todo
* make the order between grocery counts and arrival times consistent across functions and menus
* backburner - make attributes in the edit menu always visually aligned based on the largest value (currently using arbitrary 2 digit fill)
*/

int main(void) {
	utility::simulationSettings settings;
	
	//current menu to display
	utility::menu currentMenu = utility::mainMenu;

	const int menuTypes = 4;
	std::array<std::vector<std::string>, menuTypes> menuOpts{ {
		{//main menu
			". Run Simulation\n",
			". Run Tests\n",
			". Settings\n",
			". Exit\n"
		},{//settings menu
			". Show Queue Printouts           [", //0
			". Show Arrivals & Departures     [", //1
			". Show Grocery Lists On Arrival  [", //2
			". Only Print Final Queue States  [", //3
			". Track Worst Times              [", //4
			". Pause On Queue Print           [", //5
			". Clear On Queue Print           [", //6
			". Show Execution Time            [", //7
			//lane count will go here
			". ID Recycling Interval          [", //8
			". Queue Printout Interval        [", //9
			". Input Units                    [", //10
			//the last three settings options do not have current states to display as part of the menu and therefore do not need the space or the bracket
			". Create & Edit Lanes\n",            //11
			". Restore Default Settings\n",       //12
			". Back\n"                            //13
		},{//units menu
			". Minutes\n",
			". Hours\n",
			". Days\n",
			". Weeks\n",
			". Back\n"
		},{//lane atts menu
			". Add New Lane Type\n",
			". Delete Lane Type\n",
			//all lanes and their attributes will be presented here, with editing options
			". Back\n"
		}
	} };

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
				for (int i = 0; i < utility::mainMenu; ++i) 
					std::cout << (currentSelection == (i + 1) ? "\033[1m > " : "\033[0m   ") << (i + 1) << menuOpts[0][i];
				break;
			case utility::settingsMenu:
				{
					const int totalOptCount = utility::settingsMenu + settings.laneTypeCount;
					const int highestOptDigitCount = (int)(log10(totalOptCount) + 1);

					for (int i = 0; i < totalOptCount; ++i) {
						//numbering and selection indicator
						std::string optNum = std::to_string(i + 1);
						std::cout << (currentSelection == (i + 1) ? "\033[1m > " : "\033[0m   ") << std::setw(static_cast<unsigned long long>(highestOptDigitCount) + 1 - optNum.size()) << std::setfill(' ') << optNum;

						//print the option text
						if (i < 8) //print the first 8 predefined options normally
							std::cout << menuOpts[1][i]; 
						else if (i < 8 + settings.laneTypeCount) //generate each lane count option
							std::cout << ". " << settings.laneTypeAttributes[static_cast<std::vector<utility::laneAttributeSet, std::allocator<utility::laneAttributeSet>>::size_type>(i) - 8].laneName << " Lane Count                [";
						else //print the remaining options normally, with numbering adjusted appropriately
							std::cout << menuOpts[1][static_cast<std::vector<std::string, std::allocator<std::string>>::size_type>(i) - settings.laneTypeCount];


						//print the state of applicable options
						//settings dependent on the states of others are locked and marked as such when said others override them
						switch (i) { //first 8 options (toggles)
						case 0:
							std::cout
								<< (settings.includeQueuePrints && !settings.onlyPrintFinalQueues ? "TRUE" : "FALSE") << "]\t"
								<< (settings.onlyPrintFinalQueues ? "[LOCKED BY 4]" : "") << "\n";
							break;
						case 1:
							std::cout
								<<(settings.includeArrivalsAndDepartures && !settings.onlyPrintFinalQueues ? "TRUE" : "FALSE") << "]\t" << (settings.onlyPrintFinalQueues ? "[LOCKED BY 4]" : "") << "\n";
							break;
						case 2:
							std::cout
								<< (settings.showGroceryLists && settings.includeArrivalsAndDepartures && !settings.onlyPrintFinalQueues ? "TRUE" : "FALSE") << "]\t" 
								<< (settings.onlyPrintFinalQueues ? "[LOCKED BY 4]" : !settings.includeArrivalsAndDepartures ? "[LOCKED BY 2]" : "") << "\n";
							break;
						case 3:
							std::cout
								<< (settings.onlyPrintFinalQueues ? "TRUE" : "FALSE") << "]\n";
							break;
						case 4:
							std::cout
								<< (settings.trackWorstTimes ? "TRUE" : "FALSE") << "]\n";
							break;
						case 5:
							std::cout
								<< (settings.pauseOnQueue && settings.includeQueuePrints && !settings.onlyPrintFinalQueues ? "TRUE" : "FALSE")
								<< "]\t" << (settings.onlyPrintFinalQueues ? "[LOCKED BY 4]" : !settings.includeQueuePrints ? "[LOCKED BY 1] " : "") << "\n";
							break;
						case 6:
							std::cout
								<< (settings.clearOnQueue && settings.includeQueuePrints && !settings.onlyPrintFinalQueues ? "TRUE" : "FALSE") 
								<< "]\t" << (settings.onlyPrintFinalQueues ? "[LOCKED BY 4]" : !settings.includeQueuePrints ? "[LOCKED BY 1] " : "") << "\n";
							break;
						case 7:
							std::cout
								<< (settings.measureExecutionTime ? "TRUE" : "FALSE") << "]\n";
							break;
						}
						//lane counts
						if (i > 7 && i <= 7 + settings.laneTypeCount) {
							std::cout << settings.laneCounts[static_cast<std::vector<int, std::allocator<int>>::size_type>(i) - 8] << "]\n";
						}
						//the three options following lane count
						switch (i - settings.laneTypeCount) {
						case 8:
							std::cout << ((double)settings.recycleIDInterval) / settings.inputUnits << "]\n";
							break;
						case 9:
							std::cout << ((double)settings.queuePrintInterval) / settings.inputUnits << "]\n";
							break;
						case 10:
							std::cout << (settings.inputUnits == utility::minute ? "MINUTES" : settings.inputUnits == utility::hour ? "HOURS" : settings.inputUnits == utility::day ? "DAYS" : "WEEKS") << "]\n";
							break;
						}
					}
				}
				break;
			case utility::timeUnitMenu:
				for (int i = 0; i < utility::timeUnitMenu; ++i) 
					std::cout << (currentSelection == (i + 1) ? "\033[1m > " : "\033[0m   ") << (i + 1) << menuOpts[2][i];
				break;
			case utility::laneAttMenu:
				{
					const int totalOptCount = utility::laneAttMenu + settings.laneTypeCount;
					const int highestOptDigitCount = (int)(log10(totalOptCount) + 1);

					for (int i = 0; i < totalOptCount; ++i) {
						//numbering and selection indicator
						std::string optNum = std::to_string(i + 1);
						std::cout << (currentSelection == (i + 1) ? "\033[1m > " : "\033[0m   ") << std::setw(static_cast<unsigned long long>(highestOptDigitCount) + 1 - optNum.size()) << std::setfill(' ') << optNum;

						//print the option text
						if (i < 2) //print the first 2 predefined options normally
							std::cout << menuOpts[3][i];
						else if (i < 2 + settings.laneTypeCount) {
							//generate each edit option
							std::cout << ". Edit " << settings.laneTypeAttributes[static_cast<std::vector<utility::laneAttributeSet, std::allocator<utility::laneAttributeSet>>::size_type>(i) - 2].laneName << " Attributes  -  Arrival Times: ["
								<< std::setw(2) << std::setfill('0') << std::ceil((double)settings.laneTypeAttributes[static_cast<std::vector<int, std::allocator<int>>::size_type>(i) - 2].arrivalTimes.param()._Min * 100 / settings.inputUnits) / 100 << "-"
								<< std::setw(2) << std::setfill('0') << std::ceil((double)settings.laneTypeAttributes[static_cast<std::vector<int, std::allocator<int>>::size_type>(i) - 2].arrivalTimes.param()._Max * 100 / settings.inputUnits) / 100 << "]" << " | Grocery Counts: ["
								<< std::setw(2) << std::setfill('0') << (double)settings.laneTypeAttributes[static_cast<std::vector<int, std::allocator<int>>::size_type>(i) - 2].groceryCounts.param()._Min << "-"
								<< std::setw(2) << std::setfill('0') << (double)settings.laneTypeAttributes[static_cast<std::vector<int, std::allocator<int>>::size_type>(i) - 2].groceryCounts.param()._Max << "]\n";
						}
						else { //print back option
							std::cout << menuOpts[3][static_cast<std::vector<std::string, std::allocator<std::string>>::size_type>(utility::laneAttMenu) - 1];
						}
					}
				}
				break;
			}

			//interprets user menu navigation inputs to determine a selected action to perform below
			currentSelection = utility::menuNav(currentSelection, currentMenu + (currentMenu == utility::settingsMenu || currentMenu == utility::laneAttMenu ? settings.laneTypeCount : 0), stayInMenu);
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
				GroceryLane::runSim(simDuration, settings, settings.laneTypeCount, settings.laneTypeAttributes);
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
				if (settings.onlyPrintFinalQueues) break; //toggle dependency based lock - prevents hidden toggling if another setting is overriding it
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
			}
			if (currentSelection >= 9 && currentSelection < 9 + settings.laneTypeCount) { //adjust counts of each type of lane, including custom lane types
				std::cout << "\nEnter New Count: ";
				//ensure input is valid
				while (!std::isdigit(std::cin.peek())) std::cin.ignore(1);
				std::cin >> settings.laneCounts[static_cast<std::vector<int, std::allocator<int>>::size_type>(currentSelection) - 9];
				break;
			}
			if (currentSelection == 9 + settings.laneTypeCount) { //adjust ID recycle interval
				std::cout << "\nEnter New Interval: ";
				//ensure input is valid
				while (!std::isdigit(std::cin.peek())) std::cin.ignore(1);
				std::cin >> settings.recycleIDInterval;
				settings.recycleIDInterval *= settings.inputUnits;
				break;
			}
			if (currentSelection == 10 + settings.laneTypeCount) { //adjust queue print interval
				std::cout << "\nEnter New Interval: ";
				//ensure input is valid
				while (!std::isdigit(std::cin.peek())) std::cin.ignore(1);
				std::cin >> settings.queuePrintInterval;
				settings.queuePrintInterval *= settings.inputUnits;
				break;
			}
			if (currentSelection == 11 + settings.laneTypeCount) { //change unit that user input is interpreted as
				currentMenu = utility::timeUnitMenu;
				currentSelection = settings.inputUnits == utility::minute ? 1 : settings.inputUnits == utility::hour ? 2 : settings.inputUnits == utility::day ? 3 : 4;
				break;
			}
			if (currentSelection == 12 + settings.laneTypeCount) { //add, edit, or delete lane types
				currentMenu = utility::laneAttMenu;
				currentSelection = 1;
			}
			if (currentSelection == 13 + settings.laneTypeCount) { //reset settings to defaults
				utility::reset(settings);
				currentSelection = 15;
				break;
			}
			if (currentSelection == 14 + settings.laneTypeCount) {
				currentMenu = utility::mainMenu;
				currentSelection = 3; //settings menu option in main menu - where user last was before moving to submenu
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
		case utility::laneAttMenu:
			switch (currentSelection) {
			case 1: //add lane type
				settings.laneTypeAttributes.push_back(utility::getLaneAttributeInput());
				settings.laneCounts.push_back(1);
				++settings.laneTypeCount;
				break;
			case 2: //delete lane type
				if (settings.laneTypeCount == 0) break;

				std::cout << "Enter a number 1-" << settings.laneTypeCount << " to delete: ";
				{
					int typeToDelete = utility::getNumericalInput() - 1;
					settings.laneTypeAttributes.erase(settings.laneTypeAttributes.begin() + typeToDelete);
					settings.laneCounts.erase(settings.laneCounts.begin() + typeToDelete);
				}
				--settings.laneTypeCount;
				break;
			default:
				if (currentSelection != utility::laneAttMenu + settings.laneTypeCount) { //edit
					settings.laneTypeAttributes.insert(settings.laneTypeAttributes.erase(settings.laneTypeAttributes.begin() + (currentSelection - 3)), utility::getLaneAttributeInput());
				}
				else { //back
					currentMenu = utility::settingsMenu;
					currentSelection = 12 + settings.laneTypeCount; //lane atts option in settings menu
				}
				break;
			}
			break;
		}
	}
}