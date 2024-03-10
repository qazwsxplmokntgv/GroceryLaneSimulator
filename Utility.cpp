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
			+ std::to_string(minute / week) + "." //weeks
			+ std::to_string(minute % week / day) + "|" //days
			+ (((minute % day / hour) < 10) ? "0" : "") + std::to_string(minute % day / hour) + ":" //hours
			+ (((minute % hour) < 10) ? "0" : "") + std::to_string(minute % hour) //minutes
			+ "]";
	}

	int getNumericalInput(void)
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
		int minGroc = utility::getNumericalInput();
		std::cout << "\tMax Grocery Count: ";
		while (maxGroc < minGroc) maxGroc = utility::getNumericalInput();
		std::cout << "\tMin Arrival Interval: ";
		int minArrv = utility::getNumericalInput();
		std::cout << "\tMax Arrival Interval: ";
		while (maxArrv < minArrv) maxArrv = utility::getNumericalInput();
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
		std::cout << "\033[1mZ & X / Arrow to Scroll\nSpace / Enter to Select\n"
			//title printout
			<< " _____                                 _____ _\n"
			"|  __ \\                               /  ___(_)\n"
			"| |  \\/_ __ ___   ___ ___ _ __ _   _  \\ `--. _ _ __ ___\n"
			"| | __| '__/ _ \\ / __/ _ \\ '__| | | |  `--. \\ | '_ ` _ \\\n"
			"| |_\\ \\ | | (_) | (_|  __/ |  | |_| | /\\__/ / | | | | | |\n"
			" \\____/_|  \\___/ \\___\\___|_|   \\__, | \\____/|_|_| |_| |_|\n"
			"                                __/ |\n"
			"                               |___/\n";
	}

	int menuNav(int currentSelection, int menuOpts, bool& stayInMenu)
	{
		//getting user input for menu nav
		bool gettingInput = true,
			//if the key the user entered might have been an arrow key
			mightBeArrow = false;
		while (gettingInput) {
			switch (_getch()) {
			case 0:
			case 224: //extended ascii indicators
				mightBeArrow = true;
				continue;
			case 72: //up
			case 75: //left
				if (!mightBeArrow) continue; //ignore characters corresponding to these values in unextended ascii
				mightBeArrow = false;
			case 'z': //scroll up
				--currentSelection;
				if (currentSelection < 1) currentSelection = menuOpts; //current menu underlying value is number of options
				break;
			case 80: //down
			case 77: //right
				if (!mightBeArrow) continue; //ignore unextended ascii
				mightBeArrow = false;
			case 'x': //scroll down
				++currentSelection;
				if (currentSelection > menuOpts) currentSelection = 1;
				break;
			case ' ':
			case '\n':
			case '\r': //selection
				stayInMenu = false;
				break;
			default: //invalid input, continue avoids unnecessary menu display refresh
				continue;
			}
			gettingInput = false;
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