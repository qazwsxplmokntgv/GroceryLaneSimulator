#include "Utility.hpp"

namespace utility {
	simulationSettings reset(simulationSettings& settings)
	{
		settings.showGroceryLists = false;
		settings.includeQueuePrints = settings.includeArrivalsAndDepartures = true;
		settings.onlyPrintFinalQueues = false;
		settings.trackWorstTimes = true;
		settings.pauseOnQueue = settings.clearOnQueue = false;
		settings.measureExecutionTime = true;
		settings.recycleIDInterval = 1440;
		settings.queuePrintInterval = 10;
		settings.laneTypeCount = 2;
		settings.laneCounts.resize(settings.laneTypeCount);
		settings.laneCounts[0] = settings.laneCounts[1] = 1;
		settings.inputUnits = minute;
		settings.laneTypeAttributes.resize(0);
		settings.laneTypeAttributes.push_back({ std::uniform_int_distribution<>(1, 35), std::uniform_int_distribution<>(1, 5), "EXPR" }); //express
		settings.laneTypeAttributes.push_back({ std::uniform_int_distribution<>(20, 60), std::uniform_int_distribution<>(3, 8), "NORM" });  //normal
		return settings;
	}
	std::string getTimeStamp(int minute)
	{
		return std::string("[")
			+ std::to_string(minute / year) + "." //months
			+ (((minute % year / month) < 10) ? "0" : "") + std::to_string(minute % year / month) + "." //months
			+ (((minute % month / day) < 10) ? "0" : "") + std::to_string(minute % month / day) + "|" //days
			+ (((minute % day / hour) < 10) ? "0" : "") + std::to_string(minute % day / hour) + ":" //hours
			+ (((minute % hour) < 10) ? "0" : "") + std::to_string(minute % hour) //minutes
			+ "]";
	}

	std::string getUnits(timeUnit unit, bool asUpper)
	{
		switch (unit) {
		case minute:
			return asUpper ? "MINUTES" : "minutes";
		case hour:
			return asUpper ? "HOURS" : "hours";
		case day:
			return asUpper ? "DAYS" : "days";
		case week:
			return asUpper ? "WEEKS" : "weeks";
		case month:
			return asUpper ? "MONTHS" : "months";
		case year:
			return asUpper ? "YEARS" : "years";
		default:
			return "ERROR";
		}
	}

	int getNumericInput(void)
	{
		int out;
		while (!std::isdigit(std::cin.peek())) std::cin.ignore(1);
		std::cin >> out;
		return out;
	}

	laneAttributeSet getLaneAttributeInput(void)
	{
		int maxGroc = 0, maxArrv = 0;
		std::cout << "\nEnter New Lane Type Information:\n";
		std::cout << "\tMin Grocery Count: ";
		int minGroc = utility::getNumericInput();
		std::cout << "\tMax Grocery Count: ";
		while (maxGroc < minGroc) maxGroc = utility::getNumericInput();
		std::cout << "\tMin Arrival Interval: ";
		int minArrv = utility::getNumericInput();
		std::cout << "\tMax Arrival Interval: ";
		while (maxArrv < minArrv) maxArrv = utility::getNumericInput();
		std::cout << "\t4 Letter Lane Code: ";
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		while (std::cin.peek() < 0) std::cin.ignore(1);
		char code[5];
		std::cin.readsome(code, 4);
		code[4] = '\0';
		return { std::uniform_int_distribution<>(minGroc, maxGroc), std::uniform_int_distribution<>(minArrv, maxArrv), code };
	}

	void printMenuHeader(void)
	{
		//menu controls
		std::cout << std::string(
			//title printout
			"\033[1mZ & X / Arrow to Scroll\nSpace / Enter to Select\n") +
			" _____                                 _____ _\n" +
			"|  __ \\                               /  ___(_)\n" +
			"| |  \\/_ __ ___   ___ ___ _ __ _   _  \\ `--. _ _ __ ___\n" +
			"| | __| '__/ _ \\ / __/ _ \\ '__| | | |  `--. \\ | '_ ` _ \\\n" +
			"| |_\\ \\ | | (_) | (_|  __/ |  | |_| | /\\__/ / | | | | | |\n" +
			" \\____/_|  \\___/ \\___\\___|_|   \\__, | \\____/|_|_| |_| |_|\n" +
			"                                __/ |\n"+
			"                               |___/\n";
	}

	int menuNav(int& currentSelection, int menuOpts, bool& stayInMenu)
	{
		//if the key the user entered might have been an arrow key
		bool mightBeArrow = false;
		while (true) {
			switch (_getch()) {
			case 0:
			case 224: //extended ascii indicators
				mightBeArrow = true;
				continue;
			case 72: //up
			case 75: //left
				if (!mightBeArrow) continue; //ignore characters corresponding to these values in unextended ascii
				mightBeArrow = false; //resets extended ascii flag once arrow is consumed
			case 'z': //scroll up
				--currentSelection;
				if (currentSelection < 1) currentSelection = menuOpts; //scrolling wraps around
				break;
			case 80: //down
			case 77: //right
				if (!mightBeArrow) continue; //ignore unextended ascii
				mightBeArrow = false; //resets flag
			case 'x': //scroll down
				++currentSelection;
				if (currentSelection > menuOpts) currentSelection = 1;
				break;
			case ' ':
			case '\n':
			case '\r': //selection
				stayInMenu = false; //exits the menu to do the action chosen by user
				break;
			default: //invalid input, continue avoids unnecessary menu display refresh
				continue;
			}
			break; //reaching this point instead of continuing means good input was recieved
		}
		return currentSelection;
	}

	int runMenu(int& currentSelection, menu& currentMenu, const simulationSettings& settings, const std::array<std::vector<std::string>, menuTypes>& menuOpts)
	{
		bool stayInMenu = true;
		//visual portion of the menu - displaying options to the user and allowing them to navigate between them
		while (stayInMenu) {
			clearScreen();

			printMenuHeader();

			//menu printouts
			switch (currentMenu) {
			case mainMenu: //menus with constant numbers of opts
			case timeUnitMenu:
				for (int i = 0; i < menuSizes[currentMenu]; ++i)
					std::cout << (currentSelection == (i + 1) ? "\033[1m > " : "\033[0m   ") << (i + 1) << menuOpts[currentMenu][i];
				break;
			case settingsMenu:
			case laneAttMenu:
				{
					const int totalOptCount = menuSizes[currentMenu] + settings.laneTypeCount;
					const int highestOptDigitCount = (int)(log10(totalOptCount) + 1);

					for (int i = 0; i < totalOptCount; ++i) {
						//numbering and selection indicator
						std::string optNum = std::to_string(i + 1);
						std::cout << (currentSelection == (i + 1) ? "\033[1m > " : "\033[0m   ") << std::setw(static_cast<unsigned long long>(highestOptDigitCount) + 1 - optNum.size()) << std::setfill(' ') << optNum;
						
						if (currentMenu == settingsMenu) {
							//print the option text
							if (i < 8) //print the first 8 predefined options normally
								std::cout << menuOpts[settingsMenu][i];
							else if (i < 8 + settings.laneTypeCount) //assemble each lane count option
								std::cout << ". " << settings.laneTypeAttributes[static_cast<std::vector<laneAttributeSet, std::allocator<laneAttributeSet>>::size_type>(i) - 8].laneName << " Lane Count                [";
							else //print the remaining options
								std::cout << menuOpts[settingsMenu][static_cast<std::vector<std::string, std::allocator<std::string>>::size_type>(i) - settings.laneTypeCount];

							//appends the state of applicable options
							//settings dependent on the states of others are locked and marked as such when said others override them
							switch (i) {
							case 0: //first 8 options (all toggles)
								std::cout << (settings.includeQueuePrints && !settings.onlyPrintFinalQueues ? "TRUE" : "FALSE") << "]\t" //status
									<< (settings.onlyPrintFinalQueues ? "[LOCKED BY 4]" : "") << "\n"; //locked indicator
								break;
							case 1:
								std::cout << (settings.includeArrivalsAndDepartures && !settings.onlyPrintFinalQueues ? "TRUE" : "FALSE") << "]\t"
									<< (settings.onlyPrintFinalQueues ? "[LOCKED BY 4]" : "") << "\n";
								break;
							case 2:
								std::cout << (settings.showGroceryLists && settings.includeArrivalsAndDepartures && !settings.onlyPrintFinalQueues ? "TRUE" : "FALSE") << "]\t"
									<< (settings.onlyPrintFinalQueues ? "[LOCKED BY 4]" : !settings.includeArrivalsAndDepartures ? "[LOCKED BY 2]" : "") << "\n";
								break;
							case 3:
								std::cout << (settings.onlyPrintFinalQueues ? "TRUE" : "FALSE") << "]\n";
								break;
							case 4:
								std::cout << (settings.trackWorstTimes ? "TRUE" : "FALSE") << "]\n";
								break;
							case 5:
								std::cout << (settings.pauseOnQueue && settings.includeQueuePrints && !settings.onlyPrintFinalQueues ? "TRUE" : "FALSE") << "]\t" 
									<< (settings.onlyPrintFinalQueues ? "[LOCKED BY 4]" : !settings.includeQueuePrints ? "[LOCKED BY 1] " : "") << "\n";
								break;
							case 6:
								std::cout << (settings.clearOnQueue && settings.includeQueuePrints && !settings.onlyPrintFinalQueues ? "TRUE" : "FALSE") << "]\t" 
									<< (settings.onlyPrintFinalQueues ? "[LOCKED BY 4]" : !settings.includeQueuePrints ? "[LOCKED BY 1] " : "") << "\n";
								break;
							case 7:
								std::cout << (settings.measureExecutionTime ? "TRUE" : "FALSE") << "]\n";
								break;
							default: //remaining options - lane counts or the three options following them
								switch (i - settings.laneTypeCount) {
								default: //lane counts
									std::cout << settings.laneCounts[static_cast<std::vector<int, std::allocator<int>>::size_type>(i) - 8] << "]\n";
									break;
								case 8: //recycle id interval
									std::cout << ((double)settings.recycleIDInterval) / settings.inputUnits << "]\n";
									break;
								case 9: //queue print interval
									std::cout << ((double)settings.queuePrintInterval) / settings.inputUnits << "]\n";
									break;
								case 10: //input units
									std::cout << (getUnits(settings.inputUnits, true)) << "]\n";
									break;
								case 11:
								case 12:
								case 13: //stateless options
									break;
								}
							}
						}
						else {
							//print the option text
							if (i < 2) //print the first 2 predefined options normally
								std::cout << menuOpts[laneAttMenu][i];
							else if (i < 2 + settings.laneTypeCount) {
								//generate each edit option
								std::cout << ". Edit " << settings.laneTypeAttributes[static_cast<std::vector<laneAttributeSet, std::allocator<laneAttributeSet>>::size_type>(i) - 2].laneName
									<< " Attributes  -  Grocery Counts: ["
									<< std::setw(2) << std::setfill('0') << (double)settings.laneTypeAttributes[static_cast<std::vector<int, std::allocator<int>>::size_type>(i) - 2].groceryCounts.param()._Min << "-"
									<< std::setw(2) << std::setfill('0') << (double)settings.laneTypeAttributes[static_cast<std::vector<int, std::allocator<int>>::size_type>(i) - 2].groceryCounts.param()._Max << "]"
									<< std::setw(2) << std::setfill(' ') << " | Arrival Times: ["
									<< std::setw(2) << std::setfill('0') << std::ceil((double)settings.laneTypeAttributes[static_cast<std::vector<int, std::allocator<int>>::size_type>(i) - 2].arrivalTimes.param()._Min * 100 /*/ settings.inputUnits*/) / 100 << "-" //currently undecided on converting units here
									<< std::setw(2) << std::setfill('0') << std::ceil((double)settings.laneTypeAttributes[static_cast<std::vector<int, std::allocator<int>>::size_type>(i) - 2].arrivalTimes.param()._Max * 100 /*/ settings.inputUnits*/) / 100 << "]\n";
							}
							else { //print back option
								std::cout << menuOpts[laneAttMenu][static_cast<std::vector<std::string, std::allocator<std::string>>::size_type>(menuSizes[laneAttMenu]) - 1];
							}
						}
					}
					break; //outermost switch (menus)
				}
			}
			//interprets user menu navigation inputs
			//will either update the menu or proceed below
			//param 2 is the size of current menu plus any options generated based on number of lane types
			menuNav(currentSelection, menuSizes[currentMenu] + (currentMenu == settingsMenu || currentMenu == laneAttMenu ? settings.laneTypeCount : 0), stayInMenu);
		}
		return currentSelection;
	}

	unsigned int getTimeSeed()
	{
		return (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
	}

	void pause()
	{
		std::cout << "Press any key to continue . . . ";
		(void)_getch();
	}

	void clearScreen()
	{
		//ansi clearscreen
		std::cout << "\033[2J\033[1;1H";
	}
}