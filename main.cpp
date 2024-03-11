#include "GroceryLane.hpp"
#include "TestQueue.hpp"

/*
*|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*| Written By:  Dylan Smith    |\\\\\\\\\\\\\\\\\\\\\\\\\\|
*| Last Edited: 3/10/24        |//////////////////////////|
*|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*| Grocery Lane Sim: Heavily user-customizable simulation |
*| of various checkout lanes at a grocery store.          |
*|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/

/* idea log / todo
* fix crash when destructing extremely large queues
* backburner - make attributes in the edit menu always visually aligned based on the largest value (currently using arbitrary 2 digit fill)
*/

int main(void) {
	utility::simulationSettings settings;
	
	//current menu to display
	utility::menu currentMenu = utility::mainMenu;

	//current user menu selection
	int currentSelection = 1;

	std::array<std::vector<std::string>, 4> menuOpts{ {
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
			//the last three settings options do not have current states to display as part of the menu and therefore do not need the spacing or bracket
			". Create & Edit Lanes\n",            //11
			". Restore Default Settings\n",       //12
			". Back\n"                            //13
		},{//units menu
			". Minutes\n",
			". Hours\n",
			". Days\n",
			". Weeks\n",
			". Months\n",
			". Years\n",
			". Back\n"
		},{//lane atts menu
			". Add New Lane Type\n",
			". Delete Lane Type\n",
			//all lanes and their attributes will be presented here, with editing options
			". Back\n"
		}
	} };


	for (std::vector<std::string>& optList : menuOpts) optList.shrink_to_fit();

	//program continues to run until the user selects exit from the main menu (mainMenu opt #4)
	while (true) {
		//continuously runs the menu until the user selects an option
		currentSelection = utility::runMenu(currentSelection, currentMenu, settings, menuOpts);

		//acts on user selection
		switch (currentMenu) {
		case utility::mainMenu:
			switch (currentSelection) {
			case 1: //run sim
				std::cout << "\nEnter Simulation Duration (" << utility::getUnits(settings.inputUnits, false) << "): ";
				{
					int simDuration = utility::getNumericInput() * settings.inputUnits;
					utility::clearScreen();
					GroceryLane::runSim(simDuration, settings, settings.laneTypeCount, settings.laneTypeAttributes);
				}
				utility::pause();
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
			default: 
				switch (currentSelection - settings.laneTypeCount) {
				default: //adjust counts of each type of lane, including custom lane types
					std::cout << "\nEnter New Count: ";
					settings.laneCounts[static_cast<std::vector<int, std::allocator<int>>::size_type>(currentSelection) - 9] = utility::getNumericInput();
					break;
				case 9: //adjust ID recycle interval
				case 10: //adjust queue print interval
					std::cout << "\nEnter New Interval: ";
					(currentSelection - settings.laneTypeCount == 9 ? settings.recycleIDInterval : settings.queuePrintInterval) = utility::getNumericInput();
					break;
				case 11: //change unit that user input is interpreted as
					currentMenu = utility::timeUnitMenu;
					currentSelection = settings.inputUnits == utility::minute ? 1 : settings.inputUnits == utility::hour ? 2 : settings.inputUnits == utility::day ? 3 : settings.inputUnits == utility::week ? 4 : settings.inputUnits == utility::month ? 5 : 6;
					break;
				case 12: //add, edit, or delete lane types
					currentMenu = utility::laneAttMenu;
					currentSelection = 1;
					break;
				case 13: //reset settings to defaults
					utility::reset(settings);
					currentSelection = 15;
					break;
				case 14: //back
					currentMenu = utility::mainMenu;
					currentSelection = 3; //settings menu option in main menu - where user last was before moving to submenu
					break;
				}
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
			case 5: //months
				settings.inputUnits = utility::month;
				break;
			case 6: //years
				settings.inputUnits = utility::year;
				break;
			case 7: //back
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
				{
					if (settings.laneTypeCount == 0) break;
					int typeToDelete = 0;
					if (settings.laneTypeCount > 1) {
						std::cout << "Select a number 1-" << settings.laneTypeCount << " to delete: ";
						typeToDelete = utility::getNumericInput() - 1;
					}
					if (typeToDelete > settings.laneTypeCount) break;
					settings.laneTypeAttributes.erase(settings.laneTypeAttributes.begin() + typeToDelete);
					settings.laneCounts.erase(settings.laneCounts.begin() + typeToDelete);
				}
				--settings.laneTypeCount;
				break;
			default:
				if (currentSelection != utility::menuSizes[utility::laneAttMenu] + settings.laneTypeCount) { //edit
					settings.laneTypeAttributes[static_cast<std::vector<utility::laneAttributeSet, std::allocator<utility::laneAttributeSet>>::size_type>(currentSelection) - 3] = utility::getLaneAttributeInput();
				}
				else { //back
					settings.laneCounts.shrink_to_fit();
					settings.laneTypeAttributes.shrink_to_fit();
					currentMenu = utility::settingsMenu;
					currentSelection = 12 + settings.laneTypeCount; //lane atts option in settings menu
				}
				break;
			}
			break;
		}
	}
}