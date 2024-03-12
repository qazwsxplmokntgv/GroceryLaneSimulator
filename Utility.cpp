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
		std::string timeStamp("[");
		timeStamp.append(std::to_string(minute / year)); //year
		timeStamp.push_back('.');
		if ((minute % year / month) < 10) timeStamp.push_back('0'); //leading 0
		timeStamp.append(std::to_string(minute % year / month)); //month
		timeStamp.push_back('.');
		if ((minute % month / day) < 10) timeStamp.push_back('0'); //leading 0
		timeStamp.append(std::to_string(minute % month / day)); //days
		timeStamp.push_back('|');
		if ((minute % day / hour) < 10) timeStamp.push_back('0'); //leading 0
		timeStamp.append(std::to_string(minute % day / hour)); //hours
		timeStamp.push_back(':');
		if ((minute % hour) < 10) timeStamp.push_back('0'); //leading 0
		timeStamp.append(std::to_string(minute % hour)); //minutes
		timeStamp.push_back(']');
		return timeStamp;
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
		std::string out("\033[1mZ & X / Arrow to Scroll\nSpace / Enter to Select\n");
		//title printout
		out.append(" _____                                 _____ _\n");
		out.append("|  __ \\                               /  ___(_)\n");
		out.append("| |  \\/_ __ ___   ___ ___ _ __ _   _  \\ `--. _ _ __ ___\n");
		out.append("| | __| '__/ _ \\ / __/ _ \\ '__| | | |  `--. \\ | '_ ` _ \\\n");
		out.append("| |_\\ \\ | | (_) | (_|  __/ |  | |_| | /\\__/ / | | | | | |\n");
		out.append(" \\____/_|  \\___/ \\___\\___|_|   \\__, | \\____/|_|_| |_| |_|\n");
		out.append("                                __/ |\n");
		std::cout << out.append("                               |___/\n");
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
				for (int i = 0; i < menuSizes[currentMenu]; ++i) {
					std::string out;
					out.reserve(32); //prereserves out to avoid realloc
					//selection indicator
					out += (currentSelection == (i + 1) ? "\033[1m > " : "\033[0m   ");
					//numbering
					out.append(std::to_string(i + 1));
					//actual option text
					std::cout << out.append(menuOpts[currentMenu][i]);
				}
				break;
			case settingsMenu:
			case laneAttMenu:
				{
					const int totalOptCount = menuSizes[currentMenu] + settings.laneTypeCount;
					const int highestOptDigitCount = (int)(log10(totalOptCount) + 1);

					for (int i = 0; i < totalOptCount; ++i) {
						std::string optNum = std::to_string(i + 1);
						std::string out;
						out.reserve(96); //prereserves out to avoid realloc
						//selection indicator
						out += (currentSelection == (i + 1) ? "\033[1m > " : "\033[0m   ");
						//adds leading space to right align numbers
						for (int j = 0; j <= highestOptDigitCount - optNum.size(); ++j) out.push_back(' ');
						//numbering
						out.append(optNum);
						
						switch (currentMenu) {
						case settingsMenu:
							//print the option text
							if (i < 8) { //print the first 8 predefined options normally
								out += menuOpts[settingsMenu][i];
								switch (i) { 
								case 0: //first 8 option states (all toggles)
									out += (settings.includeQueuePrints && !settings.onlyPrintFinalQueues ? "TRUE]\t" : "FALSE]\t"); //status
									if (settings.onlyPrintFinalQueues) out += "[LOCKED BY 4]\n"; //locked indicator - shows that another setting is overriding this one
									else out.push_back('\n');
									break;
								case 1:
									out += (settings.includeArrivalsAndDepartures && !settings.onlyPrintFinalQueues ? "TRUE]\t" : "FALSE]\t");
									if (settings.onlyPrintFinalQueues) out += "[LOCKED BY 4]\n";
									else out.push_back('\n');
									break;
								case 2:
									out += (settings.showGroceryLists && settings.includeArrivalsAndDepartures && !settings.onlyPrintFinalQueues ? "TRUE]\t" : "FALSE]\t");
									if (settings.onlyPrintFinalQueues) out += "[LOCKED BY 4]\n";
									else if (!settings.includeArrivalsAndDepartures) out += "[LOCKED BY 2]\n";
									else out.push_back('\n');
									break;
								case 3:
									out += (settings.onlyPrintFinalQueues ? "TRUE]\n" : "FALSE]\n");
									break;
								case 4:
									out += (settings.trackWorstTimes ? "TRUE]\n" : "FALSE]\n");
									break;
								case 5:
									out += (settings.pauseOnQueue && settings.includeQueuePrints && !settings.onlyPrintFinalQueues ? "TRUE]\t" : "FALSE]\t");
									if (settings.onlyPrintFinalQueues) out += "[LOCKED BY 4]\n";
									else if (!settings.includeQueuePrints) out += "[LOCKED BY 1]\n";
									else out.push_back('\n');
									break;
								case 6:
									out += (settings.clearOnQueue && settings.includeQueuePrints && !settings.onlyPrintFinalQueues ? "TRUE]\t" : "FALSE]\t");
									if (settings.onlyPrintFinalQueues) out += "[LOCKED BY 4]\n";
									else if (!settings.includeQueuePrints) out += "[LOCKED BY 1]\n";
									else out.push_back('\n');
									break;
								case 7:
									out += (settings.measureExecutionTime ? "TRUE]\n" : "FALSE]\n");
									break;
								}
							}
							else if (i < 8 + settings.laneTypeCount) { //assemble each lane count option
								out += ". ";
								out += settings.laneTypeAttributes[(size_t)i - 8].laneName;
								out += " Lane Count                [";
								out += std::to_string(settings.laneCounts[(size_t)i - 8]);
								out += "]\n";
							}
							else { //print the remaining options
								out += menuOpts[settingsMenu][(size_t)i - settings.laneTypeCount];
								switch (i - settings.laneTypeCount) {
								case 8: //recycle id interval
									{
										std::ostringstream ss;
										ss << ((double)settings.recycleIDInterval) / settings.inputUnits;
										out += ss.str();
										out += "]\n";
									}
									break;
								case 9: //queue print interval
									{
										std::ostringstream ss;
										ss << ((double)settings.queuePrintInterval) / settings.inputUnits;
										out += ss.str();
										out += "]\n";
									}
									break;
								case 10: //input units
									out += (getUnits(settings.inputUnits, true));
									out += "]\n";
									break;
								}
							}
							break;
						case laneAttMenu:
							//print the option text
							if (i < 2) //print the first 2 predefined options normally
								out += menuOpts[laneAttMenu][i];
							else if (i < 2 + settings.laneTypeCount) {
								//generate each edit option
								out += ". Edit ";
								out += settings.laneTypeAttributes[(size_t)i - 2].laneName;
								out += " Attributes  -  Grocery Counts: [";
								out += std::to_string(settings.laneTypeAttributes[(size_t)i - 2].groceryCounts.param()._Min);
								out.push_back('-');								  
								out += std::to_string(settings.laneTypeAttributes[(size_t)i - 2].groceryCounts.param()._Max);
								out.push_back(']');								  
								out += " | Arrival Times: [";					  
								out += std::to_string(settings.laneTypeAttributes[(size_t)i - 2].arrivalTimes.param()._Min);
								out.push_back('-');								  
								out += std::to_string(settings.laneTypeAttributes[(size_t)i - 2].arrivalTimes.param()._Max);
								out += "]\n";
							}
							else { //print back option
								out += menuOpts[laneAttMenu][(size_t)menuSizes[laneAttMenu] - 1];
							}
							break;
						}
						std::cout << out; //prints for settings or lane atts
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