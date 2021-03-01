/**
 * \file
 *       ui.cpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#include "ui.hpp"

#include <fstream>      // needed for "fstream"
#include <stdexcept>    // needed for exceptions





// TRANSITION UNIT FUNCTION
void executeScenario(std::string scenario_path)
{
	std::string const   file_format     = ".rwes";
	std::fstream        in_file;

	// 1. Open file and read data
	in_file.open((scenario_path.substr(scenario_path.size() - file_format.size()) == file_format) ? (scenario_path) : (scenario_path + file_format), std::fstream::in);
	if (!in_file.is_open())
		throw std::invalid_argument("Scenario was not found.");
	
	in_file.close();
	return;
}





void run(void)
{
	std::string command = "";

	// 1. Await for the next command
	while (command != "exit")
	{
		std::cout << ">>> ";
		std::cin >> command;
	}

	return;
}





void runScenario(std::string scenario_path)
{
	return;
}
