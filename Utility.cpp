#include "Utility.hpp"

namespace utility {
	std::string getTimeStamp(int minute)
	{
		return std::string("[")
			+ std::to_string(minute / week) + "." //weeks
			+ std::to_string(minute % week / day) + "|" //days
			+ (((minute % day / hour) < 10) ? "0" : "") + std::to_string(minute % day / hour) + ":" //hours
			+ (((minute % hour) < 10) ? "0" : "") + std::to_string(minute % hour) //minutes
			+ "]";
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

	int menuNav(int currentSelection, menu currentMenu, bool& stayInMenu)
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
				if (currentSelection < 1) currentSelection = currentMenu; //current menu underlying value is number of options
				break;
			case 80: //down
			case 77: //right
				if (!mightBeArrow) continue; //ignore unextended ascii
				mightBeArrow = false;
			case 'x': //scroll down
				++currentSelection;
				if (currentSelection > currentMenu) currentSelection = 1;
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